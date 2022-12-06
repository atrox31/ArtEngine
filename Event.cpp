#include "Event.h"

EventBit EventBitFromEvent(Event event) {
	EventBit flag = EventBit::NONE;
	switch (event)
	{
	case Invalid:
		break;
	case EvOncreate:
		break;
	case DEF_VALUES:
		break;
	case EvOnkeyDown:
		flag = flag | EventBit::HAVE_KEYBOARD_EVENT;
		flag = flag | EventBit::HAVE_KEYBOARD_EVENT_DOWN;
		break;
	case EvOnkeyUp:
		flag = flag | EventBit::HAVE_KEYBOARD_EVENT;
		flag = flag | EventBit::HAVE_KEYBOARD_EVENT_UP;
		break;
	case EvOnmouseMotion:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		break;
	case EvOnMouseWheel:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		break;
	case EvOnMouseUp:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		flag = flag | EventBit::HAVE_MOUSE_EVENT_UP;
		break;
	case EvOnMouseDown:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		flag = flag | EventBit::HAVE_MOUSE_EVENT_UP;
		break;
	case EvOnCollision:
		flag = flag | EventBit::HAVE_COLLISION;
		break;
	case EvOnViewEnter:
		flag = flag | EventBit::HAVE_VIEWCHANGE;
		break;
	case EvOnViewLeave:
		flag = flag | EventBit::HAVE_VIEWCHANGE;
		break;
	case EvClicked:
		flag = flag | EventBit::HAVE_MOUSE_EVENT;
		flag = flag | EventBit::HAVE_MOUSE_EVENT_DOWN;
		flag = flag | EventBit::HAVE_MOUSE_EVENT_CLICK;
		break;
	case EvTrigger:
		flag = flag | EventBit::HAVE_TRIGGER;
		break;
	case EvStep:
		break;
	case EvDraw:
		break;
	case END:
		break;
	default:
		break;
	}
	return flag;
}