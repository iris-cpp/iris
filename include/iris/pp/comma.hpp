#ifndef IRIS_PREPROCESS_COMMA_HPP
#define IRIS_PREPROCESS_COMMA_HPP

#include <iris/pp/if.hpp>

#define IRIS_PP_COMMA() ,
#define IRIS_PP_COMMA_I_EMPTY()

#define IRIS_PP_COMMA_IF(cond) IRIS_PP_IF(cond, IRIS_PP_COMMA, IRIS_PP_COMMA_I_EMPTY)()

#endif
