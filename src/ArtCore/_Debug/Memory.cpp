#ifdef _DEBUG
#include "Memory.h"

void Memory::StartSnapshot()
{
	_CrtMemCheckpoint(&_s_old);
}

void Memory::EndSnapshoot()
{
    _CrtMemCheckpoint(&_s_new); //take a snapshot 
    if (_CrtMemDifference(&_s_diff, &_s_old, &_s_new)) // if there is a difference
    {
        OutputDebugString(L"-----------_CrtMemDumpStatistics ---------");
        _CrtMemDumpStatistics(&_s_diff);
        OutputDebugString(L"-----------_CrtMemDumpAllObjectsSince ---------");
        _CrtMemDumpAllObjectsSince(&_s_old);
        OutputDebugString(L"-----------_CrtDumpMemoryLeaks ---------");
        _CrtDumpMemoryLeaks();
    }
}
#endif