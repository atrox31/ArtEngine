#ifdef _DEBUG
#include "Memory.h"

void Memory::StartSnapshot()
{
	_CrtMemCheckpoint(&sOld);
}

void Memory::EndSnapshoot()
{
    _CrtMemCheckpoint(&sNew); //take a snapshot 
    if (_CrtMemDifference(&sDiff, &sOld, &sNew)) // if there is a difference
    {
        OutputDebugString(L"-----------_CrtMemDumpStatistics ---------");
        _CrtMemDumpStatistics(&sDiff);
        OutputDebugString(L"-----------_CrtMemDumpAllObjectsSince ---------");
        _CrtMemDumpAllObjectsSince(&sOld);
        OutputDebugString(L"-----------_CrtDumpMemoryLeaks ---------");
        _CrtDumpMemoryLeaks();
    }
}
#endif