#pragma once
#ifdef _DEBUG

#include <iostream>

#include "windows.h"
#define _CRTDBG_MAP_ALLOC //to get more details
#include <stdlib.h>  
#include <crtdbg.h>   //for malloc and free
class Memory
{
private:
    _CrtMemState sOld;
    _CrtMemState sNew;
    _CrtMemState sDiff;
public:
    void StartSnapshot();
    void EndSnapshoot();
};


#endif // _DEBUG