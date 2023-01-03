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
    _CrtMemState _s_old = {};
    _CrtMemState _s_new = {};
    _CrtMemState _s_diff = {};
public:
    void StartSnapshot();
    void EndSnapshoot();
};


#endif // _DEBUG