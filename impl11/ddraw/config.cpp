// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "config.h"
#include "common.h"

#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

// Wrapper to avoid undefined behaviour due to
// sign-extending char to int when passing to isspace.
// That would be undefined behaviour and crashes with table
// based implamentations.
// In addition it avoids issues due to ambiguity between
// multiple isspace overloads when used in a template context.
static int isspace_wrapper(char c)
{
    return std::isspace(static_cast<unsigned char>(c));
}

Config g_config;

Config::Config()
{
	this->AspectRatioPreserved = true;
	this->MultisamplingAntialiasingEnabled = true;
	this->AnisotropicFilteringEnabled = true;
	this->GenerateMipMaps = true;
	this->WireframeFillMode = false;
	this->ScalingType = 0;
	this->Fullscreen = 0;
	this->XWAMode = true;
	int XWAModeInt = -1;
	int ProcessAffinity = -1;

	this->Concourse3DScale = 0.6f;

	this->PresentSleepTime = -1;

	// Try to always load config from executable path, not CWD
	char execPath[MAX_PATH] = "";
	HMODULE module = GetModuleHandle(NULL);
	if (module)
	{
		GetModuleFileNameA(module, execPath, sizeof(execPath));
		char *end = strrchr(execPath, '\\');
		if (end)
		{
			end[1] = 0;
		}
		else
		{
			execPath[0] = 0;
		}
	}

	std::ifstream file(std::string(execPath) + "ddraw.cfg");

	if (file.is_open())
	{
		std::string line;

		while (std::getline(file, line))
		{
			if (!line.length())
			{
				continue;
			}

			if (line[0] == '#' || line[0] == ';' || (line[0] == '/' && line[1] == '/'))
			{
				continue;
			}

			int pos = line.find("=");

			std::string name = line.substr(0, pos);
			name.erase(remove_if(name.begin(), name.end(), isspace_wrapper), name.end());

			std::string value = line.substr(pos + 1);
			value.erase(remove_if(value.begin(), value.end(), isspace_wrapper), value.end());

			if (!name.length() || !value.length())
			{
				continue;
			}

			if (name == "PreserveAspectRatio")
			{
				this->AspectRatioPreserved = stoi(value) != 0;
			}
			else if (name == "EnableMultisamplingAntialiasing")
			{
				this->MultisamplingAntialiasingEnabled = stoi(value) != 0;
			}
			else if (name == "EnableAnisotropicFiltering")
			{
				this->AnisotropicFilteringEnabled = stoi(value) != 0;
			}
			else if (name == "GenerateMipMaps")
			{
				this->GenerateMipMaps = stoi(value) != 0;
			}
			else if (name == "FillWireframe")
			{
				this->WireframeFillMode = stoi(value) != 0;
			}
			else if (name == "ScalingType")
			{
				this->ScalingType = stoi(value);
			}
			else if (name == "Fullscreen")
			{
				this->Fullscreen = stoi(value);
			}
			else if (name == "XWAMode")
			{
				XWAModeInt = stoi(value);
			}
			else if (name == "Concourse3DScale")
			{
				this->Concourse3DScale = stof(value);
			}
			else if (name == "ProcessAffinity")
			{
				ProcessAffinity = stoi(value);
			}
			else if (name == "PresentSleepTime")
			{
				this->PresentSleepTime = stoi(value);
			}
		}
	}
	if (XWAModeInt == -1)
	{
		char name[4096] = {};
		GetModuleFileNameA(NULL, name, sizeof(name));
		int len = strlen(name);
		this->XWAMode = len >= 17 && _stricmp(name + len - 17, "xwingalliance.exe") == 0;
	}
	else
	{
		this->XWAMode = XWAModeInt != 0;
	}
	if (ProcessAffinity != 0) {
		DWORD_PTR CurProcessAffinity, SystemAffinity;
		HANDLE mod = GetCurrentProcess();
		if (GetProcessAffinityMask(mod, &CurProcessAffinity, &SystemAffinity)) {
			if (ProcessAffinity > 0) {
				ProcessAffinity &= SystemAffinity;
			} else {
				ProcessAffinity = (SystemAffinity & 2) ? 2 : 1;
			}
		}
		SetProcessAffinityMask(mod, ProcessAffinity);
	}
}
