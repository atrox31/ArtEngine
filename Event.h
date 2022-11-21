#pragma once
enum class Event {
	FIRST,
	EV_ONCREATE,

	EV_ONKEY_DOWN,
	EV_ONKEY_UP,

	EV_ONMOUSE_MOTION,
	EV_ONMOUSE_WHEEL,
	EV_ONMOUSE_DOWN,
	EV_ONMOUSE_UP,

	EV_ONCOLLISION,

	EV_ONVIEW_ENTER,
	EV_ONVIEW_LEAVE,

	EV_CLICKED,
	EV_TRIGGER,

	EV_STEP,
	EV_DRAW,
	LAST,
};
Event& operator ++ (Event& e)
{
	if (e == Event::LAST) {
		throw std::out_of_range("for E& operator ++ (E&)");
	}
	e = Event(static_cast<std::underlying_type<Event>::type>(e) + 1);
	return e;
}