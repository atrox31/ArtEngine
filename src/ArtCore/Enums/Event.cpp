#include "Event.h"

event_bit EventBitFromEvent(Event event) {
	event_bit flag = event_bit::NONE;
	switch (event)
	{
	case EventInvalid:
		break;
	case EvOnCreate:
		break;
	case DEF_VALUES:
		break;
	case EvOnkeyDown:
		flag = flag | event_bit::HAVE_KEYBOARD_EVENT;
		flag = flag | event_bit::HAVE_KEYBOARD_EVENT_DOWN;
		break;
	case EvOnkeyUp:
		flag = flag | event_bit::HAVE_KEYBOARD_EVENT;
		flag = flag | event_bit::HAVE_KEYBOARD_EVENT_UP;
		break;
	case EvOnMouseMotion:
		flag = flag | event_bit::HAVE_MOUSE_EVENT;
		break;
	case EvOnMouseWheel:
		flag = flag | event_bit::HAVE_MOUSE_EVENT;
		break;
	case EvOnMouseUp:
		flag = flag | event_bit::HAVE_MOUSE_EVENT;
		flag = flag | event_bit::HAVE_MOUSE_EVENT_UP;
		break;
	case EvOnMouseDown:
		flag = flag | event_bit::HAVE_MOUSE_EVENT;
		flag = flag | event_bit::HAVE_MOUSE_EVENT_UP;
		break;
	case EvOnCollision:
		flag = flag | event_bit::HAVE_COLLISION;
		break;
	case EvOnViewEnter:
		flag = flag | event_bit::HAVE_VIEW_CHANGE;
		break;
	case EvOnViewLeave:
		flag = flag | event_bit::HAVE_VIEW_CHANGE;
		break;
	case EvClicked:
		flag = flag | event_bit::HAVE_MOUSE_EVENT;
		flag = flag | event_bit::HAVE_MOUSE_EVENT_DOWN;
		flag = flag | event_bit::HAVE_MOUSE_EVENT_CLICK;
		break;
	case EvTrigger:
		flag = flag | event_bit::HAVE_TRIGGER;
		break;
	case EvStep:
		break;
	case EvDraw:
		break;
	case EventEND:
		break;
	case EvOnDestroy:
		flag = flag | event_bit::HAVE_ON_DESTROY;
		break;
	default:
		break;
	}
	return flag;
}