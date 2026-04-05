#include "Util.hpp"

EFI_STATUS
Util::StrWToStr(
	IN OUT PWSTR& StrW,
	OUT PSTR& Str,
	IN CBOOLEAN UseHeap,
	IN CBOOLEAN FreeStrW
)
{
	Str = nullptr;

	if (!StrW)
	{
		return EFI_INVALID_PARAMETER;
	}

	UINT64 Length = 0;

	while (StrW[Length])
	{
		++Length;
	}

	PSTR Out = nullptr;

	if (UseHeap)
	{
		Out = new CHAR[Length + 1];

		if (!Out)
		{
			return EFI_OUT_OF_RESOURCES;
		}
	}
	else
	{
		if (Length + 1 > gStrWToStrStaticCap)
		{
			return EFI_BUFFER_TOO_SMALL;
		}

		static CHAR Buffer[gStrWToStrStaticCap] = { 0 };

		Out = Buffer;
	}

	for (UINT64 i = 0; i < Length; ++i)
	{
		CWCHAR CharW = StrW[i];
		Out[i] = (CharW <= 0x7F) ? static_cast<CHAR>(CharW) : '?';
	}

	Out[Length] = '\0';
	Str = Out;

	if (FreeStrW)
	{
		delete[] StrW;
		StrW = nullptr;
	}

	return EFI_SUCCESS;
}

EFI_STATUS
Util::StrToStrW(
	IN OUT PSTR& Str,
	OUT PWSTR& StrW,
	IN CBOOLEAN UseHeap,
	IN CBOOLEAN FreeStr
)
{
	StrW = nullptr;

	if (!Str)
	{
		return EFI_INVALID_PARAMETER;
	}

	UINT64 Length = 0;

	while (Str[Length])
	{
		++Length;
	}

	PWSTR Out = nullptr;

	if (UseHeap)
	{
		Out = new WCHAR[Length + 1];

		if (!Out)
		{
			return EFI_OUT_OF_RESOURCES;
		}
	}
	else
	{
		if (Length + 1 > gStrToStrWStaticCap)
		{
			return EFI_BUFFER_TOO_SMALL;
		}

		static WCHAR Buffer[gStrToStrWStaticCap] = { 0 };

		Out = Buffer;
	}

	for (UINT64 i = 0; i < Length; ++i)
	{
		Out[i] = static_cast<WCHAR>(Str[i]);
	}

	Out[Length] = L'\0';
	StrW = Out;

	if (FreeStr)
	{
		delete[] Str;
		Str = nullptr;
	}

	return EFI_SUCCESS;
}

EFI_STATUS
Util::CompareStr(
	IN	PCSTR	Str1,
	IN	PCSTR	Str2
)
{
	if (!Str1 || !Str2)
	{
		return EFI_INVALID_PARAMETER;
	}

	for (;;)
	{
		CHAR C1 = *Str1++;
		CHAR C2 = *Str2++;

		if (C1 != C2)
		{
			return EFI_NOT_FOUND;
		}

		if (C1 == '\0')
		{
			return EFI_SUCCESS;
		}
	}
}


EFI_STATUS
Util::CompareStrW(
	IN	PCWSTR	Str1,
	IN	PCWSTR	Str2
)
{
	if (!Str1 || !Str2)
	{
		return EFI_INVALID_PARAMETER;
	}

	for (;;)
	{
		WCHAR C1 = *Str1++;
		WCHAR C2 = *Str2++;

		if (C1 != C2)
		{
			return EFI_NOT_FOUND;
		}

		if (C1 == L'\0')
		{
			return EFI_SUCCESS;
		}
	}
}

EFI_STATUS
Util::CompareMem(
	IN  PCVOID  Buf1,
	IN  PCVOID  Buf2,
	IN  CUINT64 Length
)
{
	if (!Buf1 || !Buf2)
	{
		return EFI_INVALID_PARAMETER;
	}

	if (!Length)
	{
		return EFI_SUCCESS;
	}

	PCUINT8 A = reinterpret_cast<PCUINT8>(Buf1);
	PCUINT8 B = reinterpret_cast<PCUINT8>(Buf2);

	for (UINT64 i = 0; i < Length; ++i)
	{
		if (A[i] != B[i])
		{
			return EFI_NOT_FOUND;
		}
	}

	return EFI_SUCCESS;
}