#pragma once

#include "UEFIpp.hpp"

using SymbolAllocFn = PVOID(__fastcall)(
	IN	CUINT64	Size
);

using SymbolFreeFn = VOID(__fastcall)(
	IN	PCVOID	Address
);

#pragma pack(push, 1)
typedef struct _MSF_SUPERBLOCK7
{
	CHAR	Magic[32];
	UINT32	BlockSize;
	UINT32	FreeBlockMapBlock;
	UINT32	NumBlocks;
	UINT32	NumDirectoryBytes;
	UINT32	Unknown;
	UINT32	BlockMapAddr;
} MSF_SUPERBLOCK7, *PMSF_SUPERBLOCK7;

using CMSF_SUPERBLOCK7	= const MSF_SUPERBLOCK7;
using PCMSF_SUPERBLOCK7 = const MSF_SUPERBLOCK7*;

typedef struct _DBI_STREAM_HEADER
{
	UINT32	VersionSignature;
	UINT32	VersionHeader;
	UINT32	Age;
	UINT16	GlobalSymbolStreamIndex;
	UINT16	BuildNumber;
	UINT16	PublicSymbolStreamIndex;
	UINT16	PdbDllVersion;
	UINT16	SymRecordStreamIndex;
	UINT16	PdbDllRbld;
	INT32	ModInfoSize;
	INT32	SectionContributionSize;
	INT32	SectionMapSize;
	INT32	FileInfoSize;
	INT32	TypeServerMapSize;
	UINT32	MFCTypeServerIndex;
	INT32	OptionalDbgHeaderSize;
	INT32	ECSubstreamSize;
	UINT16	Flags;
	UINT16	Machine;
	UINT32	Padding;
} DBI_STREAM_HEADER, *PDBI_STREAM_HEADER;

using CDBI_STREAM_HEADER	= const DBI_STREAM_HEADER;
using PCDBI_STREAM_HEADER	= const DBI_STREAM_HEADER*;
#pragma pack(pop)

static constexpr CHAR MSF7_MAGIC[32] = { 'M','i','c','r','o','s','o','f','t',' ','C','/','C','+','+',' ','M','S','F',' ','7','.','0','0','\r','\n',0x1A,'D','S',0,0,0 };

typedef struct _MSF_CONTEXT
{
	PUINT8			PdbBase;
	UINT32			PdbSize;
	MSF_SUPERBLOCK7	Sb;
	UINT32			StreamCount;
	PUINT32			StreamSizes;
	PUINT32*		StreamBlocks;
	PUINT32			StreamNumBlocks;
} MSF_CONTEXT, *PMSF_CONTEXT;

using CMSF_CONTEXT	= const MSF_CONTEXT;
using PCMSF_CONTEXT = const MSF_CONTEXT*;

class SymbolResolver
{
public:
	SymbolResolver();
	~SymbolResolver();

	SymbolResolver(const SymbolResolver&) = delete;
	SymbolResolver& operator=(const SymbolResolver&) = delete;

	BOOLEAN
	Init(
		IN	PCVOID						PdbBase,
		IN	CUINT32						PdbSize,
		IN	EFI_IMAGE_SECTION_HEADER*	Sections,
		IN	CUINT16						SectionCount,
		IN	CUINT32						ImageSize,
		IN	CUINT32						MapCapacityPow2 = (1U << 16)
	);

	BOOLEAN
	InitForLoadedImage(
		IN	PCVOID			PdbBase,
		IN	CUINT64			PdbSize,
		IN	PCVOID			ImageBase,
		IN	SymbolAllocFn*	SymbolAlloc,
		IN	SymbolFreeFn*	SymbolFree
	);

	VOID
	Reset(
		VOID
	);

	BOOLEAN
	IsReady(
		VOID
	) const;

	BOOLEAN
	FindRva(
		IN	PCSTR	Name,
		OUT	UINT32&	Rva
	) const;

	PVOID
	FindVaRaw(
		IN	CUINT64	ImageBase,
		IN	PCSTR	Name
	) const;

	template<typename T>
	T
	FindVa(
		IN	CUINT64	ImageBase,
		IN	PCSTR	Name
	) const
	{
		return reinterpret_cast<T>(FindVaRaw(ImageBase, Name));
	}

	BOOLEAN
	FindRvaByHash(
		IN	CUINT64	NameHash,
		OUT UINT32&	Rva
	) const;

	PVOID
	FindVaByHash(
		IN	PCVOID ImageBase,
		IN	CUINT64 NameHash
	) const;

	UINT32
	Capacity(
		VOID
	) const;

	UINT32
	Used(
		VOID
	) const;

private:
	typedef struct _MAP MAP, *PMAP;
	PMAP Map_;

	PUINT8 PdbBase_;
	UINT32 PdbSize_;

	EFI_IMAGE_SECTION_HEADER* Sections_;
	UINT16 SectionCount_;
	UINT32 ImageSize_;

	BOOLEAN Ready_;

	SymbolAllocFn* SymbolAlloc_;
	SymbolFreeFn* SymbolFree_;

	BOOLEAN
	InitMap(
		IN	PMAP	Map,
		IN	CUINT32	Capacity
	);

	VOID
	FreeMap(
		IN	PMAP	Map
	);

	BOOLEAN
	LoadSymbolsFromPdb(
		IN		PUINT8						PdbBase,
		IN		CUINT32						PdbSize,
		IN		EFI_IMAGE_SECTION_HEADER*	Sections,
		IN		CUINT16						SectionCount,
		IN		CUINT32						ImageSize,
		IN OUT	PMAP						Map
	);

	BOOLEAN
	InitMsf(
		IN OUT	PMSF_CONTEXT	Ctx,
		IN		PUINT8			PdbBase,
		IN		CUINT32			PdbSize
	);

	PUINT8
	ReadMsfStream(
		IN	PCMSF_CONTEXT	Ctx,
		IN	CUINT32			StreamIndex,
		OUT	UINT32&			Size
	);

	VOID
	FreeMsf(
		IN	PMSF_CONTEXT	Ctx
	);

	BOOLEAN
	FindDbiStreamIndex(
		IN	PCMSF_CONTEXT		Ctx,
		OUT	UINT32&				OutDbiStreamIndex,
		OUT	DBI_STREAM_HEADER& OutDbiHeader
	);

	PVOID
	AllocateSafe(
		IN	CUINT64	Size
	);

	VOID
	FreeSafe(
		IN	PCVOID	Buffer
	);
};