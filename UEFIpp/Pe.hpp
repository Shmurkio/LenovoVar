#pragma once

#include "UEFIpp.hpp"

#pragma pack(push,4)
constexpr CUINT16 IMAGE_DOS_SIGNATURE			= 0x5A4D;
constexpr CUINT32 IMAGE_NT_SIGNATURE			= 0x00004550;
constexpr CUINT16 IMAGE_NT_OPTIONAL_HDR64_MAGIC	= 0x020B;

constexpr CUINT16 IMAGE_FILE_RELOCS_STRIPPED = 0x0001;

typedef struct _IMAGE_DOS_HEADER
{
	UINT16	e_magic;
	UINT16	e_cblp;
	UINT16	e_cp;
	UINT16	e_crlc;
	UINT16	e_cparhdr;
	UINT16	e_minalloc;
	UINT16	e_maxalloc;
	UINT16	e_ss;
	UINT16	e_sp;
	UINT16	e_csum;
	UINT16	e_ip;
	UINT16	e_cs;
	UINT16	e_lfarlc;
	UINT16	e_ovno;
	UINT16	e_res[4];
	UINT16	e_oemid;
	UINT16	e_oeminfo;
	UINT16	e_res2[10];
	INT32	e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

using CIMAGE_DOS_HEADER		= const IMAGE_DOS_HEADER;
using PCIMAGE_DOS_HEADER	= const IMAGE_DOS_HEADER*;

typedef struct _IMAGE_FILE_HEADER
{
	UINT16	Machine;
	UINT16	NumberOfSections;
	UINT32	TimeDateStamp;
	UINT32	PointerToSymbolTable;
	UINT32	NumberOfSymbols;
	UINT16	SizeOfOptionalHeader;
	UINT16	Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

using CIMAGE_FILE_HEADER	= const IMAGE_FILE_HEADER;
using PCIMAGE_FILE_HEADER	= const IMAGE_FILE_HEADER*;

typedef struct _IMAGE_DATA_DIRECTORY
{
	UINT32	VirtualAddress;
	UINT32	Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

using CIMAGE_DATA_DIRECTORY		= const IMAGE_DATA_DIRECTORY;
using PCIMAGE_DATA_DIRECTORY	= const IMAGE_DATA_DIRECTORY*;

constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_EXPORT			= 0;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_IMPORT			= 1;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_RESOURCE		= 2;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_EXCEPTION		= 3;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_SECURITY		= 4;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_BASERELOC		= 5;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_DEBUG			= 6;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_ARCHITECTURE	= 7;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_GLOBALPTR		= 8;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_TLS				= 9;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG		= 10;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT	= 11;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_IAT				= 12;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	= 13;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR	= 14;
constexpr CUINT32 IMAGE_DIRECTORY_ENTRY_RESERVED		= 15;
constexpr CUINT32 IMAGE_NUMBEROF_DIRECTORY_ENTRIES		= 16;

typedef struct _IMAGE_OPTIONAL_HEADER64
{
	UINT16					Magic;
	UINT8					MajorLinkerVersion;
	UINT8					MinorLinkerVersion;
	UINT32					SizeOfCode;
	UINT32					SizeOfInitializedData;
	UINT32					SizeOfUninitializedData;
	UINT32					AddressOfEntryPoint;
	UINT32					BaseOfCode;
	UINT64					ImageBase;
	UINT32					SectionAlignment;
	UINT32					FileAlignment;
	UINT16					MajorOperatingSystemVersion;
	UINT16					MinorOperatingSystemVersion;
	UINT16					MajorImageVersion;
	UINT16					MinorImageVersion;
	UINT16					MajorSubsystemVersion;
	UINT16					MinorSubsystemVersion;
	UINT32					Win32VersionValue;
	UINT32					SizeOfImage;
	UINT32					SizeOfHeaders;
	UINT32					CheckSum;
	UINT16					Subsystem;
	UINT16					DllCharacteristics;
	UINT64					SizeOfStackReserve;
	UINT64					SizeOfStackCommit;
	UINT64					SizeOfHeapReserve;
	UINT64					SizeOfHeapCommit;
	UINT32					LoaderFlags;
	UINT32					NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY	DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

using CIMAGE_OPTIONAL_HEADER64	= const IMAGE_OPTIONAL_HEADER64;
using PCIMAGE_OPTIONAL_HEADER64	= const IMAGE_OPTIONAL_HEADER64*;

typedef struct _IMAGE_NT_HEADERS64
{
	UINT32						Signature;
	IMAGE_FILE_HEADER			FileHeader;
	IMAGE_OPTIONAL_HEADER64		OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

using CIMAGE_NT_HEADERS64	= const IMAGE_NT_HEADERS64;
using PCIMAGE_NT_HEADERS64	= const IMAGE_NT_HEADERS64*;

constexpr CUINT32 IMAGE_SIZEOF_SHORT_NAME = 8;

typedef struct _IMAGE_SECTION_HEADER
{
	UINT8	Name[IMAGE_SIZEOF_SHORT_NAME];
	union
	{
		UINT32	PhysicalAddress;
		UINT32	VirtualSize;
	} Misc;
	UINT32	VirtualAddress;
	UINT32  SizeOfRawData;
	UINT32  PointerToRawData;
	UINT32  PointerToRelocations;
	UINT32  PointerToLinenumbers;
	UINT16  NumberOfRelocations;
	UINT16  NumberOfLinenumbers;
	UINT32  Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

using CIMAGE_SECTION_HEADER		= const IMAGE_SECTION_HEADER;
using PCIMAGE_SECTION_HEADER	= const IMAGE_SECTION_HEADER*;

typedef struct _IMAGE_EXPORT_DIRECTORY
{
	UINT32	Characteristics;
	UINT32  TimeDateStamp;
	UINT16  MajorVersion;
	UINT16  MinorVersion;
	UINT32  Name;
	UINT32  Base;
	UINT32  NumberOfFunctions;
	UINT32  NumberOfNames;
	UINT32  AddressOfFunctions;
	UINT32  AddressOfNames;
	UINT32  AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

using CIMAGE_EXPORT_DIRECTORY	= const IMAGE_EXPORT_DIRECTORY;
using PCIMAGE_EXPORT_DIRECTORY	= const IMAGE_EXPORT_DIRECTORY*;

typedef struct _IMAGE_IMPORT_DESCRIPTOR
{
	union
	{
		UINT32	Characteristics;
		UINT32	OriginalFirstThunk;
	};
	UINT32	TimeDateStamp;
	UINT32  ForwarderChain;
	UINT32  Name;
	UINT32  FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR, *PIMAGE_IMPORT_DESCRIPTOR;

using CIMAGE_IMPORT_DESCRIPTOR	= const IMAGE_IMPORT_DESCRIPTOR;
using PCIMAGE_IMPORT_DESCRIPTOR	= const IMAGE_IMPORT_DESCRIPTOR*;

typedef struct _IMAGE_THUNK_DATA64
{
	union
	{
		UINT64	ForwarderString;
		UINT64	Function;
		UINT64	Ordinal;
		UINT64	AddressOfData;
	} u1;
} IMAGE_THUNK_DATA64, *PIMAGE_THUNK_DATA64;

using CIMAGE_THUNK_DATA64	= const IMAGE_THUNK_DATA64;
using PCIMAGE_THUNK_DATA64	= const IMAGE_THUNK_DATA64*;

typedef struct _IMAGE_IMPORT_BY_NAME
{
	UINT16	Hint;
	CHAR	Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

using CIMAGE_IMPORT_BY_NAME		= const IMAGE_IMPORT_BY_NAME;
using PCIMAGE_IMPORT_BY_NAME	= const IMAGE_IMPORT_BY_NAME*;

constexpr CUINT64 IMAGE_ORDINAL_FLAG64 = 0x8000000000000000ULL;

typedef struct _IMAGE_BASE_RELOCATION
{
	UINT32  VirtualAddress;
	UINT32  SizeOfBlock;
} IMAGE_BASE_RELOCATION, *PIMAGE_BASE_RELOCATION;

using CIMAGE_BASE_RELOCATION	= const IMAGE_BASE_RELOCATION;
using PCIMAGE_BASE_RELOCATION	= const IMAGE_BASE_RELOCATION*;

constexpr CUINT16 IMAGE_REL_BASED_ABSOLUTE	= 0;
constexpr CUINT16 IMAGE_REL_BASED_HIGH		= 1;
constexpr CUINT16 IMAGE_REL_BASED_LOW		= 2;
constexpr CUINT16 IMAGE_REL_BASED_HIGHLOW	= 3;
constexpr CUINT16 IMAGE_REL_BASED_HIGHADJ	= 4;
constexpr CUINT16 IMAGE_REL_BASED_SECTION	= 6;
constexpr CUINT16 IMAGE_REL_BASED_DIR64		= 10;

constexpr
UINT16
IMAGE_REL_BASED_TYPE(
	IN	UINT16	word
) noexcept
{
	return static_cast<UINT16>(word >> 12);
}

constexpr
UINT16
IMAGE_REL_BASED_OFFSET(
	IN	UINT16	word
) noexcept
{
	return static_cast<UINT16>(word & 0x0FFF);
}

typedef struct _IMAGE_DEBUG_DIRECTORY
{
	UINT32	Characteristics;
	UINT32  TimeDateStamp;
	UINT16  MajorVersion;
	UINT16  MinorVersion;
	UINT32  Type;
	UINT32  SizeOfData;
	UINT32  AddressOfRawData;
	UINT32  PointerToRawData;
} IMAGE_DEBUG_DIRECTORY, *PIMAGE_DEBUG_DIRECTORY;

using CIMAGE_DEBUG_DIRECTORY	= const IMAGE_DEBUG_DIRECTORY;
using PCIMAGE_DEBUG_DIRECTORY	= const IMAGE_DEBUG_DIRECTORY*;

typedef struct _WIN_CERTIFICATE
{
	UINT32  dwLength;
	UINT16  wRevision;
	UINT16  wCertificateType;
	UINT8   bCertificate[1];
} WIN_CERTIFICATE, *PWIN_CERTIFICATE;

using CWIN_CERTIFICATE	= const WIN_CERTIFICATE;
using PCWIN_CERTIFICATE	= const WIN_CERTIFICATE*;

typedef struct _IMAGE_TLS_DIRECTORY64
{
	UINT64  StartAddressOfRawData;
	UINT64  EndAddressOfRawData;
	UINT64  AddressOfIndex;
	UINT64  AddressOfCallBacks;
	UINT32  SizeOfZeroFill;
	UINT32  Characteristics;
} IMAGE_TLS_DIRECTORY64, *PIMAGE_TLS_DIRECTORY64;

using CIMAGE_TLS_DIRECTORY64	= const IMAGE_TLS_DIRECTORY64;
using PCIMAGE_TLS_DIRECTORY64	= const IMAGE_TLS_DIRECTORY64*;

typedef struct _IMAGE_LOAD_CONFIG_CODE_INTEGRITY
{
	UINT16  Flags;
	UINT16  Catalog;
	UINT32  CatalogOffset;
	UINT32	Reserved;
} IMAGE_LOAD_CONFIG_CODE_INTEGRITY, *PIMAGE_LOAD_CONFIG_CODE_INTEGRITY;

using CIMAGE_LOAD_CONFIG_CODE_INTEGRITY		= const IMAGE_LOAD_CONFIG_CODE_INTEGRITY;
using PCIMAGE_LOAD_CONFIG_CODE_INTEGRITY	= const IMAGE_LOAD_CONFIG_CODE_INTEGRITY*;

typedef struct _IMAGE_LOAD_CONFIG_DIRECTORY64
{
	UINT32								Size;
	UINT32								TimeDateStamp;
	UINT16								MajorVersion;
	UINT16								MinorVersion;
	UINT32								GlobalFlagsClear;
	UINT32								GlobalFlagsSet;
	UINT32								CriticalSectionDefaultTimeout;
	UINT64								DeCommitFreeBlockThreshold;
	UINT64								DeCommitTotalFreeThreshold;
	UINT64								LockPrefixTable;
	UINT64								MaximumAllocationSize;
	UINT64								VirtualMemoryThreshold;
	UINT64								ProcessAffinityMask;
	UINT32								ProcessHeapFlags;
	UINT16								CSDVersion;
	UINT16								DependentLoadFlags;
	UINT64								EditList;
	UINT64								SecurityCookie;
	UINT64								SEHandlerTable;
	UINT64								SEHandlerCount;
	UINT64								GuardCFCheckFunctionPointer;
	UINT64								GuardCFDispatchFunctionPointer;
	UINT64								GuardCFFunctionTable;
	UINT64								GuardCFFunctionCount;
	UINT32								GuardFlags;
	IMAGE_LOAD_CONFIG_CODE_INTEGRITY	CodeIntegrity;
	UINT64								GuardAddressTakenIatEntryTable;
	UINT64								GuardAddressTakenIatEntryCount;
	UINT64								GuardLongJumpTargetTable;
	UINT64								GuardLongJumpTargetCount;
	UINT64								DynamicValueRelocTable;
	UINT64								CHPEMetadataPointer;
	UINT64								GuardRFFailureRoutine;
	UINT64								GuardRFFailureRoutineFunctionPointer;
	UINT32								DynamicValueRelocTableOffset;
	UINT16								DynamicValueRelocTableSection;
	UINT16								Reserved2;
	UINT64								GuardRFVerifyStackPointerFunctionPointer;
	UINT32								HotPatchTableOffset;
	UINT32								Reserved3;
	UINT64								EnclaveConfigurationPointer;
	UINT64								VolatileMetadataPointer;
	UINT64								GuardEHContinuationTable;
	UINT64								GuardEHContinuationCount;
	UINT64								GuardXFGCheckFunctionPointer;
	UINT64								GuardXFGDispatchFunctionPointer;
	UINT64								GuardXFGTableDispatchFunctionPointer;
	UINT64								CastGuardOsDeterminedFailureMode;
	UINT64								GuardMemcpyFunctionPointer;
	UINT64								UmaFunctionPointers;
} IMAGE_LOAD_CONFIG_DIRECTORY64, *PIMAGE_LOAD_CONFIG_DIRECTORY64;

using CIMAGE_LOAD_CONFIG_DIRECTORY64	= const IMAGE_LOAD_CONFIG_DIRECTORY64;
using PCIMAGE_LOAD_CONFIG_DIRECTORY64	= const IMAGE_LOAD_CONFIG_DIRECTORY64*;

typedef struct _IMAGE_BOUND_IMPORT_DESCRIPTOR
{
	UINT32	TimeDateStamp;
	UINT16  OffsetModuleName;
	UINT16  NumberOfModuleForwarderRefs;
} IMAGE_BOUND_IMPORT_DESCRIPTOR, *PIMAGE_BOUND_IMPORT_DESCRIPTOR;

using CIMAGE_BOUND_IMPORT_DESCRIPTOR	= const IMAGE_BOUND_IMPORT_DESCRIPTOR;
using PCIMAGE_BOUND_IMPORT_DESCRIPTOR	= const IMAGE_BOUND_IMPORT_DESCRIPTOR*;

typedef struct _IMAGE_BOUND_FORWARDER_REF
{
	UINT32  TimeDateStamp;
	UINT16  OffsetModuleName;
	UINT16  Reserved;
} IMAGE_BOUND_FORWARDER_REF, *PIMAGE_BOUND_FORWARDER_REF;

using CIMAGE_BOUND_FORWARDER_REF	= const IMAGE_BOUND_FORWARDER_REF;
using PCIMAGE_BOUND_FORWARDER_REF	= const IMAGE_BOUND_FORWARDER_REF*;

typedef struct _IMAGE_DELAYLOAD_DESCRIPTOR
{
	UINT32  Attributes;
	UINT32  DllNameRVA;
	UINT32  ModuleHandleRVA;
	UINT32  ImportAddressTableRVA;
	UINT32  ImportNameTableRVA;
	UINT32  BoundImportAddressTableRVA;
	UINT32  UnloadInformationTableRVA;
	UINT32  TimeDateStamp;
} IMAGE_DELAYLOAD_DESCRIPTOR, *PIMAGE_DELAYLOAD_DESCRIPTOR;

using CIMAGE_DELAYLOAD_DESCRIPTOR	= const IMAGE_DELAYLOAD_DESCRIPTOR;
using PCIMAGE_DELAYLOAD_DESCRIPTOR	= const IMAGE_DELAYLOAD_DESCRIPTOR*;

typedef struct _IMAGE_COR20_HEADER
{
	UINT32					cb;
	UINT16					MajorRuntimeVersion;
	UINT16					MinorRuntimeVersion;
	IMAGE_DATA_DIRECTORY	MetaData;
	UINT32					Flags;
	UINT32					EntryPointToken;
	IMAGE_DATA_DIRECTORY	Resources;
	IMAGE_DATA_DIRECTORY	StrongNameSignature;
	IMAGE_DATA_DIRECTORY	CodeManagerTable;
	IMAGE_DATA_DIRECTORY	VTableFixups;
	IMAGE_DATA_DIRECTORY	ExportAddressTableJumps;
	IMAGE_DATA_DIRECTORY	ManagedNativeHeader;
} IMAGE_COR20_HEADER, *PIMAGE_COR20_HEADER;

using CIMAGE_COR20_HEADER	= const IMAGE_COR20_HEADER;
using PCIMAGE_COR20_HEADER	= const IMAGE_COR20_HEADER*;


typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY
{
	UINT32  BeginAddress;
	UINT32  EndAddress;
	UINT32  UnwindInfoAddress;
} IMAGE_RUNTIME_FUNCTION_ENTRY, *PIMAGE_RUNTIME_FUNCTION_ENTRY;

using CIMAGE_RUNTIME_FUNCTION_ENTRY		= const IMAGE_RUNTIME_FUNCTION_ENTRY;
using PCIMAGE_RUNTIME_FUNCTION_ENTRY	= const IMAGE_RUNTIME_FUNCTION_ENTRY*;
#pragma pack(pop)

constexpr CUINT16 IMAGE_FILE_MACHINE_UNKNOWN		= 0x0000;
constexpr CUINT16 IMAGE_FILE_MACHINE_TARGET_HOST	= 0x0001;
constexpr CUINT16 IMAGE_FILE_MACHINE_ALPHA_OLD		= 0x0183;
constexpr CUINT16 IMAGE_FILE_MACHINE_ALPHA			= 0x0184;
constexpr CUINT16 IMAGE_FILE_MACHINE_ALPHA64		= 0x0284;
constexpr CUINT16 IMAGE_FILE_MACHINE_AXP64			= IMAGE_FILE_MACHINE_ALPHA64;
constexpr CUINT16 IMAGE_FILE_MACHINE_AM33			= 0x01D3;
constexpr CUINT16 IMAGE_FILE_MACHINE_AMD64			= 0x8664;
constexpr CUINT16 IMAGE_FILE_MACHINE_ARM			= 0x01C0;
constexpr CUINT16 IMAGE_FILE_MACHINE_THUMB			= 0x01C2;
constexpr CUINT16 IMAGE_FILE_MACHINE_ARMNT			= 0x01C4;
constexpr CUINT16 IMAGE_FILE_MACHINE_ARMV7			= IMAGE_FILE_MACHINE_ARMNT;
constexpr CUINT16 IMAGE_FILE_MACHINE_ARM64			= 0xAA64;
constexpr CUINT16 IMAGE_FILE_MACHINE_ARM64EC		= 0xA641;
constexpr CUINT16 IMAGE_FILE_MACHINE_ARM64X			= 0xA64E;
constexpr CUINT16 IMAGE_FILE_MACHINE_CEE			= 0xC0EE;
constexpr CUINT16 IMAGE_FILE_MACHINE_CEF			= 0x0CEF;
constexpr CUINT16 IMAGE_FILE_MACHINE_CHPE_X86		= 0x3A64;
constexpr CUINT16 IMAGE_FILE_MACHINE_HYBRID_X86		= IMAGE_FILE_MACHINE_CHPE_X86;
constexpr CUINT16 IMAGE_FILE_MACHINE_EBC			= 0x0EBC;
constexpr CUINT16 IMAGE_FILE_MACHINE_I386			= 0x014C;
constexpr CUINT16 IMAGE_FILE_MACHINE_I860			= 0x014D;
constexpr CUINT16 IMAGE_FILE_MACHINE_IA64			= 0x0200;
constexpr CUINT16 IMAGE_FILE_MACHINE_LOONGARCH32	= 0x6232;
constexpr CUINT16 IMAGE_FILE_MACHINE_LOONGARCH64	= 0x6264;
constexpr CUINT16 IMAGE_FILE_MACHINE_M32R			= 0x9041;
constexpr CUINT16 IMAGE_FILE_MACHINE_M68K			= 0x0268;
constexpr CUINT16 IMAGE_FILE_MACHINE_MIPS16			= 0x0266;
constexpr CUINT16 IMAGE_FILE_MACHINE_MIPSFPU		= 0x0366;
constexpr CUINT16 IMAGE_FILE_MACHINE_MIPSFPU16		= 0x0466;
constexpr CUINT16 IMAGE_FILE_MACHINE_MPPC_601		= 0x0601;
constexpr CUINT16 IMAGE_FILE_MACHINE_OMNI			= 0xACE1;
constexpr CUINT16 IMAGE_FILE_MACHINE_PARISC			= 0x0290;
constexpr CUINT16 IMAGE_FILE_MACHINE_POWERPC		= 0x01F0;
constexpr CUINT16 IMAGE_FILE_MACHINE_POWERPCFP		= 0x01F1;
constexpr CUINT16 IMAGE_FILE_MACHINE_POWERPCBE		= 0x01F2;
constexpr CUINT16 IMAGE_FILE_MACHINE_R3000			= 0x0162;
constexpr CUINT16 IMAGE_FILE_MACHINE_R3000_BE		= 0x0160;
constexpr CUINT16 IMAGE_FILE_MACHINE_R4000			= 0x0166;
constexpr CUINT16 IMAGE_FILE_MACHINE_R10000			= 0x0168;
constexpr CUINT16 IMAGE_FILE_MACHINE_WCEMIPSV2		= 0x0169;
constexpr CUINT16 IMAGE_FILE_MACHINE_RISCV32		= 0x5032;
constexpr CUINT16 IMAGE_FILE_MACHINE_RISCV64		= 0x5064;
constexpr CUINT16 IMAGE_FILE_MACHINE_RISCV128		= 0x5128;
constexpr CUINT16 IMAGE_FILE_MACHINE_SH3			= 0x01A2;
constexpr CUINT16 IMAGE_FILE_MACHINE_SH3DSP			= 0x01A3;
constexpr CUINT16 IMAGE_FILE_MACHINE_SH3E			= 0x01A4;
constexpr CUINT16 IMAGE_FILE_MACHINE_SH4			= 0x01A6;
constexpr CUINT16 IMAGE_FILE_MACHINE_SH5			= 0x01A8;
constexpr CUINT16 IMAGE_FILE_MACHINE_TAHOE			= 0x07CC;
constexpr CUINT16 IMAGE_FILE_MACHINE_TRICORE		= 0x0520;

constexpr CUINT16 IMAGE_SUBSYSTEM_UNKNOWN					= 0;
constexpr CUINT16 IMAGE_SUBSYSTEM_NATIVE					= 1;
constexpr CUINT16 IMAGE_SUBSYSTEM_WINDOWS_GUI				= 2;
constexpr CUINT16 IMAGE_SUBSYSTEM_WINDOWS_CUI				= 3;
constexpr CUINT16 IMAGE_SUBSYSTEM_OS2_CUI					= 5;
constexpr CUINT16 IMAGE_SUBSYSTEM_POSIX_CUI					= 7;
constexpr CUINT16 IMAGE_SUBSYSTEM_NATIVE_WINDOWS			= 8;
constexpr CUINT16 IMAGE_SUBSYSTEM_WINDOWS_CE_GUI			= 9;
constexpr CUINT16 IMAGE_SUBSYSTEM_EFI_APPLICATION			= 10;
constexpr CUINT16 IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER	= 11;
constexpr CUINT16 IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER		= 12;
constexpr CUINT16 IMAGE_SUBSYSTEM_EFI_ROM					= 13;
constexpr CUINT16 IMAGE_SUBSYSTEM_XBOX						= 14;
constexpr CUINT16 IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION	= 16;

constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA			= 0x0020;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE				= 0x0040;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY			= 0x0080;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_NX_COMPAT				= 0x0100;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_NO_ISOLATION				= 0x0200;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_NO_SEH					= 0x0400;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_NO_BIND					= 0x0800;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_APPCONTAINER				= 0x1000;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_WDM_DRIVER				= 0x2000;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_GUARD_CF					= 0x4000;
constexpr CUINT16 IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE	= 0x8000;

constexpr
PIMAGE_SECTION_HEADER
IMAGE_FIRST_SECTION(
	IN	PCIMAGE_NT_HEADERS64	NtHeaders
)
{
	if (!NtHeaders)
	{
		return nullptr;
	}

	return reinterpret_cast<PIMAGE_SECTION_HEADER>(const_cast<PUINT8>(reinterpret_cast<PCUINT8>(NtHeaders)) + offsetof(IMAGE_NT_HEADERS64, OptionalHeader) + NtHeaders->FileHeader.SizeOfOptionalHeader);
}

constexpr CUINT32 DLL_PROCESS_DETACH	= 0;
constexpr CUINT32 DLL_PROCESS_ATTACH	= 1;
constexpr CUINT32 DLL_THREAD_ATTACH		= 2;
constexpr CUINT32 DLL_THREAD_DETACH		= 3;

using TlsCallbackFn = VOID(__cdecl)(
	IN	PVOID	DllHandle,
	IN	UINT32	Reason,
	IN	PVOID	Reserved
);

using AllocFn = PVOID(__cdecl)(
	IN	UINT64	Size
);

template<typename T>
static
inline
auto
ToPtr(
	IN	T	Value
) -> std::conditional_t<std::is_const_v<std::remove_pointer_t<std::remove_reference_t<T>>>, PCUINT8, PUINT8>
{
	using RetPtr = std::conditional_t<std::is_const_v<std::remove_pointer_t<std::remove_reference_t<T>>>, PCUINT8, PUINT8>;

	if constexpr (std::is_pointer_v<std::remove_reference_t<T>>)
	{
		return reinterpret_cast<RetPtr>(Value);
	}
	else
	{
		if (!Value)
		{
			return nullptr;
		}

		return reinterpret_cast<RetPtr>(static_cast<UINT64>(Value));
	}
}

static
inline
auto
RvaToPtr(
	IN	PCVOID	ImageBase,
	IN	CUINT32	Rva
) -> PCUINT8
{
	auto Base = ToPtr(ImageBase);
	return Base ? (Base + Rva) : nullptr;
}

static
inline
auto
StrLen(
	IN	PCSTR	Str
) -> UINT64
{
	UINT64 Len = 0;
	while (Str && Str[Len]) ++Len;
	return Len;
}

static
inline
auto
StrCmp(
	IN	PCSTR	Str1,
	IN	PCSTR	Str2
) -> INT64
{
	if (!Str1 || !Str2)
	{
		return (Str1 == Str2) ? 0 : (Str1 ? 1 : -1);
	}

	while (*Str1 && (*Str1 == *Str2))
	{
		++Str1;
		++Str2;
	}

	return static_cast<INT64>(static_cast<CINT64>(static_cast<UINT8>(*Str1)) - static_cast<CINT64>(static_cast<UINT8>(*Str2)));
}

using FindModuleFn = EFI_STATUS(__cdecl)(
	IN	PCSTR	DllName,
	OUT	PVOID*	ModuleBase
);

static
inline
auto
SplitForwarder(
	IN	PSTR	Forwarder,
	OUT	PSTR&	Dll,
	OUT	PSTR&	Sym
) -> EFI_STATUS
{
	Dll = nullptr;
	Sym = nullptr;

	if (!Forwarder)
	{
		return EFI_INVALID_PARAMETER;
	}

	auto Dot = Forwarder;
	while (*Dot && *Dot != '.') ++Dot;

	if (*Dot != '.')
	{
		return EFI_UNSUPPORTED;
	}

	*Dot = '\0';
	Dll = Forwarder;
	Sym = Dot + 1;

	if (!*Dll || !*Sym)
	{
		return EFI_UNSUPPORTED;
	}

	return EFI_SUCCESS;
}

static
inline
auto
IsCleanThunkValue(
	IN	CUINT32	ImageSize,
	IN	CUINT64	Value
) -> BOOLEAN
{
	if ((Value & IMAGE_ORDINAL_FLAG64))
	{
		return true;
	}
	
	return (Value < static_cast<UINT64>(ImageSize));
}

namespace Pe
{
	template<typename T>
	auto
	GetHeader(
		IN	T						ImageBase,
		OUT	PIMAGE_DOS_HEADER&		DosHeader,
		OUT	PIMAGE_NT_HEADERS64&	NtHeaders
	) -> EFI_STATUS
	{
		DosHeader = nullptr;
		NtHeaders = nullptr;

		auto Base = ToPtr(ImageBase);

		if (!Base)
		{
			return EFI_INVALID_PARAMETER;
		}

		auto _DosHeader = reinterpret_cast<PCIMAGE_DOS_HEADER>(Base);

		if (_DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		{
			return EFI_UNSUPPORTED;
		}

		if (_DosHeader->e_lfanew <= 0)
		{
			return EFI_COMPROMISED_DATA;
		}

		auto _NtHeaders = reinterpret_cast<PCIMAGE_NT_HEADERS64>(Base + _DosHeader->e_lfanew);

		if (_NtHeaders->Signature != IMAGE_NT_SIGNATURE)
		{
			return EFI_UNSUPPORTED;
		}

		if (_NtHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
			return EFI_UNSUPPORTED;
		}

		DosHeader = const_cast<PIMAGE_DOS_HEADER>(_DosHeader);
		NtHeaders = const_cast<PIMAGE_NT_HEADERS64>(_NtHeaders);

		return EFI_SUCCESS;
	}

	template<typename T>
	auto
	GetExport(
		IN	T				ModuleBase,
		IN	PCSTR			Name,
		IN	CUINT16			Ordinal,
		OUT	PVOID&			Address,
		IN	FindModuleFn*	FindModule = nullptr,
		IN	CUINT32			MaxForwardDepth = 8
	) -> EFI_STATUS
	{
		Address = nullptr;

		auto Base = ToPtr(ModuleBase);

		if (!Base)
		{
			return EFI_INVALID_PARAMETER;
		}

		PIMAGE_DOS_HEADER DosHeader = nullptr;
		PIMAGE_NT_HEADERS64 NtHeaders = nullptr;

		auto Status = Pe::GetHeader(Base, DosHeader, NtHeaders);

		if (EfiError(Status))
		{
			return Status;
		}

		auto& Directory = NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

		if (!Directory.VirtualAddress || !Directory.Size)
		{
			return EFI_NOT_FOUND;
		}

		auto Export = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(Base + Directory.VirtualAddress);

		if (!Export->AddressOfFunctions || !Export->NumberOfFunctions)
		{
			return EFI_NOT_FOUND;
		}

		auto Functions = reinterpret_cast<PUINT32>(Base + Export->AddressOfFunctions);
		auto Names = reinterpret_cast<PUINT32>(Base + Export->AddressOfNames);
		auto Ordinals = reinterpret_cast<PUINT16>(Base + Export->AddressOfNameOrdinals);

		UINT32 FuncRva = 0;

		if (Name)
		{
			if (!Export->AddressOfNames || !Export->AddressOfNameOrdinals || !Export->NumberOfNames)
			{
				return EFI_NOT_FOUND;
			}

			INT64 Low = 0;
			INT64 High = static_cast<INT64>(Export->NumberOfNames - 1);

			while (Low <= High)
			{
				auto Mid = (Low + High) / 2;

				auto MidName = reinterpret_cast<PCSTR>(Base + Names[Mid]);
				auto Cmp = StrCmp(Name, MidName);

				if (!Cmp)
				{
					auto OrdIndex = Ordinals[Mid];

					if (OrdIndex >= Export->NumberOfFunctions)
					{
						return EFI_COMPROMISED_DATA;
					}

					FuncRva = Functions[OrdIndex];
					break;
				}

				if (Cmp < 0)
				{
					High = Mid - 1;
				}
				else
				{
					Low = Mid + 1;
				}
			}

			if (!FuncRva)
			{
				return EFI_NOT_FOUND;
			}
		}
		else
		{
			if (Ordinal < static_cast<UINT16>(Export->Base))
			{
				return EFI_NOT_FOUND;
			}

			auto OrdIndex = static_cast<UINT32>(Ordinal - static_cast<UINT16>(Export->Base));

			if (OrdIndex >= Export->NumberOfFunctions)
			{
				return EFI_NOT_FOUND;
			}

			FuncRva = Functions[OrdIndex];

			if (!FuncRva)
			{
				return EFI_NOT_FOUND;
			}
		}

		if (FuncRva >= Directory.VirtualAddress && FuncRva < (Directory.VirtualAddress + Directory.Size))
		{
			if (!FindModule || !MaxForwardDepth)
			{
				return EFI_UNSUPPORTED;
			}

			auto FwdStr = reinterpret_cast<PCSTR>(Base + FuncRva);

			CHAR Tmp[256]{};
			auto Length = StrLen(FwdStr);

			if (!Length || Length >= sizeof(Tmp))
			{
				return EFI_BUFFER_TOO_SMALL;
			}

			for (UINT64 i = 0; i <= Length; ++i)
			{
				Tmp[i] = FwdStr[i];
			}

			PSTR Dll = nullptr;
			PSTR Sym = nullptr;

			Status = SplitForwarder(Tmp, Dll, Sym);

			if (EfiError(Status))
			{
				return Status;
			}

			PVOID FwdMod = nullptr;
			Status = FindModule(Dll, &FwdMod);

			if (EfiError(Status) || !FwdMod)
			{
				return EFI_NOT_FOUND;
			}

			if (Sym[0] == '#')
			{
				UINT64 O = 0;

				for (auto p = Sym + 1; *p; ++p)
				{
					if (*p < '0' || *p > '9')
					{
						return EFI_UNSUPPORTED;
					}

					O = (O * 10) + static_cast<UINT64>(*p - '0');

					if (O > 0xFFFF)
					{
						return EFI_UNSUPPORTED;
					}
				}

				return Pe::GetExport(FwdMod, nullptr, (UINT16)O, Address, FindModule, MaxForwardDepth - 1);
			}
			else
			{
				return Pe::GetExport(FwdMod, Sym, 0, Address, FindModule, MaxForwardDepth - 1);
			}
		}

		Address = const_cast<PVOID>(reinterpret_cast<PCVOID>(Base + FuncRva));
		return EFI_SUCCESS;
	}

	template<typename T>
	auto
	ApplyRelocations(
		IN	T	ImageBase
	) -> EFI_STATUS
	{
		auto Base = ToPtr(ImageBase);

		if (!Base)
		{
			return EFI_INVALID_PARAMETER;
		}

		PIMAGE_DOS_HEADER DosHeader = nullptr;
		PIMAGE_NT_HEADERS64 NtHeaders = nullptr;

		auto Status = Pe::GetHeader(Base, DosHeader, NtHeaders);

		if (EfiError(Status))
		{
			return Status;
		}

		auto PreferredBase = NtHeaders->OptionalHeader.ImageBase;
		auto ActualBase = reinterpret_cast<UINT64>(Base);

		if (PreferredBase == ActualBase)
		{
			return EFI_SUCCESS;
		}

		auto Delta = static_cast<INT64>(ActualBase) - static_cast<INT64>(PreferredBase);

		auto& Directory = NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

		if (!Directory.VirtualAddress || !Directory.Size)
		{
			if (NtHeaders->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
			{
				return EFI_UNSUPPORTED;
			}

			NtHeaders->OptionalHeader.ImageBase = ActualBase;
			return EFI_SUCCESS;
		}

		auto Reloc = reinterpret_cast<PIMAGE_BASE_RELOCATION>(Base + Directory.VirtualAddress);
		auto End = reinterpret_cast<PUINT8>(Reloc) + Directory.Size;

		while (reinterpret_cast<PUINT8>(Reloc) < End && Reloc->SizeOfBlock)
		{
			auto BlockVa = Reloc->VirtualAddress;
			auto BlockSize = Reloc->SizeOfBlock;

			if (BlockSize < sizeof(IMAGE_BASE_RELOCATION))
			{
				return EFI_COMPROMISED_DATA;
			}

			auto EntryCount = static_cast<UINT32>((BlockSize - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(UINT16));
			auto Entries = reinterpret_cast<PUINT16>(reinterpret_cast<PUINT8>(Reloc) + sizeof(IMAGE_BASE_RELOCATION));

			for (UINT32 i = 0; i < EntryCount; ++i)
			{
				auto TypeOffset = Entries[i];

				auto Type = static_cast<UINT16>((TypeOffset >> 12) & 0xF);
				auto Offset = static_cast<UINT16>(TypeOffset & 0x0FFF);

				auto Patch = Base + BlockVa + Offset;

				switch (Type)
				{
				case IMAGE_REL_BASED_ABSOLUTE:
				{
					break;
				}

				case IMAGE_REL_BASED_DIR64:
				{
					auto p = reinterpret_cast<PUINT64>(Patch);
					*p = static_cast<UINT64>(static_cast<INT64>(*p) + Delta);
					break;
				}

				case IMAGE_REL_BASED_HIGHLOW:
				{
					auto p = reinterpret_cast<PUINT32>(Patch);
					*p = static_cast<UINT32>(static_cast<INT64>(*p) + Delta);
					break;
				}

				case IMAGE_REL_BASED_HIGH:
				{
					auto p = reinterpret_cast<PUINT16>(Patch);
					auto Add = static_cast<UINT16>((static_cast<UINT64>(Delta) >> 16) & 0xFFFF);
					*p = static_cast<UINT16>(*p + Add);
					break;
				}

				case IMAGE_REL_BASED_LOW:
				{
					auto p = reinterpret_cast<PUINT16>(Patch);
					auto Add = static_cast<UINT16>(static_cast<UINT64>(Delta) & 0xFFFF);
					*p = static_cast<UINT16>(*p + Add);
					break;
				}

				case IMAGE_REL_BASED_HIGHADJ:
				{
					if ((i + 1) >= EntryCount)
					{
						return EFI_COMPROMISED_DATA;
					}

					auto p = reinterpret_cast<PUINT16>(Patch);

					auto High16 = static_cast<INT32>(static_cast<INT16>(*p));
					auto Low16 = static_cast<INT32>(static_cast<INT16>(Entries[i + 1]));

					auto Value = (High16 << 16) + Low16;
					Value = static_cast<INT32>(Value + Delta);

					auto NewHigh = static_cast<INT16>((Value + 0x8000) >> 16);
					*p = static_cast<UINT16>(NewHigh);

					++i;
					break;
				}

				case IMAGE_REL_BASED_SECTION:
				{
					return EFI_UNSUPPORTED;
				}

				default:
				{
					return EFI_UNSUPPORTED;
				}
				}
			}

			Reloc = reinterpret_cast<PIMAGE_BASE_RELOCATION>(reinterpret_cast<PUINT8>(Reloc) + BlockSize);
		}

		NtHeaders->OptionalHeader.ImageBase = ActualBase;
		return EFI_SUCCESS;
	}

	template<typename T, typename U = T>
	auto
	ResolveImports(
		IN	T				ImageBase,
		IN	FindModuleFn*	FindModule,
		IN	CUINT32			MaxForwardDepth = 8,
		IN	U				ReferenceImage = nullptr
	) -> EFI_STATUS
	{
		auto Base = ToPtr(ImageBase);

		if (!Base || !FindModule)
		{
			return EFI_INVALID_PARAMETER;
		}

		PIMAGE_DOS_HEADER DosHeader = nullptr;
		PIMAGE_NT_HEADERS64 NtHeaders = nullptr;

		auto Status = Pe::GetHeader(Base, DosHeader, NtHeaders);

		if (EfiError(Status))
		{
			return Status;
		}

		auto& Directory = NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

		if (!Directory.VirtualAddress || !Directory.Size)
		{
			return EFI_SUCCESS;
		}

		PCUINT8 RefBase = ToPtr(ReferenceImage);
		PCIMAGE_IMPORT_DESCRIPTOR RefDesc = nullptr;

		if (RefBase)
		{
			PIMAGE_DOS_HEADER RefDos = nullptr;
			PIMAGE_NT_HEADERS64 RefNt = nullptr;

			Status = Pe::GetHeader(RefBase, RefDos, RefNt);

			if (EfiError(Status))
			{
				return Status;
			}

			auto& RefDir = RefNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

			if (!RefDir.VirtualAddress || !RefDir.Size)
			{
				return EFI_UNSUPPORTED;
			}

			RefDesc = reinterpret_cast<PCIMAGE_IMPORT_DESCRIPTOR>(RefBase + RefDir.VirtualAddress);
		}

		auto ImportDesc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(Base + Directory.VirtualAddress);

		for (UINT32 DescIndex = 0; ImportDesc[DescIndex].Name; ++DescIndex)
		{
			auto DllName = reinterpret_cast<PCSTR>(Base + ImportDesc[DescIndex].Name);

			PVOID ModuleBase = nullptr;
			Status = FindModule(DllName, &ModuleBase);

			if (EfiError(Status) || !ModuleBase)
			{
				return EFI_NOT_FOUND;
			}

			auto IatThunk = reinterpret_cast<PIMAGE_THUNK_DATA64>(Base + ImportDesc[DescIndex].FirstThunk);
			PCIMAGE_THUNK_DATA64 IntThunk = nullptr;

			if (ImportDesc[DescIndex].OriginalFirstThunk)
			{
				IntThunk = reinterpret_cast<PIMAGE_THUNK_DATA64>(Base + ImportDesc[DescIndex].OriginalFirstThunk);
			}
			else
			{
				auto FirstVal = static_cast<UINT64>(IatThunk[0].u1.AddressOfData);

				if (IsCleanThunkValue(NtHeaders->OptionalHeader.SizeOfImage, FirstVal))
				{
					IntThunk = IatThunk;
				}
				else
				{
					if (!RefDesc || !RefBase)
					{
						return EFI_UNSUPPORTED;
					}

					auto RefOft = RefDesc[DescIndex].OriginalFirstThunk;

					if (!RefOft)
					{
						RefOft = RefDesc[DescIndex].FirstThunk;
					}

					if (!RefOft)
					{
						return EFI_UNSUPPORTED;
					}

					IntThunk = reinterpret_cast<PCIMAGE_THUNK_DATA64>(RefBase + RefOft);
				}
			}

			for (UINT32 ThunkIndex = 0; IntThunk[ThunkIndex].u1.AddressOfData; ++ThunkIndex)
			{
				PVOID Proc = nullptr;

				auto Th = static_cast<UINT64>(IntThunk[ThunkIndex].u1.AddressOfData);

				if ((Th & IMAGE_ORDINAL_FLAG64) != 0)
				{
					auto Ord = static_cast<UINT16>(Th & 0xFFFF);
					Status = Pe::GetExport(ModuleBase, nullptr, Ord, Proc, FindModule, MaxForwardDepth);
				}
				else
				{
					auto ImportByName = reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(Base + static_cast<UINT32>(Th));
					auto FuncName = reinterpret_cast<PCSTR>(ImportByName->Name);

					Status = Pe::GetExport(ModuleBase, FuncName, 0, Proc, FindModule, MaxForwardDepth);
				}

				if (EfiError(Status) || !Proc)
				{
					return EFI_NOT_FOUND;
				}

				IatThunk[ThunkIndex].u1.Function = static_cast<UINT64>(reinterpret_cast<UINT64>(Proc));
			}

			{
				UINT32 ThunkIndex = 0;

				while (IntThunk[ThunkIndex].u1.AddressOfData)
				{
					++ThunkIndex;
				}

				IatThunk[ThunkIndex].u1.AddressOfData = 0;
			}
		}

		return EFI_SUCCESS;
	}

	template<typename T>
	auto
	ProcessTls(
		IN	T			ImageBase,
		IN	CUINT32		Reason = DLL_PROCESS_ATTACH,
		IN	PVOID		Reserved = nullptr,
		IN	AllocFn*	Alloc = nullptr,
		OUT	PVOID*		OutTlsData = nullptr
	) -> EFI_STATUS
	{
		if (OutTlsData)
		{
			*OutTlsData = nullptr;
		}

		auto Base = ToPtr(ImageBase);

		if (!Base)
		{
			return EFI_INVALID_PARAMETER;
		}

		PIMAGE_DOS_HEADER DosHeader = nullptr;
		PIMAGE_NT_HEADERS64 NtHeaders = nullptr;

		auto Status = Pe::GetHeader(Base, DosHeader, NtHeaders);

		if (EfiError(Status))
		{
			return Status;
		}

		auto& Directory = NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];

		if (!Directory.VirtualAddress || !Directory.Size)
		{
			return EFI_SUCCESS;
		}

		auto Tls = reinterpret_cast<PIMAGE_TLS_DIRECTORY64>(Base + Directory.VirtualAddress);

		if (Tls->AddressOfIndex)
		{
			auto IndexPtr = reinterpret_cast<PUINT32>(Tls->AddressOfIndex);
			*IndexPtr = 0;
		}

		if (Alloc && OutTlsData && Tls->StartAddressOfRawData && Tls->EndAddressOfRawData)
		{
			auto Start = Tls->StartAddressOfRawData;
			auto End = Tls->EndAddressOfRawData;

			if (End > Start)
			{
				auto RawSize = End - Start;
				auto TotalSize = RawSize + static_cast<UINT64>(Tls->SizeOfZeroFill);

				auto TlsData = Alloc(TotalSize);

				if (!TlsData)
				{
					return EFI_OUT_OF_RESOURCES;
				}

				Util::CopyMemory(reinterpret_cast<PVOID>(Start), TlsData, RawSize,false);

				if (Tls->SizeOfZeroFill)
				{
					Util::SetMemory(reinterpret_cast<PUINT8>(TlsData) + RawSize, static_cast<UINT64>(Tls->SizeOfZeroFill), 0, false);
				}

				*OutTlsData = TlsData;
			}
		}

		if (Tls->AddressOfCallBacks)
		{
			auto Callbacks = reinterpret_cast<TlsCallbackFn**>(Tls->AddressOfCallBacks);

			for (; *Callbacks; ++Callbacks)
			{
				(*Callbacks)(reinterpret_cast<PVOID>(Base), static_cast<UINT32>(Reason), Reserved);
			}
		}

		return EFI_SUCCESS;
	}

	template<typename T, typename U>
	auto
	RemapImage(
		IN	T				SourceBase,
		IN	U				TargetBase,
		IN	FindModuleFn*	FindModule,
		IN	CUINT32			MaxForwardDepth = 8,
		IN	AllocFn*		AllocTls = nullptr,
		OUT	PVOID*			OutTlsData = nullptr
	) -> EFI_STATUS
	{
		if (OutTlsData)
		{
			*OutTlsData = nullptr;
		}

		auto Src = ToPtr(SourceBase);
		auto Dst = ToPtr(TargetBase);

		if (!Src || !Dst || !FindModule)
		{
			return EFI_INVALID_PARAMETER;
		}

		PIMAGE_DOS_HEADER SrcDos = nullptr;
		PIMAGE_NT_HEADERS64 SrcNt = nullptr;

		auto Status = Pe::GetHeader(Src, SrcDos, SrcNt);

		if (EfiError(Status))
		{
			return Status;
		}

		auto ImageSize = static_cast<UINT64>(SrcNt->OptionalHeader.SizeOfImage);

		if (!ImageSize)
		{
			return EFI_COMPROMISED_DATA;
		}

		Status = Util::CopyMemory(Src, Dst, ImageSize, false);

		if (EfiError(Status))
		{
			return Status;
		}

		Status = Pe::ApplyRelocations(Dst);

		if (EfiError(Status))
		{
			return Status;
		}

		Status = Pe::ResolveImports(Dst, FindModule, MaxForwardDepth, Src);

		if (EfiError(Status))
		{
			return Status;
		}

		Status = Pe::ProcessTls(Dst, DLL_PROCESS_ATTACH, nullptr, AllocTls, OutTlsData);

		if (EfiError(Status))
		{
			return Status;
		}

		return EFI_SUCCESS;
	}

	template<typename T>
	auto
	CalculateCheckSum(
		IN	T		File,
		IN	CUINT32	FileSize,
		IN	CUINT32	CheckSumOffset,
		OUT	UINT32&	CheckSum
	) -> EFI_STATUS
	{
		CheckSum = 0;

		auto Base = ToPtr(File);

		if (!Base || !FileSize)
		{
			return EFI_INVALID_PARAMETER;
		}

		if (CheckSumOffset >= FileSize || (CheckSumOffset + 3) >= FileSize)
		{
			return EFI_INVALID_PARAMETER;
		}

		UINT64 Sum = 0;

		for (UINT32 i = 0; i < FileSize; i += 2)
		{
			if (i == CheckSumOffset || i == (CheckSumOffset + 2))
			{
				continue;
			}

			UINT16 Word = 0;

			if ((i + 1) < FileSize)
			{
				Word = static_cast<UINT16>(static_cast<UINT16>(Base[i]) | (static_cast<UINT16>(Base[i + 1]) << 8));
			}
			else
			{
				Word = static_cast<UINT16>(Base[i]);
			}

			Sum += Word;
			Sum = (Sum & 0xFFFF) + (Sum >> 16);
		}

		Sum = (Sum & 0xFFFF) + (Sum >> 16);
		Sum = (Sum & 0xFFFF) + (Sum >> 16);

		CheckSum = static_cast<UINT32>(Sum + FileSize);
		return EFI_SUCCESS;
	}
}