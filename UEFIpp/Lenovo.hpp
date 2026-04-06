#pragma once

#include "UEFIpp.hpp"

namespace Lenovo
{
	constexpr CEFI_GUID LENOVO_VARIABLE_PROTOCOL_GUID = { 0xBFD02359, 0x8DFE, 0x459A, { 0x8B, 0x69, 0xA7, 0x3A, 0x6B, 0xAF, 0xAD, 0xC0 } };

	typedef struct _LENOVO_VARIABLE_PROTOCOL LENOVO_VARIABLE_PROTOCOL, *PLENOVO_VARIABLE_PROTOCOL;

#pragma pack(push, 1)
	typedef struct _NAMESPACE_ID
	{
		UINT8					Bytes[0x0E];
	} NAMESPACE_ID, *PNAMESPACE_ID;

	using ENTRY_TYPE = UINT16;

	typedef struct _ENTRY_KEY
	{
		NAMESPACE_ID			NamespaceId;	// +0x00: Used to group entries in different categories
		ENTRY_TYPE				Type;			// +0x0E: Specifies the type of entry within the namespace
	} ENTRY_KEY, *PENTRY_KEY;
#pragma pack(pop)

	using CENTRY_KEY		= const ENTRY_KEY;
	using PCENTRY_KEY		= const ENTRY_KEY*;
	using CNAMESPACE_ID		= const NAMESPACE_ID;
	using PCNAMESPACE_ID	= const NAMESPACE_ID*;

	using GetEntryByKeyFn = EFI_STATUS(__fastcall)(PLENOVO_VARIABLE_PROTOCOL This, PCENTRY_KEY Key, PUINT32 Size, PVOID Data);
	using SetEntryByKeyFn = EFI_STATUS(__fastcall)(PLENOVO_VARIABLE_PROTOCOL This, PCENTRY_KEY Key, UINT32 Size, PVOID Data);
	using ProtectEntryByKeyFn = EFI_STATUS(__fastcall)(PLENOVO_VARIABLE_PROTOCOL This, PCENTRY_KEY Key, UINT32 Length, UINT32 Value);
	using UnprotectEntryByKeyFn = EFI_STATUS(__fastcall)(PLENOVO_VARIABLE_PROTOCOL This, PCENTRY_KEY Key, UINT32 Length, PUINT32 Value);

#pragma pack(push, 1)
	typedef struct _LENOVO_VARIABLE_PROTOCOL
	{
		GetEntryByKeyFn*		GetEntryByKey;				// +0x00
		SetEntryByKeyFn*		SetEntryByKey;				// +0x08
		ProtectEntryByKeyFn*	ProtectEntryByKey;			// +0x10
		UnprotectEntryByKeyFn*	UnprotectEntryByKey;		// +0x18
	} LENOVO_VARIABLE_PROTOCOL, *PLENOVO_VARIABLE_PROTOCOL;
#pragma pack(pop)

	constexpr const NAMESPACE_ID SMBIOS_NAMESPACE = { 0x55, 0x57, 0x0E, 0xC2, 0x69, 0x11, 0x56, 0x4C, 0xA4, 0x8A, 0x98, 0x24, 0xAB, 0x43 };

	constexpr const ENTRY_TYPE SMBIOS_ENTRY_WINDOWS_KEY				= 0x0001;
	constexpr const ENTRY_TYPE SMBIOS_ENTRY_OA3_KEY_ID				= 0x000B;
	constexpr const ENTRY_TYPE SMBIOS_ENTRY_MOTHERBOARD_NAME		= 0x0100;
	constexpr const ENTRY_TYPE SMBIOS_ENTRY_MACHINE_TYPE_MODEL		= 0x0200;
	constexpr const ENTRY_TYPE SMBIOS_ENTRY_BASEBOARD_SERIAL_NUMBER	= 0x0400;
	constexpr const ENTRY_TYPE SMBIOS_ENTRY_SYSTEM_UUID				= 0x0500;
	constexpr const ENTRY_TYPE SMBIOS_ENTRY_BASEBOARD_PLATFORM_ID	= 0x0F00;
	constexpr const ENTRY_TYPE SMBIOS_ENTRY_OS_PRELOAD_SUFFIX		= 0x1000;

	//constexpr ENTRY_KEY SMBIOS_ENTRY_KEY_WINDOWS_KEY				= { SMBIOS_NAMESPACE, SMBIOS_ENTRY_WINDOWS_KEY };
	constexpr ENTRY_KEY SMBIOS_ENTRY_KEY_OA3_KEY_ID					= { SMBIOS_NAMESPACE, SMBIOS_ENTRY_OA3_KEY_ID };
	constexpr ENTRY_KEY SMBIOS_ENTRY_KEY_MOTHERBOARD_NAME			= { SMBIOS_NAMESPACE, SMBIOS_ENTRY_MOTHERBOARD_NAME };
	constexpr ENTRY_KEY SMBIOS_ENTRY_KEY_MACHINE_TYPE_MODEL			= { SMBIOS_NAMESPACE, SMBIOS_ENTRY_MACHINE_TYPE_MODEL };
	constexpr ENTRY_KEY SMBIOS_ENTRY_KEY_BASEBOARD_SERIAL_NUMBER	= { SMBIOS_NAMESPACE, SMBIOS_ENTRY_BASEBOARD_SERIAL_NUMBER };
	constexpr ENTRY_KEY SMBIOS_ENTRY_KEY_SYSTEM_UUID				= { SMBIOS_NAMESPACE, SMBIOS_ENTRY_SYSTEM_UUID };
	constexpr ENTRY_KEY SMBIOS_ENTRY_KEY_BASEBOARD_PLATFORM_ID		= { SMBIOS_NAMESPACE, SMBIOS_ENTRY_BASEBOARD_PLATFORM_ID };
	constexpr ENTRY_KEY SMBIOS_ENTRY_KEY_OS_PRELOAD_SUFFIX			= { SMBIOS_NAMESPACE, SMBIOS_ENTRY_OS_PRELOAD_SUFFIX };

	// Logs DMI changes to the LDBG buffer.
	using AppendDmiChangeRecordFn = EFI_STATUS(__fastcall)(PVOID Context, PVOID Key, UINT8 Operation, UINT32 Size);

	// Signature for the function responsible for appending DMI change records:
	//								AppendDmiChangeRecord proc near
	//
	//								arg_0 = qword ptr  8
	//								arg_8 = qword ptr  10h
	//								arg_10 = qword ptr  18h
	//
	// 48 89 5C 24 08				mov[rsp + arg_0], rbx
	// 48 89 6C 24 10				mov[rsp + arg_8], rbp
	// 48 89 74 24 18				mov[rsp + arg_10], rsi
	// 57							push    rdi
	// 41 54						push    r12
	// 41 55						push    r13
	// 41 56						push    r14
	// 41 57						push    r15
	// 48 83 EC 20					sub     rsp, 20h
	constexpr PCSTR APPEND_DMI_CHANGE_RECORD_SIGNATURE = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC 20";

	// LenovoVariableDxe (installs LENOVO_VARIABLE_PROTOCOL) file GUID: C0C64D42-E3D9-4D20-865D-A3DB750F91D6.
	constexpr CEFI_GUID LENOVO_VARIABLE_DXE_GUID = { 0xC0C64D42, 0xE3D9, 0x4D20, { 0x86, 0x5D, 0xA3, 0xDB, 0x75, 0x0F, 0x91, 0xD6 } };
}