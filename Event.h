#pragma once

#include <boost/preprocessor.hpp>                                                
#include <string>

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

#define X_ENUM_WITH_STRING_CONVERSION_TOSTRING_CASE(r, data, elem)   \
    case elem : return BOOST_PP_STRINGIZE(elem);                               

#define X_ENUM_WITH_STRING_CONVERSION_FROMSTRING_CASE(r, data, elem) \
    if (str == BOOST_PP_STRINGIZE(elem)) return elem;                          

#define ENUM_WITH_STRING_CONVERSION(name, enumerators)               \
    enum name {                                                      \
        Invalid,                                                     \
        BOOST_PP_SEQ_ENUM(enumerators),                               \
        END                                                             \
    };                                                               \
                                                                     \
    inline name name##_fromString(const std::string & str)           \
    {                                                                \
        BOOST_PP_SEQ_FOR_EACH(                                       \
            X_ENUM_WITH_STRING_CONVERSION_FROMSTRING_CASE,           \
            name,                                                    \
            enumerators                                              \
        )                                                            \
        return Invalid;                                              \
    }                                                                \
                                                                     \
    inline std::string name##_toString(name v)                       \
    {                                                                \
        switch (v)                                                   \
        {                                                            \
            BOOST_PP_SEQ_FOR_EACH(                                   \
                X_ENUM_WITH_STRING_CONVERSION_TOSTRING_CASE,         \
                name,                                                \
                enumerators                                          \
            )                                                        \
            default:                                                 \
                return "Invalid";                                    \
        }                                                            \
    }                                                                \


ENUM_WITH_STRING_CONVERSION(
    Event,
    (EV_ONCREATE)
    (EV_ONDESTROY)
    (DEF_VALUES)

    (EV_ONKEY_DOWN)
    (EV_ONKEY_UP)

    (EV_ONMOUSE_MOTION)
    (EV_ONMOUSE_WHEEL)
    (EV_ONMOUSE_DOWN)
    (EV_ONMOUSE_UP)

    (EV_ONCOLLISION)

    (EV_ONVIEW_ENTER)
    (EV_ONVIEW_LEAVE)

    (EV_CLICKED)
    (EV_TRIGGER)

    (EV_STEP)
    (EV_DRAW)
)

#define _e_type uint16_t

enum class EventBit : _e_type {

    NONE =                      0,
    HAVE_MOUSE_EVENT =          1 << 0,
    HAVE_MOUSE_EVENT_CLICK =    1 << 1,
    HAVE_MOUSE_EVENT_DOWN =     1 << 2,
    HAVE_MOUSE_EVENT_UP =       1 << 3,
    HAVE_MOUSE_EVENT_SCROLL =   1 << 4,
    HAVE_MOUSE_EVENT_MONTION =  1 << 5,

    HAVE_KEYBOARD_EVENT =       1 << 6,
    HAVE_KEYBOARD_EVENT_UP =    1 << 7,
    HAVE_KEYBOARD_EVENT_DOWN =  1 << 8,

    HAVE_COLLISION =            1 << 9,
    HAVE_TRIGGER =              1 << 10,
    HAVE_VIEWCHANGE =           1 << 11,

};
EventBit EventBitFromEvent(Event);
inline constexpr EventBit operator|(EventBit a, EventBit b) {
    return static_cast<EventBit>(static_cast<char>(a) | static_cast<char>(b));
}

inline constexpr EventBit operator&(EventBit a, EventBit b) {
    return static_cast<EventBit>(static_cast<char>(a) & static_cast<char>(b));

}
// if condition is in flag
#define EventBitTest(condition, flag)  (flag & condition) == condition
