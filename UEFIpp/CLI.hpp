#pragma once

#include "UEFIpp.hpp"

namespace CLI
{
	auto Run(PCSTR* ArgV, UINT64 ArgC) -> EFI_STATUS;
}