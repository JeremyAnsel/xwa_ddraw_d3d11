// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

#ifdef _DEBUG
#define LOGGER 1
#else
#define LOGGER 0
#endif

#if LOGGER

#include <string>

void LogText(std::string text);

#endif // LOGGER
