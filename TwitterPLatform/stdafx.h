// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <sstream>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif


#include "amp.h"
#include "stdafx.cpp"
#include <filesystem>
#include <array>
#include <intrin.h>
#include <array>
#include <ppl.h>
#include <algorithm>
#include <windows.h>
#include <algorithm>

namespace fs = std::experimental::filesystem;
using namespace concurrency;

typedef long long int64;
typedef unsigned long long uint64;