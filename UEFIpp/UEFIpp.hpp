#pragma once

#include "Uefi/Uefi.hpp"

#include "Uefi/Memory/Memory.hpp"
#include "Uefi/Console/Console.hpp"
#include "Uefi/Serial/Serial.hpp"
#include "Uefi/Util/Util.hpp"
#include "Uefi/NVRAM/NVRAM.hpp"
#include "Uefi/Image/Image.hpp"
#include "Uefi/Detour/Detour.hpp"

#include <cstdint>

template<typename T, typename MemberT>
constexpr T*
ContainingRecord(
    IN MemberT* MemberPtr,
    IN MemberT T::* Member
)
{
    auto Offset = reinterpret_cast<std::uintptr_t>(&(reinterpret_cast<T const volatile*>(0)->*Member));
    return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(MemberPtr) - Offset);
}