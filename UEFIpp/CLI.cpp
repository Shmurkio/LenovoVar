#include "CLI.hpp"
#include "Lenovo.hpp"
#include "Cast.hpp"

enum class COMMAND
{
	Invalid,
	Help,
	Version,
	List,
	Get,
	Set
};

typedef struct _OPTIONS
{
	COMMAND Command = COMMAND::Invalid;
	Lenovo::PENTRY_KEY Key = nullptr;
	PCSTR KeyName = nullptr;
	PCSTR Value = nullptr;
} OPTIONS, *POPTIONS;

static constexpr PCSTR APP_NAME = "LenovoVar";
static constexpr PCSTR APP_VERSION = "1.0.0";

static auto StringsEqual(PCSTR Str1, PCSTR Str2) -> bool
{
	if (!Str1 || !Str2) return false;

	while (*Str1 && *Str2)
	{
		if (*Str1 != *Str2) return false;
		++Str1;
		++Str2;
	}

	return (*Str1 == '\0' && *Str2 == '\0');
}

static auto StringLen(PCSTR Str) -> UINT64
{
	if (!Str) return 0;

	UINT64 Length = 0;

	while (Str[Length]) ++Length;

	return Length;
}

static auto ResolveKeyByName(PCSTR Name) -> Lenovo::PCENTRY_KEY
{
	if (!Name) return nullptr;

	if (StringsEqual(Name, "mtm"))				return &Lenovo::SMBIOS_ENTRY_KEY_MACHINE_TYPE_MODEL;
	if (StringsEqual(Name, "motherboardname"))	return &Lenovo::SMBIOS_ENTRY_KEY_MOTHERBOARD_NAME;
	if (StringsEqual(Name, "serialnumber"))		return &Lenovo::SMBIOS_ENTRY_KEY_BASEBOARD_SERIAL_NUMBER;
	if (StringsEqual(Name, "uuid"))				return &Lenovo::SMBIOS_ENTRY_KEY_SYSTEM_UUID;
	if (StringsEqual(Name, "platformid"))		return &Lenovo::SMBIOS_ENTRY_KEY_BASEBOARD_PLATFORM_ID;
	if (StringsEqual(Name, "ossufix"))			return &Lenovo::SMBIOS_ENTRY_KEY_OS_PRELOAD_SUFFIX;
	if (StringsEqual(Name, "oa3"))				return &Lenovo::SMBIOS_ENTRY_KEY_OA3_KEY_ID;
	//if (StringsEqual(Name, "winkey"))			return &Lenovo::SMBIOS_ENTRY_KEY_WINDOWS_KEY;

	return nullptr;
}

static auto IsUuidKey(Lenovo::PCENTRY_KEY Key) -> bool
{
	return Key == &Lenovo::SMBIOS_ENTRY_KEY_SYSTEM_UUID;
}

static auto IsHexChar(char Char) -> bool
{
	return
		(Char >= '0' && Char <= '9') ||
		(Char >= 'a' && Char <= 'f') ||
		(Char >= 'A' && Char <= 'F');
}

static auto HexCharToValue(char Char) -> UINT8
{
	if (Char >= '0' && Char <= '9')
	{
		return static_cast<UINT8>(Char - '0');
	}

	if (Char >= 'a' && Char <= 'f')
	{
		return static_cast<UINT8>(Char - 'a' + 10);
	}

	if (Char >= 'A' && Char <= 'F')
	{
		return static_cast<UINT8>(Char - 'A' + 10);
	}

	return 0xFF;
}

static auto ParseUuidString(PCSTR String, UINT8(&Bytes)[16]) -> bool
{
	if (!String)
	{
		return false;
	}

	if (StringLen(String) != 36)
	{
		return false;
	}

	if (String[8] != '-' || String[13] != '-' || String[18] != '-' || String[23] != '-')
	{
		return false;
	}

	UINT8 Parsed[16] = { 0 };
	UINT64 ParsedIndex = 0;

	for (UINT64 i = 0; i < 36;)
	{
		if (String[i] == '-')
		{
			++i;
			continue;
		}

		if ((i + 1) >= 36)
		{
			return false;
		}

		if (!IsHexChar(String[i]) || !IsHexChar(String[i + 1]))
		{
			return false;
		}

		UINT8 High = HexCharToValue(String[i]);
		UINT8 Low = HexCharToValue(String[i + 1]);

		if (High == 0xFF || Low == 0xFF || ParsedIndex >= 16)
		{
			return false;
		}

		Parsed[ParsedIndex++] = static_cast<UINT8>((High << 4) | Low);
		i += 2;
	}

	if (ParsedIndex != 16)
	{
		return false;
	}

	Bytes[0] = Parsed[3];
	Bytes[1] = Parsed[2];
	Bytes[2] = Parsed[1];
	Bytes[3] = Parsed[0];
	Bytes[4] = Parsed[5];
	Bytes[5] = Parsed[4];
	Bytes[6] = Parsed[7];
	Bytes[7] = Parsed[6];
	Bytes[8] = Parsed[8];
	Bytes[9] = Parsed[9];
	Bytes[10] = Parsed[10];
	Bytes[11] = Parsed[11];
	Bytes[12] = Parsed[12];
	Bytes[13] = Parsed[13];
	Bytes[14] = Parsed[14];
	Bytes[15] = Parsed[15];

	return true;
}

static auto PrintHexByte(UINT8 Byte) -> VOID
{
	static constexpr char HexDigits[] = "0123456789ABCDEF";

	char Buffer[3] = { 0 };
	Buffer[0] = HexDigits[(Byte >> 4) & 0x0F];
	Buffer[1] = HexDigits[Byte & 0x0F];
	Buffer[2] = '\0';

	Console::Out << Buffer;
}

static auto PrintUuidBytes(PCUINT8 Bytes, UINT32 Size) -> VOID
{
	if (!Bytes || Size != 16)
	{
		Console::Out << "Invalid UUID size: " << Size << Console::Endl;
		return;
	}

	Console::Out << "UUID: ";

	PrintHexByte(Bytes[3]);
	PrintHexByte(Bytes[2]);
	PrintHexByte(Bytes[1]);
	PrintHexByte(Bytes[0]);
	Console::Out << '-';

	PrintHexByte(Bytes[5]);
	PrintHexByte(Bytes[4]);
	Console::Out << '-';

	PrintHexByte(Bytes[7]);
	PrintHexByte(Bytes[6]);
	Console::Out << '-';

	PrintHexByte(Bytes[8]);
	PrintHexByte(Bytes[9]);
	Console::Out << '-';

	PrintHexByte(Bytes[10]);
	PrintHexByte(Bytes[11]);
	PrintHexByte(Bytes[12]);
	PrintHexByte(Bytes[13]);
	PrintHexByte(Bytes[14]);
	PrintHexByte(Bytes[15]);

	Console::Out << Console::Dec << Console::Endl;
}

static auto PrintBanner() -> VOID
{
	Console::Out << APP_NAME << " " << APP_VERSION << Console::Endl;
}

static auto PrintUsage() -> VOID
{
	PrintBanner();

	Console::Out << Console::Endl
		<< "Usage:" << Console::Endl
		<< "  " << APP_NAME << ".efi --help" << Console::Endl
		<< "  " << APP_NAME << ".efi --version" << Console::Endl
		<< "  " << APP_NAME << ".efi list" << Console::Endl
		<< "  " << APP_NAME << ".efi get --key <name>" << Console::Endl
		<< "  " << APP_NAME << ".efi set --key <name> --value <value>" << Console::Endl
		<< Console::Endl
		<< "Options:" << Console::Endl
		<< "  -h, --help       Show this help message" << Console::Endl
		<< "  -v, --version    Show version information" << Console::Endl
		<< "  --key <name>     Select entry key" << Console::Endl
		<< "  --value <value>  Value to write" << Console::Endl
		<< Console::Endl
		<< "Keys:" << Console::Endl
		<< "  mtm             Machine Type Model" << Console::Endl
		<< "  motherboardname Motherboard Name" << Console::Endl
		<< "  serialnumber    Baseboard Serial Number" << Console::Endl
		<< "  uuid            System UUID" << Console::Endl
		<< "  platformid      Baseboard Platform ID" << Console::Endl
		<< "  ossufix         OS Preload Suffix" << Console::Endl
		<< "  oa3             OA3 Key ID" << Console::Endl
		<< Console::Endl
		<< "UUID format:" << Console::Endl
		<< "  " << APP_NAME << ".efi set --key uuid --value 00112233-4455-6677-8899-AABBCCDDEEFF" << Console::Endl;
}

static auto PrintKeyList() -> VOID
{
	Console::Out
		<< "Available keys:" << Console::Endl
		<< "  mtm             Machine Type Model" << Console::Endl
		<< "  motherboardname Motherboard Name" << Console::Endl
		<< "  serialnumber    Baseboard Serial Number" << Console::Endl
		<< "  uuid            System UUID" << Console::Endl
		<< "  platformid      Baseboard Platform ID" << Console::Endl
		<< "  ossufix         OS Preload Suffix" << Console::Endl
		<< "  oa3             OA3 Key ID" << Console::Endl;
		//<< "  winkey          Windows Key" << Console::Endl;
}

static auto ParseCommand(PCSTR Arg) -> COMMAND
{
	if (!Arg) return COMMAND::Invalid;

	if (StringsEqual(Arg, "--help") || StringsEqual(Arg, "-h"))		return COMMAND::Help;
	if (StringsEqual(Arg, "--version") || StringsEqual(Arg, "-v"))	return COMMAND::Version;
	if (StringsEqual(Arg, "list"))									return COMMAND::List;
	if (StringsEqual(Arg, "get"))									return COMMAND::Get;
	if (StringsEqual(Arg, "set"))									return COMMAND::Set;
	return COMMAND::Invalid;
}

static auto ParseOption(PCSTR* ArgV, UINT64 ArgC, OPTIONS& Options) -> EFI_STATUS
{
	if (!ArgV || ArgC < 2)
	{
		Options.Command = COMMAND::Help;
		return EFI_SUCCESS;
	}

	for (UINT64 i = 0; i < ArgC; ++i)
	{
		if (StringsEqual(ArgV[i], "--help") || StringsEqual(ArgV[i], "-h"))
		{
			Options.Command = COMMAND::Help;
			return EFI_SUCCESS;
		}

		if (StringsEqual(ArgV[i], "--version") || StringsEqual(ArgV[i], "-v"))
		{
			Options.Command = COMMAND::Version;
			return EFI_SUCCESS;
		}
	}

	Options.Command = ParseCommand(ArgV[1]);

	if (Options.Command == COMMAND::Invalid)
	{
		Console::Out << "Unknown command: " << ArgV[1] << Console::Endl;
		return EFI_INVALID_PARAMETER;
	}

	for (UINT64 i = 2; i < ArgC; ++i)
	{
		auto Arg = ArgV[i];

		if (StringsEqual(Arg, "--key"))
		{
			if ((i + 1) >= ArgC)
			{
				Console::Out << "Missing value for --key" << Console::Endl;
				return EFI_INVALID_PARAMETER;
			}

			Options.KeyName = ArgV[++i];
			Options.Key = const_cast<Lenovo::PENTRY_KEY>(ResolveKeyByName(Options.KeyName));

			if (!Options.Key)
			{
				Console::Out << "Unknown key: " << Options.KeyName << Console::Endl;
				return EFI_INVALID_PARAMETER;
			}

			continue;
		}

		if (StringsEqual(Arg, "--value"))
		{
			if ((i + 1) >= ArgC)
			{
				Console::Out << "Missing value for --value" << Console::Endl;
				return EFI_INVALID_PARAMETER;
			}

			Options.Value = ArgV[++i];
			continue;
		}

		Console::Out << "Unknown option: " << Arg << Console::Endl;
		return EFI_INVALID_PARAMETER;
	}

	switch (Options.Command)
	{
	case COMMAND::Help:
	case COMMAND::Version:
	case COMMAND::List:
	{
		return EFI_SUCCESS;
	}
	case COMMAND::Get:
	{
		if (!Options.Key)
		{
			Console::Out << "The get command requires --key <name>" << Console::Endl;
			return EFI_INVALID_PARAMETER;
		}

		return EFI_SUCCESS;
	}
	case COMMAND::Set:
	{
		if (!Options.Key)
		{
			Console::Out << "The set command requires --key <name>" << Console::Endl;
			return EFI_INVALID_PARAMETER;
		}

		if (!Options.Value)
		{
			Console::Out << "The set command requires --value <value>" << Console::Endl;
			return EFI_INVALID_PARAMETER;
		}

		return EFI_SUCCESS;
	}

	default:
	{
		return EFI_INVALID_PARAMETER;
	}
	}
}

static auto LocateLenovoVariableProtocol(Lenovo::PLENOVO_VARIABLE_PROTOCOL& Protocol) -> EFI_STATUS
{
	Protocol = nullptr;

	auto Status = gBS->LocateProtocol(&Lenovo::LENOVO_VARIABLE_PROTOCOL_GUID, nullptr, Cast::To<PVOID*>(&Protocol));

	if (EfiError(Status)) Console::Out << "Failed to locate Lenovo Variable Protocol: 0x" << Console::Hex << Status << Console::Dec << Console::Endl;

	return Status;
}

static auto ReadEntry(Lenovo::PLENOVO_VARIABLE_PROTOCOL Protocol, Lenovo::PENTRY_KEY Key) -> EFI_STATUS
{
	if (!Protocol || !Key) return EFI_INVALID_PARAMETER;

	CHAR Buffer[0x1000] = { 0 };
	UINT32 Size = sizeof(Buffer);

	auto Status = Protocol->GetEntryByKey(Protocol, Key, &Size, Buffer);

	if (EfiError(Status))
	{
		Console::Out << "GetEntryByKey failed: 0x" << Console::Hex << Status << Console::Dec << Console::Endl;
		return Status;
	}

	Console::Out << "Size: 0x" << Console::Hex << Size << Console::Dec << Console::Endl;

	if (IsUuidKey(Key))
	{
		Console::Out << "Data: ";
		PrintUuidBytes(reinterpret_cast<PCUINT8>(Buffer), Size);
		return EFI_SUCCESS;
	}

	Console::Out << "Data: " << Buffer << Console::Endl;

	return EFI_SUCCESS;
}

static auto WriteEntry(Lenovo::PLENOVO_VARIABLE_PROTOCOL Protocol, Lenovo::PENTRY_KEY Key, PCSTR Value) -> EFI_STATUS
{
	if (!Protocol || !Key || !Value) return EFI_INVALID_PARAMETER;

	if (IsUuidKey(Key))
	{
		UINT8 UuidBytes[16] = { 0 };

		if (!ParseUuidString(Value, UuidBytes))
		{
			Console::Out << "Invalid UUID format. Expected XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX" << Console::Endl;
			return EFI_INVALID_PARAMETER;
		}

		auto Status = Protocol->SetEntryByKey(Protocol, Key, sizeof(UuidBytes), UuidBytes);

		if (EfiError(Status))
		{
			Console::Out << "SetEntryByKey failed: 0x" << Console::Hex << Status << Console::Dec << Console::Endl;
			return Status;
		}

		Console::Out << "UUID write successful" << Console::Endl;
		return EFI_SUCCESS;
	}

	auto Size = Cast::To<UINT32>(StringLen(Value)); // Data is stored without null terminator

	auto Status = Protocol->SetEntryByKey(Protocol, Key, Size, reinterpret_cast<PVOID>(const_cast<PSTR>(Value)));

	if (EfiError(Status))
	{
		Console::Out << "SetEntryByKey failed: 0x" << Console::Hex << Status << Console::Dec << Console::Endl;
		return Status;
	}

	Console::Out << "Write successful" << Console::Endl;
	return EFI_SUCCESS;
}

auto CLI::Run(PCSTR* ArgV, UINT64 ArgC) -> EFI_STATUS
{
	OPTIONS Options = { };

	auto Status = ParseOption(ArgV, ArgC, Options);

	if (EfiError(Status))
	{
		Console::Out << Console::Endl;
		PrintUsage();
		return Status;
	}

	switch (Options.Command)
	{
	case COMMAND::Help:
	{
		PrintUsage();
		return EFI_SUCCESS;
	}
	case COMMAND::Version:
	{
		PrintBanner();
		return EFI_SUCCESS;
	}
	case COMMAND::List:
	{
		PrintKeyList();
		return EFI_SUCCESS;
	}
	case COMMAND::Get:
	case COMMAND::Set:
	{
		break;
	}
	default:
	{
		PrintUsage();
		return EFI_INVALID_PARAMETER;
	}
	}

	Lenovo::PLENOVO_VARIABLE_PROTOCOL Protocol = nullptr;

	Status = LocateLenovoVariableProtocol(Protocol);

	if (EfiError(Status))
	{
		return Status;
	}

	switch (Options.Command)
	{
	case COMMAND::Get:
	{
		return ReadEntry(Protocol, Options.Key);
	}
	case COMMAND::Set:
	{
		return WriteEntry(Protocol, Options.Key, Options.Value);
	}
	default:
	{
		return EFI_INVALID_PARAMETER;
	}
	}
}