#pragma once

#include <string>
#include "enumstring.h"

ENUM_WITH_STRING_CONVERSION(
    Event,
    (EvOnCreate)
    (EvOnDestroy)
    (DEF_VALUES)

    (EvOnkeyDown)
    (EvOnkeyUp)

    (EvOnMouseMotion)
    (EvOnMouseWheel)
    (EvOnMouseDown)
    (EvOnMouseUp)

    (EvOnCollision)

    (EvOnViewEnter)
    (EvOnViewLeave)

    (EvClicked)
    (EvTrigger)

    (EvStep)
    (EvDraw)
)

#define E_TYPE std::uint16_t
enum class event_bit : E_TYPE {

    NONE =                      0,
    HAVE_MOUSE_EVENT =          1 << 0,
    HAVE_MOUSE_EVENT_CLICK =    1 << 1,
    HAVE_MOUSE_EVENT_DOWN =     1 << 2,
    HAVE_MOUSE_EVENT_UP =       1 << 3,
    HAVE_MOUSE_EVENT_SCROLL =   1 << 4,
    HAVE_MOUSE_EVENT_MOTION =   1 << 5,

    HAVE_KEYBOARD_EVENT =       1 << 6,
    HAVE_KEYBOARD_EVENT_UP =    1 << 7,
    HAVE_KEYBOARD_EVENT_DOWN =  1 << 8,

    HAVE_COLLISION =            1 << 9,
    HAVE_TRIGGER =              1 << 10,
    HAVE_VIEW_CHANGE =          1 << 11,
    HAVE_ON_DESTROY =           1 << 12,
    HAVE_CONTROLLER_INPUT =     1 << 13,
    UNUSED_2 =                  1 << 14,
    UNUSED_3 =                  1 << 15,

};
event_bit EventBitFromEvent(Event);
inline constexpr event_bit operator|(event_bit a, event_bit b) {
    return static_cast<event_bit>(static_cast<E_TYPE>(a) | static_cast<E_TYPE>(b));
}

inline constexpr event_bit operator&(event_bit a, event_bit b) {
    return static_cast<event_bit>(static_cast<E_TYPE>(a) & static_cast<E_TYPE>(b));

}
// if condition is in flag
#define EVENT_BIT_TEST(condition, flag)  (flag & condition) == condition
