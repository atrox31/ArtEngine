#pragma once
/*
#include "Core.h"
#include "Rect.h"
class Core;
class Instance
{
public:
	Instance();
	virtual ~Instance();
	Uint64 GetId() { return _id; };
	void Delete() { Alive = false; };

	void RegisterEvent(Core::Event ev);
	void UnRegisterEvent(Core::Event ev);

	bool Alive;
	std::string Tag;
	bool InView;
	bool E_MaskClicked = false;
	Rect Mask;

private:
	//friend Art::Core;
	std::vector< Core::Event > _events_registered;
	Uint64 _id;
	static Uint64 _cid;
};
*/