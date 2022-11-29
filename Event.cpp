#include "Event.h"

EventBit EventBitFromEvent(Event event) {
	EventBit flag = EventBit::NONE;
	switch (event)
	{
	case Invalid:
		break;
	case EV_ONCREATE:
		break;
	case DEF_VALUES:
		break;
	case EV_ONKEY_DOWN:
		flag = flag | EventBit::HAVE_KEYBOARD_EVENT;
		flag = flag | EventBit::HAVE_KEYBOARD_EVENT_DOWN;
		break;
	case EV_ONKEY_UP:
		flag = flag | EventBit::HAVE_KEYBOARD_EVENT;
		flag = flag | EventBit::HAVE_KEYBOARD_EVENT_UP;
		break;
	case EV_ONMOUSE_MOTION:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		break;
	case EV_ONMOUSE_WHEEL:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		break;
	case EV_ONMOUSE_DOWN:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		flag = flag | EventBit::HAVE_MOUSE_EVENT_DOWN;
		break;
	case EV_ONMOUSE_UP:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		flag = flag | EventBit::HAVE_MOUSE_EVENT_UP;
		break;
	case EV_ONCOLLISION:
		flag = flag | EventBit::HAVE_COLLISION;
		break;
	case EV_ONVIEW_ENTER:
		flag = flag | EventBit::HAVE_VIEWCHANGE;
		break;
	case EV_ONVIEW_LEAVE:
		flag = flag | EventBit::HAVE_VIEWCHANGE;
		break;
	case EV_CLICKED:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		flag = flag | EventBit::HAVE_MOUSE_EVENT_DOWN;
		flag = flag | EventBit::HAVE_MOUSE_EVENT_CLICK;
		break;
	case EV_TRIGGER:
		flag = flag | EventBit::HAVE_TRIGGER;
		break;
	case EV_STEP:
		break;
	case EV_DRAW:
		break;
	case END:
		break;
	default:
		break;
	}
	return flag;
}