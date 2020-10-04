#ifndef CAPTAL_CONFIG_HPP_INCLUDED
#define CAPTAL_CONFIG_HPP_INCLUDED

#include <captal_foundation/base.hpp>
#include <captal_foundation/enum_operations.hpp>
#include <captal_foundation/version.hpp>

#define CAPTAL_API CAPTAL_FOUNDATION_API

namespace cpt
{

#ifdef CAPTAL_DEBUG
inline constexpr bool debug_enabled{true};
#else
inline constexpr bool debug_enabled{false};
#endif

}

#endif
