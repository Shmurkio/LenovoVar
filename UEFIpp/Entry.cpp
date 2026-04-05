#include "UEFIpp.hpp"
#include "CLI.hpp"

auto
EfiMain(
	MAYBE_UNUSED IN PCSTR* ArgV,
	MAYBE_UNUSED IN UINT64 ArgC
) -> EFI_STATUS
{
	return CLI::Run(ArgV, ArgC);
}