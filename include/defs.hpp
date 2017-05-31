// Copyright 2017, Pavel Korozevtsev.

#pragma once

namespace pkr {

#ifdef DEBUG
#define DO_DEBUG(expr) expr
#else
#define DO_DEBUG(expr)
#endif

using Port = uint16_t;

}  // namespace pkr
