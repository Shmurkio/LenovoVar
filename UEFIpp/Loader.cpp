#include "Loader.hpp"
#include "Fs.hpp"

static
inline
BOOLEAN
IsEndNode(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	Node
)
{
	return Node->Type == END_DEVICE_PATH_TYPE;
}

static
inline
UINT16
DpLen(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	Node
)
{
	return static_cast<UINT16>(Node->Length[0] | (Node->Length[1] << 8));
}

static
inline
PCEFI_DEVICE_PATH_PROTOCOL
NextNode(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	Node
)
{
	return reinterpret_cast<PCEFI_DEVICE_PATH_PROTOCOL>(reinterpret_cast<PCUINT8>(Node) + DpLen(Node));
}

static
inline
UINT64
DevicePathSize(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	DevicePath
)
{
	UINT64 Size = 0;
	PCEFI_DEVICE_PATH_PROTOCOL Node = DevicePath;

	for (;;)
	{
		Size += DpLen(Node);

		if (IsEndNode(Node))
		{
			break;
		}

		Node = NextNode(Node);
	}

	return Size;
}

static
EFI_STATUS
BuildFileDevicePath(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	BaseDevicePath,
	IN	PCWSTR						FilePath,
	OUT	PEFI_DEVICE_PATH_PROTOCOL&	DevicePath
)
{
	DevicePath = nullptr;

	if (!BaseDevicePath || !FilePath || !*FilePath)
	{
		return EFI_INVALID_PARAMETER;
	}

	CUINT64 BaseSize = DevicePathSize(BaseDevicePath);

	if (BaseSize < sizeof(EFI_DEVICE_PATH_PROTOCOL))
	{
		return EFI_INVALID_PARAMETER;
	}

	CUINT64 BaseNoEndSize = BaseSize - sizeof(EFI_DEVICE_PATH_PROTOCOL);
	UINT64 PathChars = 0;

	for (PCWSTR p = FilePath; *p; ++p)
	{
		++PathChars;
	}

	CUINT64 FileNodeSize = (sizeof(FILEPATH_DEVICE_PATH) - sizeof(WCHAR)) + ((PathChars + 1) * sizeof(WCHAR));
	CUINT64 EndNodeSize = sizeof(EFI_DEVICE_PATH_PROTOCOL);
	CUINT64 TotalSize = BaseNoEndSize + FileNodeSize + EndNodeSize;
	PEFI_DEVICE_PATH_PROTOCOL NewPath = nullptr;
	EFI_STATUS Status = gBS->AllocatePool(EfiBootServicesData, TotalSize, reinterpret_cast<PVOID*>(&NewPath));

	if (EfiError(Status))
	{
		return Status;
	}

	{
		PCUINT8 Src = reinterpret_cast<PCUINT8>(BaseDevicePath);
		PUINT8 Dst = reinterpret_cast<PUINT8>(NewPath);

		for (UINT64 i = 0; i < BaseNoEndSize; ++i)
		{
			Dst[i] = Src[i];
		}
	}

	PFILEPATH_DEVICE_PATH FileNode = reinterpret_cast<PFILEPATH_DEVICE_PATH>(reinterpret_cast<PUINT8>(NewPath) + BaseNoEndSize);

	FileNode->Header.Type = MEDIA_DEVICE_PATH;
	FileNode->Header.SubType = MEDIA_FILEPATH_DP;

	FileNode->Header.Length[0] = static_cast<UINT8>(FileNodeSize & 0xFF);
	FileNode->Header.Length[1] = static_cast<UINT8>((FileNodeSize >> 8) & 0xFF);

	for (UINT64 i = 0; i < PathChars; ++i)
	{
		FileNode->PathName[i] = FilePath[i];
	}

	FileNode->PathName[PathChars] = L'\0';

	PEFI_DEVICE_PATH_PROTOCOL EndNode = reinterpret_cast<PEFI_DEVICE_PATH_PROTOCOL>(reinterpret_cast<PUINT8>(FileNode) + FileNodeSize);

	EndNode->Type = END_DEVICE_PATH_TYPE;
	EndNode->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
	EndNode->Length[0] = static_cast<UINT8>(EndNodeSize & 0xFF);
	EndNode->Length[1] = static_cast<UINT8>((EndNodeSize >> 8) & 0xFF);

	DevicePath = NewPath;

	return EFI_SUCCESS;
}

EFI_STATUS
Loader::LoadImage(
	IN	PCSTR						ImagePath,
	OUT	EFI_LOADED_IMAGE_PROTOCOL*&	LoadedImage,
	OUT	EFI_HANDLE&					ImageHandle
)
{
	LoadedImage = nullptr;
	ImageHandle = nullptr;

	if (!ImagePath || !*ImagePath)
	{
		return EFI_INVALID_PARAMETER;
	}

	PWSTR ImagePathW = nullptr;
	EFI_STATUS Status = Util::StrToStrW(const_cast<PSTR&>(ImagePath), ImagePathW, false, false);

	if (EfiError(Status))
	{
		return Status;
	}

	UINT64 FsCount = 0;
	PEFI_HANDLE FsHandles = nullptr;

	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, nullptr, &FsCount, &FsHandles);

	if (EfiError(Status))
	{
		return Status;
	}

	for (UINT64 i = 0; i < FsCount; ++i)
	{
		PEFI_DEVICE_PATH_PROTOCOL BaseDp = nullptr;
		Status = gBS->HandleProtocol(FsHandles[i], &gEfiDevicePathProtocolGuid, reinterpret_cast<PVOID*>(&BaseDp));

		if (EfiError(Status) || !BaseDp)
		{
			continue;
		}

		PEFI_DEVICE_PATH_PROTOCOL FullDp = nullptr;
		Status = BuildFileDevicePath(BaseDp, ImagePathW, FullDp);

		if (EfiError(Status) || !FullDp)
		{
			continue;
		}

		EFI_HANDLE Handle = nullptr;
		Status = gBS->LoadImage(false, gImageHandle, FullDp, nullptr, 0, &Handle);

		gBS->FreePool(FullDp);

		if (EfiError(Status) || !Handle)
		{
			continue;
		}

		PEFI_LOADED_IMAGE_PROTOCOL Image = nullptr;
		Status = gBS->HandleProtocol(Handle, &gEfiLoadedImageProtocolGuid, reinterpret_cast<PVOID*>(&Image));

		if (EfiError(Status) || !Image)
		{
			gBS->UnloadImage(Handle);
			continue;
		}

		LoadedImage = Image;
		ImageHandle = Handle;

		return EFI_SUCCESS;
	}

	if (FsHandles)
	{
		gBS->FreePool(FsHandles);
	}

	return EFI_NOT_FOUND;
}

EFI_STATUS
Loader::LoadFile(
	IN	PCSTR	FilePath,
	OUT	PVOID&	FileBuffer,
	OUT	UINT64& FileSize
)
{
	FileBuffer = nullptr;
	FileSize = 0;

	if (!FilePath || !*FilePath)
	{
		return EFI_INVALID_PARAMETER;
	}

	PWSTR FilePathW = nullptr;
	EFI_STATUS Status = Util::StrToStrW(const_cast<PSTR&>(FilePath), FilePathW, false, false);

	if (EfiError(Status))
	{
		return Status;
	}

	UINT64 FsCount = 0;
	PEFI_HANDLE FsHandles = nullptr;

	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, nullptr, &FsCount, &FsHandles);

	if (EfiError(Status))
	{
		return Status;
	}

	EFI_STATUS FinalStatus = EFI_NOT_FOUND;
	
	for (UINT64 i = 0; i < FsCount; ++i)
	{
		PEFI_SIMPLE_FILE_SYSTEM_PROTOCOL Fs = nullptr;
		Status = gBS->HandleProtocol(FsHandles[i], &gEfiSimpleFileSystemProtocolGuid, reinterpret_cast<PVOID*>(&Fs));

		if (EfiError(Status))
		{
			continue;
		}

		PEFI_FILE_PROTOCOL Root = nullptr;
		Status = Fs->OpenVolume(Fs, &Root);

		if (EfiError(Status))
		{
			continue;
		}

		PEFI_FILE_PROTOCOL File = nullptr;
		Status = Root->Open(Root, &File, FilePathW, EFI_FILE_MODE_READ, 0);

		Root->Close(Root);

		if (EfiError(Status))
		{
			continue;
		}

		UINT64 InfoSize = 0;
		Status = File->GetInfo(File, &gEfiFileInfoGuid, &InfoSize, nullptr);

		if (Status != EFI_BUFFER_TOO_SMALL || !InfoSize)
		{
			File->Close(File);
			continue;
		}

		PEFI_FILE_INFO Info = nullptr;
		Status = gBS->AllocatePool(EfiBootServicesData, InfoSize, reinterpret_cast<PVOID*>(&Info));

		if (EfiError(Status))
		{
			File->Close(File);
			FinalStatus = EFI_OUT_OF_RESOURCES;
			break;
		}

		Status = File->GetInfo(File, &gEfiFileInfoGuid, &InfoSize, Info);

		if (EfiError(Status))
		{
			gBS->FreePool(Info);
			File->Close(File);
			FinalStatus = EFI_NOT_FOUND;
		}

		if (!Info->FileSize)
		{
			gBS->FreePool(Info);
			File->Close(File);
			FinalStatus = EFI_NOT_FOUND;
			continue;
		}

		FileSize = Info->FileSize;
		gBS->FreePool(Info);

		PVOID Buffer = nullptr;
		Status = gBS->AllocatePool(EfiBootServicesData, FileSize, &Buffer);

		if (EfiError(Status))
		{
			FileSize = 0;
			File->Close(File);
			FinalStatus = EFI_OUT_OF_RESOURCES;
			break;
		}

		UINT64 ReadSize = FileSize;
		Status = File->Read(File, &ReadSize, Buffer);

		File->Close(File);

		if (EfiError(Status) || ReadSize != FileSize)
		{
			FileSize = 0;
			gBS->FreePool(Buffer);
			continue;
		}

		FileBuffer = Buffer;
		FinalStatus = EFI_SUCCESS;
		break;
	}

	if (FsHandles)
	{
		gBS->FreePool(FsHandles);
	}

	return FinalStatus;
}

static
inline
auto
IsGuidEqual(
	IN	CEFI_GUID&	Guid1,
	IN	CEFI_GUID&	Guid2
) -> BOOLEAN
{
	return (Guid1.Data1 == Guid2.Data1) &&
		(Guid1.Data2 == Guid2.Data2) &&
		(Guid1.Data3 == Guid2.Data3) &&
		(Guid1.Data4[0] == Guid2.Data4[0]) &&
		(Guid1.Data4[1] == Guid2.Data4[1]) &&
		(Guid1.Data4[2] == Guid2.Data4[2]) &&
		(Guid1.Data4[3] == Guid2.Data4[3]) &&
		(Guid1.Data4[4] == Guid2.Data4[4]) &&
		(Guid1.Data4[5] == Guid2.Data4[5]) &&
		(Guid1.Data4[6] == Guid2.Data4[6]) &&
		(Guid1.Data4[7] == Guid2.Data4[7]);
}

static
inline
auto
GetFileGuid(
	IN	PCEFI_DEVICE_PATH_PROTOCOL	FilePath,
	OUT	EFI_GUID&					FileGuid
) -> EFI_STATUS
{
	FileGuid = {};

	if (!FilePath)
	{
		return EFI_INVALID_PARAMETER;
	}

	for (auto Node = FilePath; !IsEndNode(Node); Node = NextNode(Node))
	{
		if (DevicePathType(Node) == MEDIA_DEVICE_PATH && DevicePathSubType(Node) == MEDIA_PIWG_FW_FILE_DP)
		{
			auto FvNode = reinterpret_cast<PMEDIA_FW_VOL_FILEPATH_DEVICE_PATH>(const_cast<PEFI_DEVICE_PATH_PROTOCOL>(Node));
			FileGuid = FvNode->FvFileName;
			return EFI_SUCCESS;
		}
	}

	return EFI_NOT_FOUND;
}

auto
Loader::GetImageByGuid(
	IN	CEFI_GUID&					ImageGuid,
	OUT	PEFI_LOADED_IMAGE_PROTOCOL& LoadedImage,
	OUT	EFI_HANDLE&					ImageHandle
) -> EFI_STATUS
{
	LoadedImage = nullptr;
	ImageHandle = nullptr;

	UINT64 HandleCount = 0;
	PEFI_HANDLE Handles = nullptr;

	auto Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiLoadedImageProtocolGuid, nullptr, &HandleCount, &Handles);

	if (EfiError(Status))
	{
		return Status;
	}

	Status = EFI_NOT_FOUND;

	for (UINT64 i = 0; i < HandleCount; ++i)
	{
		PEFI_LOADED_IMAGE_PROTOCOL Image = nullptr;
		auto HandleStatus = gBS->HandleProtocol(Handles[i], &gEfiLoadedImageProtocolGuid, reinterpret_cast<PVOID*>(&Image));

		if (EfiError(HandleStatus) || !Image || !Image->FilePath)
		{
			continue;
		}

		EFI_GUID FileGuid = {};
		auto GuidStatus = GetFileGuid(Image->FilePath, FileGuid);

		if (EfiError(GuidStatus))
		{
			continue;
		}

		if (IsGuidEqual(FileGuid, ImageGuid))
		{
			LoadedImage = Image;
			ImageHandle = Handles[i];
			Status = EFI_SUCCESS;
			break;
		}
	}

	if (Handles)
	{
		gBS->FreePool(Handles);
	}

	return Status;
}