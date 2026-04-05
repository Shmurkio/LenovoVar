#include "Symbols.hpp"

static
inline
UINT16
ReadU16(
	IN	PCVOID	Ptr
)
{
	PCUINT8 B = reinterpret_cast<PCUINT8>(Ptr);
	return static_cast<UINT16>(B[0] | (B[1] << 8));
}

static
inline
UINT32
ReadU32(
	IN	PCVOID	Ptr
)
{
	PCUINT8 B = reinterpret_cast<PCUINT8>(Ptr);
	return static_cast<UINT32>(B[0] | (B[1] << 8) | (B[2] << 16) | (B[3] << 24));
}

namespace Hash
{
	static constexpr CUINT64 FNV_OFFSET	= 0xCBF29CE484222325ULL;
	static constexpr CUINT64 FNV_PRIME	= 0x100000001B3ULL;

	static
	UINT64
	Fnv1a(
		IN	PCSTR	Str,
		IN	CUINT64	Length
	)
	{
		if (!Str || !*Str || !Length)
		{
			return 0;
		}

		UINT64 Hash = FNV_OFFSET;

		for (UINT64 i = 0; i < Length; ++i)
		{
			Hash ^= static_cast<CUINT8>(Str[i]);
			Hash *= FNV_PRIME;
		}

		return Hash;
	}
}

typedef struct _ENTRY
{
	UINT64	NameHash;
	UINT32	Rva;
} ENTRY, *PENTRY;

using CENTRY	= const ENTRY;
using PCENTRY	= const ENTRY*;

typedef struct _MAP
{
	PENTRY	Table;
	UINT32	Capacity;
	UINT32	Used;
} MAP, *PMAP;

using CMAP	= const MAP;
using PCMAP	= const MAP*;

static constexpr CUINT16 S_PUB32		= 0x110E;
static constexpr CUINT16 S_GPROC32		= 0x110F;
static constexpr CUINT16 S_LPROC32		= 0x1110;
static constexpr CUINT16 S_GPROC32_ID	= 0x1147;
static constexpr CUINT16 S_LPROC32_ID	= 0x1146;

PVOID
SymbolResolver::AllocateSafe(
	IN	CUINT64	Size
)
{
	if (!Size)
	{
		return nullptr;
	}

	if (SymbolResolver::SymbolAlloc_)
	{
		return SymbolResolver::SymbolAlloc_(Size);
	}
	else
	{
		PVOID Buffer = nullptr;
		gBS->AllocatePool(EfiBootServicesData, Size, &Buffer);
		return Buffer;
	}
}

VOID
SymbolResolver::FreeSafe(
	IN	PCVOID	Buffer
)
{
	if (!Buffer)
	{
		return;
	}

	if (SymbolResolver::SymbolFree_)
	{
		SymbolResolver::SymbolFree_(Buffer);
	}
	else
	{
		gBS->FreePool(const_cast<PVOID>(Buffer));
	}
}

BOOLEAN
SymbolResolver::InitMap(
	IN	PMAP	Map,
	IN	CUINT32	Capacity
)
{
	if (!Map || !Capacity || Capacity & (Capacity - 1))
	{
		return false;
	}

	Map->Capacity = Capacity;
	Map->Used = 0;

	Map->Table = reinterpret_cast<PENTRY>(SymbolResolver::AllocateSafe(Capacity * sizeof(ENTRY)));

	if (!Map->Table)
	{
		return false;
	}

	for (UINT32 i = 0; i < Capacity; ++i)
	{
		Map->Table[i].NameHash = 0;
		Map->Table[i].Rva = 0;
	}

	return true;
}

VOID
SymbolResolver::FreeMap(
	IN	PMAP	Map
)
{
	if (!Map)
	{
		return;
	}

	SymbolResolver::FreeSafe(Map->Table);

	Map->Table = nullptr;
	Map->Capacity = 0;
	Map->Used = 0;
}

static
BOOLEAN
Insert(
	IN	PMAP	Map,
	IN	CUINT64	NameHash,
	IN	CUINT32	Rva
)
{
	if (!Map || !Map->Table || !NameHash)
	{
		return false;
	}

	UINT32 Mask = Map->Capacity - 1;
	UINT32 Idx = NameHash & Mask;

	for (UINT32 i = 0; i < Map->Capacity; ++i)
	{
		ENTRY& Entry = Map->Table[Idx];

		if (!Entry.NameHash)
		{
			Entry.NameHash = NameHash;
			Entry.Rva = Rva;
			++Map->Used;
			return true;
		}

		if (Entry.NameHash == NameHash)
		{
			return true;
		}

		Idx = (Idx + 1) & Mask;
	}

	return false;
}

static
BOOLEAN
Find(
	IN	PCMAP	Map,
	IN	CUINT64	NameHash,
	OUT	UINT32& Rva
)
{
	Rva = 0;

	if (!Map || !Map->Table)
	{
		return false;
	}

	UINT32 Mask = Map->Capacity - 1;
	UINT32 Idx = NameHash & Mask;

	for (UINT32 i = 0; i < Map->Capacity; ++i)
	{
		CENTRY& Entry = Map->Table[Idx];

		if (!Entry.NameHash)
		{
			return false;
		}

		if (Entry.NameHash == NameHash)
		{
			Rva = Entry.Rva;
			return true;
		}

		Idx = (Idx + 1) & Mask;
	}

	return false;
}

static
BOOLEAN
IsCStrTerminatedWithin(
	IN	PCSTR	Str,
	IN	CUINT64	MaxLength,
	OUT	UINT64& Length
)
{
	Length = 0;

	if (!Str || !MaxLength)
	{
		return false;
	}

	for (UINT64 i = 0; i < MaxLength; ++i)
	{
		if (Str[i] == '\0')
		{
			Length = i;
			return true;
		}
	}

	return false;
}

static
BOOLEAN
SegmentOffsetToRva(
	IN	EFI_IMAGE_SECTION_HEADER*	Sections,
	IN	CUINT16						SectionCount,
	IN	CUINT32						ImageSize,
	IN	CUINT16						Segment,
	IN	CUINT32						Offset,
	OUT	UINT32&						Rva
)
{
	Rva = 0;

	if (!Sections || !Segment)
	{
		return false;
	}

	UINT32 Idx = Segment - 1;

	if (Idx >= SectionCount)
	{
		return false;
	}

	UINT32 TempRva = Sections[Idx].VirtualAddress + Offset;

	if (TempRva >= ImageSize)
	{
		return false;
	}

	Rva = TempRva;
	return true;
}

static
BOOLEAN
ParseSymRecordStream(
	IN	PUINT8						Stream,
	IN	CUINT32						StreamSize,
	IN	EFI_IMAGE_SECTION_HEADER*	Sections,
	IN	CUINT16						SectionCount,
	IN	CUINT32						ImageSize,
	IN	PMAP						Map
)
{
	if (!Stream || !StreamSize || !Sections || !SectionCount || !Map)
	{
		return false;
	}

	PUINT8 End = Stream + StreamSize;

	for (PUINT8 i = Stream; i + 4 <= End; )
	{
		CUINT16 RecLength = ReadU16(i);
		i += 2;

		if (RecLength < 2 || i + RecLength > End)
		{
			break;
		}

		CUINT16 RecType = ReadU16(i);
		i += 2;

		PUINT8 Payload = i;
		CUINT32 PayloadLength = RecLength - 2;

		UINT16 Segment = 0;
		UINT32 Offset = 0;
		PSTR Name = nullptr;
		UINT64 NameLength = 0;

		switch (RecType)
		{
		case S_PUB32:
		{
			if (PayloadLength >= 10)
			{
				Offset = ReadU32(Payload + 4);
				Segment = ReadU16(Payload + 8);
				Name = reinterpret_cast<PSTR>(Payload + 10);

				UINT64 Max = PayloadLength - 10;

				if (!IsCStrTerminatedWithin(Name, Max, NameLength))
				{
					Name = nullptr;
				}
			}

			break;
		}
		case S_GPROC32:
		case S_LPROC32:
		case S_GPROC32_ID:
		case S_LPROC32_ID:
		{
			if (PayloadLength >= 35)
			{
				Offset = ReadU32(Payload + 28);
				Segment = ReadU16(Payload + 32);
				Name = reinterpret_cast<PSTR>(Payload + 35);

				UINT64 Max = PayloadLength - 35;

				if (!IsCStrTerminatedWithin(Name, Max, NameLength))
				{
					Name = nullptr;
				}
			}

			break;
		}
		default:
		{
			break;
		}
		}

		if (Name)
		{
			UINT32 Rva = 0;

			if (SegmentOffsetToRva(Sections, SectionCount, ImageSize, Segment, Offset, Rva))
			{
				UINT64 Hash = Hash::Fnv1a(Name, NameLength);
				Insert(Map, Hash, Rva);
			}
		}

		i = Payload + PayloadLength;

		if (reinterpret_cast<UINT64>(i) & 1)
		{
			i++;
		}
	}

	return true;
}

VOID
SymbolResolver::FreeMsf(
	IN	PMSF_CONTEXT	Ctx
)
{
	if (!Ctx)
	{
		return;
	}

	if (Ctx->StreamBlocks)
	{
		for (UINT32 i = 0; i < Ctx->StreamCount; ++i)
		{
			if (Ctx->StreamBlocks[i])
			{
				SymbolResolver::FreeSafe(Ctx->StreamBlocks[i]);
			}
		}

		SymbolResolver::FreeSafe(Ctx->StreamBlocks);
	}

	if (Ctx->StreamSizes)
	{
		SymbolResolver::FreeSafe(Ctx->StreamSizes);
	}

	if (Ctx->StreamNumBlocks)
	{
		SymbolResolver::FreeSafe(Ctx->StreamNumBlocks);
	}

	Ctx->PdbBase = nullptr;
	Ctx->PdbSize = 0;
	Ctx->StreamCount = 0;
	Ctx->StreamSizes = nullptr;
	Ctx->StreamBlocks = nullptr;
	Ctx->StreamNumBlocks = nullptr;
}

BOOLEAN
SymbolResolver::InitMsf(
	IN OUT	PMSF_CONTEXT	Ctx,
	IN		PUINT8			PdbBase,
	IN		CUINT32			PdbSize
)
{
	auto Fail = [&](PUINT8 Dir) -> BOOLEAN
	{
		if (Dir)
		{
			SymbolResolver::FreeSafe(Dir);
		}

		FreeMsf(Ctx);
		return false;
	};

	if (!Ctx || !PdbBase || PdbSize < sizeof(MSF_SUPERBLOCK7))
	{
		return false;
	}

	Ctx->PdbBase = PdbBase;
	Ctx->PdbSize = PdbSize;
	Ctx->StreamCount = 0;
	Ctx->StreamSizes = nullptr;
	Ctx->StreamBlocks = nullptr;

	Ctx->StreamNumBlocks = nullptr;

	PMSF_SUPERBLOCK7 Sb = reinterpret_cast<PMSF_SUPERBLOCK7>(PdbBase);
	
	if (EfiError(Util::CompareMem(Sb->Magic, reinterpret_cast<PCVOID>(MSF7_MAGIC), 32)))
	{
		return Fail(nullptr);
	}

	Ctx->Sb = *Sb;

	CUINT32 Bs = Ctx->Sb.BlockSize;

	if (!Bs || (Bs & (Bs - 1)))
	{
		return Fail(nullptr);
	}

	CUINT64 MapOff = static_cast<CUINT64>(Ctx->Sb.BlockMapAddr) * static_cast<CUINT64>(Bs);

	if (MapOff + Bs > PdbSize)
	{
		return Fail(nullptr);
	}

	PUINT32 DirBlockList = reinterpret_cast<PUINT32>(PdbBase + MapOff);

	CUINT32 DirBytes = Ctx->Sb.NumDirectoryBytes;
	CUINT32 DirBlocks = (DirBytes + Bs - 1) / Bs;

	if (!DirBlocks)
	{
		return Fail(nullptr);
	}

	PUINT8 Dir = reinterpret_cast<PUINT8>(SymbolResolver::AllocateSafe(static_cast<UINT64>(DirBlocks) * Bs));

	if (!Dir)
	{
		return Fail(nullptr);
	}

	for (UINT32 i = 0; i < DirBlocks; ++i)
	{
		CUINT32 B = DirBlockList[i];

		if (B >= Ctx->Sb.NumBlocks)
		{
			return Fail(Dir);
		}

		CUINT64 Off = static_cast<CUINT64>(B) * static_cast<CUINT64>(Bs);

		if (Off + Bs > PdbSize)
		{
			return Fail(Dir);
		}

		Util::CopyMemory(PdbBase + Off, Dir + (static_cast<CUINT64>(i) * Bs), Bs, false);
	}

	if (DirBytes < 4)
	{
		return Fail(Dir);
	}

	Ctx->StreamCount = ReadU32(Dir);

	if (!Ctx->StreamCount)
	{
		return Fail(Dir);
	}

	CUINT64 MinNeed = 4ULL + 4ULL * static_cast<CUINT64>(Ctx->StreamCount);

	if (MinNeed > DirBytes)
	{
		return Fail(Dir);
	}

	Ctx->StreamSizes = reinterpret_cast<PUINT32>(SymbolResolver::AllocateSafe(Ctx->StreamCount * sizeof(UINT32)));
	Ctx->StreamNumBlocks = reinterpret_cast<PUINT32>(SymbolResolver::AllocateSafe(Ctx->StreamCount * sizeof(UINT32)));
	Ctx->StreamBlocks = reinterpret_cast<PUINT32*>(SymbolResolver::AllocateSafe(Ctx->StreamCount * sizeof(PUINT32)));

	if (!Ctx->StreamSizes || !Ctx->StreamNumBlocks || !Ctx->StreamBlocks)
	{
		return Fail(Dir);
	}

	for (UINT32 i = 0; i < Ctx->StreamCount; ++i)
	{
		Ctx->StreamBlocks[i] = nullptr;
	}

	PUINT8 P = Dir + 4;
	PUINT8 DirEnd = Dir + DirBytes;

	for (UINT32 i = 0; i < Ctx->StreamCount; ++i)
	{
		if (P + 4 > DirEnd)
		{
			return Fail(Dir);
		}

		UINT32 Sz = ReadU32(P); P += 4;
		Ctx->StreamSizes[i] = Sz;

		if (Sz == 0xFFFFFFFFU)
		{
			Ctx->StreamNumBlocks[i] = 0;
			continue;
		}

		Ctx->StreamNumBlocks[i] = (Sz + Bs - 1) / Bs;
	}

	for (UINT32 i = 0; i < Ctx->StreamCount; ++i)
	{
		UINT32 Sz = Ctx->StreamSizes[i];
		UINT32 Nb = Ctx->StreamNumBlocks[i];

		if (Sz == 0xFFFFFFFFu || !Nb)
		{
			continue;
		}

		if (P + static_cast<CUINT64>(Nb) * 4 > DirEnd)
		{
			return Fail(Dir);
		}

		Ctx->StreamBlocks[i] = reinterpret_cast<PUINT32>(SymbolResolver::AllocateSafe(Nb * sizeof(UINT32)));

		if (!Ctx->StreamBlocks[i])
		{
			return Fail(Dir);
		}

		for (UINT32 j = 0; j < Nb; ++j)
		{
			UINT32 Blk = ReadU32(P);
			P += 4;

			if (Blk >= Ctx->Sb.NumBlocks)
			{
				return Fail(Dir);
			}

			Ctx->StreamBlocks[i][j] = Blk;
		}
	}

	SymbolResolver::FreeSafe(Dir);

	return true;
}

PUINT8
SymbolResolver::ReadMsfStream(
	IN	PCMSF_CONTEXT	Ctx,
	IN	CUINT32			StreamIndex,
	OUT	UINT32&			Size
)
{
	Size = 0;

	if (!Ctx || !Ctx->PdbBase || StreamIndex >= Ctx->StreamCount)
	{
		return nullptr;
	}

	CUINT32 Sz = Ctx->StreamSizes[StreamIndex];

	if (Sz == 0xFFFFFFFFU)
	{
		return nullptr;
	}

	Size = Sz;

	PUINT8 Buffer = nullptr;

	UINT64 Bytes = Sz ? Sz : 1;

	Buffer = reinterpret_cast<PUINT8>(SymbolResolver::AllocateSafe(Bytes));

	if (!Buffer)
	{
		return nullptr;
	}

	CUINT32 Bs = Ctx->Sb.BlockSize;
	CUINT32 Nb = Ctx->StreamNumBlocks[StreamIndex];
	PUINT32 Bl = Ctx->StreamBlocks[StreamIndex];

	UINT32 Left = Sz;
	PUINT8 W = Buffer;

	for (UINT32 i = 0; i < Nb; ++i)
	{
		CUINT32 BlockIndex = Bl[i];

		if (BlockIndex >= Ctx->Sb.NumBlocks)
		{
			SymbolResolver::FreeSafe(Buffer);
			Size = 0;
			return nullptr;
		}

		CUINT64 Off = static_cast<CUINT64>(BlockIndex) * static_cast<CUINT64>(Bs);

		if (Off + Bs > Ctx->PdbSize)
		{
			SymbolResolver::FreeSafe(Buffer);
			Size = 0;
			return nullptr;
		}

		UINT32 Copy = (Left > Bs) ? Bs : Left;

		Util::CopyMemory(Ctx->PdbBase + Off, W, Copy, false);

		W += Copy;
		Left -= Copy;

		if (!Left)
		{
			break;
		}
	}

	return Buffer;
}

static
BOOLEAN
LooksLikeDbi(
	IN	PCDBI_STREAM_HEADER	H,
	IN	CUINT32				StreamSize,
	IN	CUINT32				TotalStreams
)
{
	if (!H || StreamSize < sizeof(DBI_STREAM_HEADER))
	{
		return false;
	}

	if (H->VersionSignature != 0xFFFFFFFFU)
	{
		return false;
	}

	if (H->SymRecordStreamIndex >= TotalStreams)
	{
		return false;
	}

	if (H->ModInfoSize < 0 || H->SectionMapSize < 0 || H->FileInfoSize < 0)
	{
		return false;
	}

	return true;
}

BOOLEAN
SymbolResolver::FindDbiStreamIndex(
	IN	PCMSF_CONTEXT		Ctx,
	OUT	UINT32&				OutDbiStreamIndex,
	OUT	DBI_STREAM_HEADER&	OutDbiHeader
)
{
	OutDbiStreamIndex = 0xFFFFFFFFu;
	OutDbiHeader = {};

	if (!Ctx)
	{
		return false;
	}

	for (UINT32 i = 0; i < Ctx->StreamCount; ++i)
	{
		UINT32 Sz = 0;
		PUINT8 Buf = ReadMsfStream(Ctx, i, Sz);

		if (!Buf)
		{
			continue;
		}

		if (Sz >= sizeof(DBI_STREAM_HEADER))
		{
			PCDBI_STREAM_HEADER H = reinterpret_cast<PCDBI_STREAM_HEADER>(Buf);

			if (LooksLikeDbi(H, Sz, Ctx->StreamCount))
			{
				OutDbiStreamIndex = i;
				OutDbiHeader = *H;
				SymbolResolver::FreeSafe(Buf);
				return true;
			}
		}

		SymbolResolver::FreeSafe(Buf);
	}

	return false;
}

BOOLEAN
SymbolResolver::LoadSymbolsFromPdb(
	IN		PUINT8						PdbBase,
	IN		CUINT32						PdbSize,
	IN		EFI_IMAGE_SECTION_HEADER*	Sections,
	IN		CUINT16						SectionCount,
	IN		CUINT32						ImageSize,
	IN OUT	PMAP						Map
)
{
	if (!PdbBase || !PdbSize || !Sections || !SectionCount || !ImageSize || !Map)
	{
		return false;
	}

	MSF_CONTEXT Msf{};

	if (!InitMsf(&Msf, PdbBase, PdbSize))
	{
		return false;
	}

	BOOLEAN Status = false;

	UINT32 DbiIndex = 0xFFFFFFFFU;
	DBI_STREAM_HEADER DbiHdr{};

	if (!FindDbiStreamIndex(&Msf, DbiIndex, DbiHdr))
	{
		FreeMsf(&Msf);
		return false;
	}

	UINT32 SymRecSize = 0;
	PUINT8 SymRec = ReadMsfStream(&Msf, DbiHdr.SymRecordStreamIndex, SymRecSize);

	if (!SymRec || !SymRecSize)
	{
		SymbolResolver::FreeSafe(SymRec);
		FreeMsf(&Msf);
		return false;
	}

	if (!Map->Table)
	{
		if (!InitMap(Map, 1U << 16))
		{
			SymbolResolver::FreeSafe(SymRec);
			FreeMsf(&Msf);
			return false;
		}
	}

	Status = ParseSymRecordStream(SymRec, SymRecSize, Sections, SectionCount, ImageSize, Map);

	SymbolResolver::FreeSafe(SymRec);

	FreeMsf(&Msf);
	return Status;
}

static
BOOLEAN
FindSymbolRva(
	IN	PCMAP	Map,
	IN	PCSTR	Name,
	OUT	UINT32& Rva
)
{
	Rva = 0;

	if (!Name)
	{
		return false;
	}

	UINT64 Length = 0;

	while (Name[Length])
	{
		++Length;
	}

	UINT64 Hash = Hash::Fnv1a(Name, Length);
	return Find(Map, Hash, Rva);
}

static
PVOID
FindSymbolVa(
	IN	PCMAP	Map,
	IN	PCVOID	ImageBase,
	IN	PCSTR	Name
)
{
	UINT32 Rva = 0;

	if (!FindSymbolRva(Map, Name, Rva))
	{
		return nullptr;
	}

	return const_cast<PUINT8>(reinterpret_cast<PCUINT8>(ImageBase)) + Rva;
}

SymbolResolver::SymbolResolver()
	: Map_(nullptr),
	PdbBase_(nullptr),
	PdbSize_(0),
	Sections_(nullptr),
	SectionCount_(0),
	ImageSize_(0),
	Ready_(false)
{
}

SymbolResolver::~SymbolResolver()
{
	Reset();
}

BOOLEAN
SymbolResolver::Init(
	IN	PCVOID						PdbBase,
	IN	CUINT32						PdbSize,
	IN	EFI_IMAGE_SECTION_HEADER*	Sections,
	IN	CUINT16						SectionCount,
	IN	CUINT32						ImageSize,
	IN	CUINT32						MapCapacityPow2
)
{
	Reset();

	if (!PdbBase || !PdbSize || !Sections || !SectionCount || !ImageSize)
	{
		return false;
	}

	Map_ = reinterpret_cast<PMAP>(SymbolResolver::AllocateSafe(sizeof(MAP)));

	if (!Map_)
	{
		return false;
	}

	Map_->Table = nullptr;
	Map_->Capacity = 0;
	Map_->Used = 0;

	if (!InitMap(Map_, MapCapacityPow2))
	{
		SymbolResolver::FreeSafe(Map_);
		Map_ = nullptr;
		return false;
	}

	PdbBase_ = const_cast<PUINT8>(reinterpret_cast<PCUINT8>(PdbBase));
	PdbSize_ = PdbSize;
	Sections_ = Sections;
	SectionCount_ = SectionCount;
	ImageSize_ = ImageSize;

	if (!LoadSymbolsFromPdb(PdbBase_, PdbSize_, Sections_, SectionCount_, ImageSize_, Map_))
	{
		Reset();
		return false;
	}

	Ready_ = true;
	return true;
}

VOID
SymbolResolver::Reset(
	VOID
)
{
	Ready_ = false;

	if (Map_)
	{
		FreeMap(Map_);
		SymbolResolver::FreeSafe(Map_);
		Map_ = nullptr;
	}

	PdbBase_ = nullptr;
	PdbSize_ = 0;

	Sections_ = nullptr;
	SectionCount_ = 0;
	ImageSize_ = 0;
}

BOOLEAN
SymbolResolver::IsReady(
	VOID
) const
{
	return Ready_;
}

UINT32
SymbolResolver::Capacity(
	VOID
) const
{
	return (Map_) ? Map_->Capacity : 0;
}

UINT32
SymbolResolver::Used(
	VOID
) const
{
	return (Map_) ? Map_->Used : 0;
}

BOOLEAN
SymbolResolver::FindRva(
	IN	PCSTR	Name,
	OUT	UINT32&	Rva
) const
{
	Rva = 0;

	if (!Ready_ || !Map_ || !Name)
	{
		return false;
	}

	UINT64 Len = 0;

	while (Name[Len])
	{
		++Len;
	}

	UINT64 Hash = Hash::Fnv1a(Name, Len);
	return Find(Map_, Hash, Rva);
}

PVOID
SymbolResolver::FindVaRaw(
	IN	CUINT64	ImageBase,
	IN	PCSTR	Name
) const
{
	UINT32 Rva = 0;

	if (!FindRva(Name, Rva))
	{
		return nullptr;
	}

	return const_cast<PUINT8>(reinterpret_cast<PCUINT8>(ImageBase)) + Rva;
}

BOOLEAN
SymbolResolver::FindRvaByHash(
	IN	CUINT64	NameHash,
	OUT UINT32&	Rva
) const
{
	Rva = 0;

	if (!Ready_ || !Map_ || !NameHash)
	{
		return false;
	}

	return Find(Map_, NameHash, Rva);
}

PVOID
SymbolResolver::FindVaByHash(
	IN	PCVOID	ImageBase,
	IN	CUINT64	NameHash
) const
{
	UINT32 Rva = 0;

	if (!FindRvaByHash(NameHash, Rva))
	{
		return nullptr;
	}

	return const_cast<PUINT8>(reinterpret_cast<PCUINT8>(ImageBase)) + Rva;
}

BOOLEAN
SymbolResolver::InitForLoadedImage(
	IN	PCVOID			PdbBase,
	IN	CUINT64			PdbSize,
	IN	PCVOID			ImageBase,
	IN	SymbolAllocFn*	SymbolAlloc,
	IN	SymbolFreeFn*	SymbolFree
)
{
	if (!PdbBase || !PdbSize || !ImageBase)
	{
		return false;
	}

	SymbolResolver::SymbolAlloc_ = SymbolAlloc;
	SymbolResolver::SymbolFree_ = SymbolFree;

	EFI_IMAGE_NT_HEADERS64* Nt = nullptr;
	EFI_IMAGE_SECTION_HEADER* Sec = nullptr;
	UINT16 SecCount = 0;

	EFI_STATUS Status = Image::GetNtHeaders64(ImageBase, Nt, Sec, SecCount);

	if (EfiError(Status) || !Nt || !Sec || !SecCount)
	{
		return false;
	}

	return Init(PdbBase, static_cast<CUINT32>(PdbSize), Sec, SecCount, Nt->OptionalHeader.SizeOfImage);
}
