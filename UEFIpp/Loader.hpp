#pragma once

#include "UEFIpp.hpp"

namespace Loader
{
	EFI_STATUS
	LoadImage(
		IN	PCSTR						ImagePath,
		OUT	EFI_LOADED_IMAGE_PROTOCOL*& LoadedImage,
		OUT	EFI_HANDLE&					ImageHandle
	);

	EFI_STATUS
	LoadFile(
		IN	PCSTR	FilePath,
		OUT	PVOID&	FileBuffer,
		OUT	UINT64& FileSize
	);

	auto
	GetImageByGuid(
		IN	CEFI_GUID&					ImageGuid,
		OUT	PEFI_LOADED_IMAGE_PROTOCOL& LoadedImage,
		OUT	EFI_HANDLE&					ImageHandle
	) -> EFI_STATUS;
}