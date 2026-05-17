#pragma once
#include <cassert>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "BytesOperator.h"
#include "GameState.h"

namespace Serdes {

using namespace GameState;

std::vector< uint8_t > writeSnapshot( const Snapshot & snapshot );
Snapshot readSnapshot( BytesReader & reader );

} // namespace Serdes
