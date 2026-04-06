#include "CLI.hpp"
#include "Lenovo.hpp"
#include "Cast.hpp"
#include "Uefi//Detour/Detour.hpp"
#include "Loader.hpp"
#include "Pe.hpp"

Detour<Lenovo::AppendDmiChangeRecordFn> gAppendDmiChangeRecordDetour;

enum class COMMAND
{
	Invalid,
	Help,
	Version,
	List,
	DriverInfo,
	Get,
	Set
};

typedef struct _OPTIONS
{
	COMMAND Command = COMMAND::Invalid;
	Lenovo::PENTRY_KEY Key = nullptr;
	PCSTR KeyName = nullptr;
	PCSTR Value = nullptr;
	BOOLEAN NoLog = false;
} OPTIONS, *POPTIONS;

static constexpr PCSTR APP_NAME			= "LenovoVar";
static constexpr PCSTR APP_VERSION		= "1.0.1";
static constexpr PCSTR APP_SOURCE_URL	= "https://github.com/Shmurkio/LenovoVar";

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
		return Cast::To<UINT8>(Char - '0');
	}

	if (Char >= 'a' && Char <= 'f')
	{
		return Cast::To<UINT8>(Char - 'a' + 10);
	}

	if (Char >= 'A' && Char <= 'F')
	{
		return Cast::To<UINT8>(Char - 'A' + 10);
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

		Parsed[ParsedIndex++] = Cast::To<UINT8>((High << 4) | Low);
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

	Console::Out << Console::Dec;
}

static auto Max(UINT64 A, UINT64 B) -> UINT64
{
	return (A > B) ? A : B;
}

static auto PrintRepeated(char Char, UINT64 Count) -> VOID
{
	for (UINT64 i = 0; i < Count; ++i)
	{
		Console::Out << Char;
	}
}

static auto PrintCenteredLineParts(PCSTR A, PCSTR B, UINT64 Width, UINT64 Padding) -> VOID
{
	auto LenA = StringLen(A);
	auto LenB = StringLen(B);
	auto TotalLen = LenA + 1 + LenB;

	auto TotalPadding = Width - TotalLen;

	auto PaddingLeft = TotalPadding / 2;
	auto PaddingRight = TotalPadding - PaddingLeft;

	Console::Out << "|";
	PrintRepeated(' ', Padding + PaddingLeft);

	Console::Out << A << ' ' << B;

	PrintRepeated(' ', Padding + PaddingRight);
	Console::Out << "|" << Console::Endl;
}

static auto PrintCenteredLine(PCSTR Text, UINT64 Width, UINT64 Padding) -> VOID
{
	auto Len = StringLen(Text);
	auto TotalPadding = Width - Len;

	auto PaddingLeft = TotalPadding / 2;
	auto PaddingRight = TotalPadding - PaddingLeft;

	Console::Out << "|";
	PrintRepeated(' ', Padding + PaddingLeft);

	Console::Out << Text;

	PrintRepeated(' ', Padding + PaddingRight);
	Console::Out << "|" << Console::Endl;
}

static auto PrintBanner() -> VOID
{
	auto Padding = 20;

	auto AppNameLen = StringLen(APP_NAME);
	auto AppVersionLen = StringLen(APP_VERSION);
	auto SourceUrlLen = StringLen(APP_SOURCE_URL);

	auto FirstLineLen = AppNameLen + 1 + AppVersionLen;
	auto ContentWidth = Max(FirstLineLen, SourceUrlLen);

	auto TotalWidth = ContentWidth + (Padding * 2);

	Console::Out << "+";
	PrintRepeated('-', TotalWidth);
	Console::Out << "+" << Console::Endl;

	PrintCenteredLineParts(APP_NAME, APP_VERSION, ContentWidth, Padding);
	PrintCenteredLine(APP_SOURCE_URL, ContentWidth, Padding);

	Console::Out << "+";
	PrintRepeated('-', TotalWidth);
	Console::Out << "+" << Console::Endl << Console::Endl;
}

static auto PrintUsage() -> VOID
{
	Console::Out
		<< "Usage:" << Console::Endl
		<< "  " << APP_NAME << ".efi --help" << Console::Endl
		<< "  " << APP_NAME << ".efi --version" << Console::Endl
		<< "  " << APP_NAME << ".efi list" << Console::Endl
		<< "  " << APP_NAME << ".efi driverinfo" << Console::Endl
		<< "  " << APP_NAME << ".efi get --key <name>" << Console::Endl
		<< "  " << APP_NAME << ".efi set --key <name> --value <value>" << Console::Endl
		<< "  " << APP_NAME << ".efi set --key <name> --value <value> --nolog" << Console::Endl
		<< Console::Endl

		<< "Commands:" << Console::Endl
		<< "  list             List all available keys" << Console::Endl
		<< "  get              Read the value of a key" << Console::Endl
		<< "  set              Write a value to a key" << Console::Endl
		<< "  driverinfo       Show LenovoVariableDxe driver information" << Console::Endl
		<< Console::Endl

		<< "Options:" << Console::Endl
		<< "  -h, --help       Show this help message" << Console::Endl
		<< "  -v, --version    Show version information" << Console::Endl
		<< "  --key <name>     Select entry key" << Console::Endl
		<< "  --value <value>  Value to write" << Console::Endl
		<< "  --nolog          Disable LDBG changelog writes (if supported)" << Console::Endl
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

		<< "UUID Format:" << Console::Endl
		<< "  " << APP_NAME << ".efi set --key uuid --value 00112233-4455-6677-8899-AABBCCDDEEFF" << Console::Endl;
}

static auto PrintKeyList() -> VOID
{
	Console::Out
		<< "Available Keys:" << Console::Endl
		<< "  mtm             Machine Type Model" << Console::Endl
		<< "  motherboardname Motherboard Name" << Console::Endl
		<< "  serialnumber    Baseboard Serial Number" << Console::Endl
		<< "  uuid            System UUID" << Console::Endl
		<< "  platformid      Baseboard Platform ID" << Console::Endl
		<< "  ossufix         OS Preload Suffix" << Console::Endl
		<< "  oa3             OA3 Key ID" << Console::Endl;
}

static auto ParseCommand(PCSTR Arg) -> COMMAND
{
	if (!Arg) return COMMAND::Invalid;

	if (StringsEqual(Arg, "--help") || StringsEqual(Arg, "-h"))		return COMMAND::Help;
	if (StringsEqual(Arg, "--version") || StringsEqual(Arg, "-v"))	return COMMAND::Version;
	if (StringsEqual(Arg, "list"))									return COMMAND::List;
	if (StringsEqual(Arg, "driverinfo"))							return COMMAND::DriverInfo;
	if (StringsEqual(Arg, "get"))									return COMMAND::Get;
	if (StringsEqual(Arg, "set"))									return COMMAND::Set;
	return COMMAND::Invalid;
}

static auto IsFlagOption(PCSTR Arg) -> bool
{
	if (!Arg) return false;

	return
		StringsEqual(Arg, "--help") ||
		StringsEqual(Arg, "-h") ||
		StringsEqual(Arg, "--version") ||
		StringsEqual(Arg, "-v") ||
		StringsEqual(Arg, "--nolog");
}

static auto IsValueOption(PCSTR Arg) -> bool
{
	if (!Arg) return false;

	return
		StringsEqual(Arg, "--key") ||
		StringsEqual(Arg, "--value");
}

static auto IsKnownOption(PCSTR Arg) -> bool
{
	return IsFlagOption(Arg) || IsValueOption(Arg);
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
			if ((i + 1) >= ArgC || IsKnownOption(ArgV[i + 1]))
			{
				Console::Out << "Missing value for --key" << Console::Endl;
				return EFI_INVALID_PARAMETER;
			}

			Options.KeyName = ArgV[++i];
			Options.Key = Cast::To<Lenovo::PENTRY_KEY>(ResolveKeyByName(Options.KeyName));

			if (!Options.Key)
			{
				Console::Out << "Unknown key: " << Options.KeyName << Console::Endl;
				return EFI_INVALID_PARAMETER;
			}

			continue;
		}

		if (StringsEqual(Arg, "--value"))
		{
			if ((i + 1) >= ArgC || IsKnownOption(ArgV[i + 1]))
			{
				Console::Out << "Missing value for --value" << Console::Endl;
				return EFI_INVALID_PARAMETER;
			}

			Options.Value = ArgV[++i];
			continue;
		}

		if (StringsEqual(Arg, "--nolog"))
		{
			Options.NoLog = true;
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
	case COMMAND::DriverInfo:
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

static EFI_STATUS __fastcall AppendDmiChangeRecordHook(PVOID Context, PVOID Key, UINT8 Operation, UINT32 Size)
{
	Console::Out << "  LDBG changelog write blocked" << Console::Endl;
	return EFI_SUCCESS;
}

static auto DisableLdbgChangelogWrites() -> EFI_STATUS
{
	PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
	EFI_HANDLE ImageHandle = nullptr;

	auto Status = Loader::GetImageByGuid(Lenovo::LENOVO_VARIABLE_DXE_GUID, LoadedImage, ImageHandle);

	if (EfiError(Status))
	{
		return Status;
	}

	auto ImageBase = Cast::To<CUINT64>(LoadedImage->ImageBase);
	auto ImageSize = LoadedImage->ImageSize;

	Lenovo::AppendDmiChangeRecordFn* AppendDmiChangeRecord = nullptr;

	Status = Util::FindPattern(ImageBase, ImageSize, Lenovo::APPEND_DMI_CHANGE_RECORD_SIGNATURE, AppendDmiChangeRecord);

	if (EfiError(Status))
	{
		return Status;
	}

	Status = gAppendDmiChangeRecordDetour.Attach(AppendDmiChangeRecord, AppendDmiChangeRecordHook, true);

	if (EfiError(Status))
	{
		return Status;
	}

	return EFI_SUCCESS;
}

static auto ReadEntry(Lenovo::PLENOVO_VARIABLE_PROTOCOL Protocol, Lenovo::PENTRY_KEY Key, BOOLEAN NoLog = false) -> EFI_STATUS
{
	Console::Out << "Read Entry:" << Console::Endl;

	if (!Protocol || !Key)
	{
		Console::Out << "  Invalid parameters" << Console::Endl;
		return EFI_INVALID_PARAMETER;
	}

	if (NoLog)
	{
		auto Status = DisableLdbgChangelogWrites();

		if (EfiError(Status))
		{
			Console::Out << "  Failed to disable LDBG changelog writes" << Console::Endl;
			return Status;
		}
	}

	auto ReenableLdbgChangelogWrites = [&]()
	{
		if (NoLog)
		{
			gAppendDmiChangeRecordDetour.Detach();
		}
	};

	CHAR Buffer[0x1000] = { 0 };
	UINT32 Size = sizeof(Buffer);

	auto Status = Protocol->GetEntryByKey(Protocol, Key, &Size, Buffer);

	if (EfiError(Status))
	{
		Console::Out << "  GetEntryByKey failed: 0x" << Console::Hex << Status << Console::Dec << Console::Endl;
		ReenableLdbgChangelogWrites();
		return Status;
	}

	Console::Out << "  Size: 0x" << Console::Hex << Size << Console::Dec << Console::Endl;

	if (IsUuidKey(Key))
	{
		Console::Out << "  Data: ";
		PrintUuidBytes(Cast::To<PCUINT8>(Buffer), Size);
		Console::Out << Console::Endl;
		ReenableLdbgChangelogWrites();
		return EFI_SUCCESS;
	}

	Console::Out << "  Data: " << Buffer << Console::Endl;
	ReenableLdbgChangelogWrites();
	return EFI_SUCCESS;
}

static auto WriteEntry(Lenovo::PLENOVO_VARIABLE_PROTOCOL Protocol, Lenovo::PENTRY_KEY Key, PCSTR Value, BOOLEAN NoLog = false) -> EFI_STATUS
{
	Console::Out << "Write Entry:" << Console::Endl;

	if (!Protocol || !Key || !Value)
	{
		Console::Out << "  Invalid parameters" << Console::Endl;
		return EFI_INVALID_PARAMETER;
	}

	if (NoLog)
	{
		auto Status = DisableLdbgChangelogWrites();

		if (EfiError(Status))
		{
			Console::Out << "  Failed to disable LDBG changelog writes" << Console::Endl;
			return Status;
		}
		else
		{
			Console::Out << "  LDBG changelog writes disabled" << Console::Endl;
		}
	}

	auto ReenableLdbgChangelogWrites = [&]()
	{
		if (NoLog)
		{
			gAppendDmiChangeRecordDetour.Detach();
		}
	};

	if (IsUuidKey(Key))
	{
		UINT8 UuidBytes[16] = { 0 };

		if (!ParseUuidString(Value, UuidBytes))
		{
			Console::Out << "  Invalid UUID format. Expected XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX" << Console::Endl;
			ReenableLdbgChangelogWrites();
			return EFI_INVALID_PARAMETER;
		}

		auto Status = Protocol->SetEntryByKey(Protocol, Key, sizeof(UuidBytes), UuidBytes);

		if (EfiError(Status))
		{
			Console::Out << "  SetEntryByKey failed: 0x" << Console::Hex << Status << Console::Dec << Console::Endl;
			ReenableLdbgChangelogWrites();
			return Status;
		}

		Console::Out << "  Write successful" << Console::Endl;
		ReenableLdbgChangelogWrites();
		return EFI_SUCCESS;
	}

	auto Size = Cast::To<UINT32>(StringLen(Value));

	auto Status = Protocol->SetEntryByKey(Protocol, Key, Size, Cast::To<PVOID>(Value));

	if (EfiError(Status))
	{
		Console::Out << "  SetEntryByKey failed: 0x" << Console::Hex << Status << Console::Dec << Console::Endl;
		ReenableLdbgChangelogWrites();
		return Status;
	}

	Console::Out << "  Write successful" << Console::Endl;
	ReenableLdbgChangelogWrites();
	return EFI_SUCCESS;
}

static auto PrintDriverInfo() -> EFI_STATUS
{
	Console::Out << "Driver Information:" << Console::Endl;

	PEFI_LOADED_IMAGE_PROTOCOL LoadedImage = nullptr;
	EFI_HANDLE ImageHandle = nullptr;

	auto Status = Loader::GetImageByGuid(Lenovo::LENOVO_VARIABLE_DXE_GUID, LoadedImage, ImageHandle);

	if (EfiError(Status))
	{
		Console::Out << "  Driver not found:    0x" << Console::Hex << Status << Console::Dec << Console::Endl;
		return Status;
	}

	Console::Out
		<< "  Image Base:          0x" << Console::Hex << LoadedImage->ImageBase << Console::Endl
		<< "  Image Size:          0x" << LoadedImage->ImageSize << Console::Endl
		<< "  Image Handle:        0x" << ImageHandle << Console::Dec << Console::Endl;

	PIMAGE_DOS_HEADER DosHeader = nullptr;
	PIMAGE_NT_HEADERS64 NtHeaders = nullptr;

	Status = Pe::GetHeader(LoadedImage->ImageBase, DosHeader, NtHeaders);

	if (EfiError(Status))
	{
		Console::Out << "  PE Header invalid:  0x" << Console::Hex << Status << Console::Dec << Console::Endl;
		return EFI_SUCCESS;
	}

	auto EntryPointVa = Cast::To<UINT64>(LoadedImage->ImageBase) + NtHeaders->OptionalHeader.AddressOfEntryPoint;

	Console::Out
		<< "  Machine:             0x" << Console::Hex << NtHeaders->FileHeader.Machine << Console::Endl
		<< "  Sections:            0x" << NtHeaders->FileHeader.NumberOfSections << Console::Endl
		<< "  Timestamp:           0x" << NtHeaders->FileHeader.TimeDateStamp << Console::Endl
		<< "  Characteristics:     0x" << NtHeaders->FileHeader.Characteristics << Console::Endl
		<< "  Entry RVA:           0x" << NtHeaders->OptionalHeader.AddressOfEntryPoint << Console::Endl
		<< "  Entry VA:            0x" << EntryPointVa << Console::Endl
		<< "  Preferred Base:      0x" << NtHeaders->OptionalHeader.ImageBase << Console::Endl
		<< "  SizeOfImage:         0x" << NtHeaders->OptionalHeader.SizeOfImage << Console::Endl
		<< "  SizeOfHeaders:       0x" << NtHeaders->OptionalHeader.SizeOfHeaders << Console::Endl
		<< "  Subsystem:           0x" << NtHeaders->OptionalHeader.Subsystem << Console::Endl
		<< "  DllChars:            0x" << NtHeaders->OptionalHeader.DllCharacteristics << Console::Dec << Console::Endl;

	auto Section = IMAGE_FIRST_SECTION(NtHeaders);

	Console::Out << "  Sections:" << Console::Endl;

	for (UINT16 i = 0; i < NtHeaders->FileHeader.NumberOfSections; ++i)
	{
		char Name[9] = { 0 };

		for (UINT32 j = 0; j < 8; ++j)
		{
			Name[j] = Cast::To<char>(Section[i].Name[j]);
		}

		Console::Out
			<< "    Section " << i << ":         "
			<< "Name=\"" << Name << "\""
			<< " | VA=0x" << Console::Hex << Section[i].VirtualAddress
			<< " | VS=0x" << Section[i].Misc.VirtualSize
			<< " | RAW=0x" << Section[i].SizeOfRawData << Console::Dec
			<< Console::Endl;
	}

	Lenovo::PLENOVO_VARIABLE_PROTOCOL Protocol = nullptr;

	Status = gBS->LocateProtocol(&Lenovo::LENOVO_VARIABLE_PROTOCOL_GUID, nullptr, Cast::To<PVOID*>(&Protocol));

	if (EfiError(Status))
	{
		Console::Out << "  Protocol not found:   0x" << Console::Hex << Status << Console::Dec << Console::Endl;
		return Status;
	}

	Console::Out
		<< "  Protocol Address:    0x" << Console::Hex << Protocol << Console::Endl
		<< "  GetEntryByKey:       0x" << Protocol->GetEntryByKey << Console::Endl
		<< "  SetEntryByKey:       0x" << Protocol->SetEntryByKey << Console::Endl
		<< "  ProtectEntryByKey:   0x" << Protocol->ProtectEntryByKey << Console::Endl
		<< "  UnprotectEntryByKey: 0x" << Protocol->UnprotectEntryByKey << Console::Dec << Console::Endl;

	return EFI_SUCCESS;
}

auto PrintVersionInfo() -> EFI_STATUS
{
	Console::Out
		<< "Version Information:" << Console::Endl
		<< "  Version: " << APP_VERSION << Console::Endl
		<< "  Source:  " << APP_SOURCE_URL << Console::Endl
		<< "  Changes: - Added '--nolog' option to disable LDBG changelog writes (if supported)" << Console::Endl
		<< "           - Added '-v/--version' handling" << Console::Endl
		<< "           - Added 'driverinfo' command to display driver information" << Console::Endl;

	return EFI_SUCCESS;
}

auto CLI::Run(PCSTR* ArgV, UINT64 ArgC) -> EFI_STATUS
{
	PrintBanner();

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
		return PrintVersionInfo();
	}
	case COMMAND::List:
	{
		PrintKeyList();
		return EFI_SUCCESS;
	}
	case COMMAND::DriverInfo:
	{
		return PrintDriverInfo();
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
		return ReadEntry(Protocol, Options.Key, Options.NoLog);
	}
	case COMMAND::Set:
	{
		return WriteEntry(Protocol, Options.Key, Options.Value, Options.NoLog);
	}
	default:
	{
		return EFI_INVALID_PARAMETER;
	}
	}
}