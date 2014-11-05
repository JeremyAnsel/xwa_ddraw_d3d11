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
