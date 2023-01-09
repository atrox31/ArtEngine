#pragma once
#include "boost/preprocessor.hpp"
#define X_ENUM_WITH_STRING_CONVERSION_TO_STRING_CASE(r, data, elem)   \
    case elem : return BOOST_PP_STRINGIZE(elem);                               

#define X_ENUM_WITH_STRING_CONVERSION_FROM_STRING_CASE(r, data, elem) \
    if (str == BOOST_PP_STRINGIZE(elem)) return elem;                          
/*
 * The following macros allow you to define an enum and get `toString`
 * and `fromString` functions generated automatically.
 * If you use
 * @code
 * ENUM_WITH_STRING_CONVERSION(Test, (One)(Two)(Three))
 * @endcode
 * your enum will be called `Test` and the conversion functions will be
 * called `Test_toString` and `Test_fromString`
 *
 * https://gist.github.com/ibab/54162246bc79279c43c7
 */
#define ENUM_WITH_STRING_CONVERSION(name, enumerators)               \
    enum name {                                                      \
        name##Invalid,                                               \
        BOOST_PP_SEQ_ENUM(enumerators),                              \
        name##END                                                          \
    };                                                               \
                                                                     \
    inline static name name##_fromString(const std::string & str)           \
    {                                                                \
        BOOST_PP_SEQ_FOR_EACH(                                       \
            X_ENUM_WITH_STRING_CONVERSION_FROM_STRING_CASE,          \
            name,                                                    \
            enumerators                                              \
        )                                                            \
        return name##Invalid;                                              \
    }                                                                \
                                                                     \
    inline static std::string name##_toString(name v)                       \
    {                                                                \
        switch (v)                                                   \
        {                                                            \
            BOOST_PP_SEQ_FOR_EACH(                                   \
                X_ENUM_WITH_STRING_CONVERSION_TO_STRING_CASE,        \
                name,                                                \
                enumerators                                          \
            )                                                        \
            default:                                                 \
                return "Invalid";                                    \
        }                                                            \
    }                                                                \
