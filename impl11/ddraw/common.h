// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

#include "targetver.h"

extern bool _IsXwaExe;

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <ddraw.h>
#include <d3d.h>

#include <dxgi.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>

#include <comdef.h>

#include <intrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#include <string>
#include <sstream>

#include "ComPtr.h"
#include "logger.h"
#include "utils.h"
#include "config.h"
