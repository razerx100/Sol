#ifndef SOL_THROW_MACROS_HPP_
#define SOL_THROW_MACROS_HPP_
#include <Exception.hpp>

#define SOL_GENERIC_THROW(errorMsg) throw GenericException(__LINE__, __FILE__, errorMsg)

#endif
