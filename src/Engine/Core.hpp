#ifndef DEPENDENCIES_HPP
#define DEPENDENCIES_HPP

#include <cstdint>

using Entity = std::uint32_t;
using DeltaTime = double;

inline constexpr Entity MAX_ENTITIES = 1000;
inline constexpr Entity NULL_ENTITY = -1;

#endif