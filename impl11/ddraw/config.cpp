// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "config.h"
#include "common.h"

#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

Config g_config;

Config::Config()
{
	this->AspectRatioPreserved = true;
	this->MultisamplingAntialiasingEnabled = true;
	this->AnisotropicFilteringEnabled = true;
	this->WireframeFillMode = false;
	this->Fullscreen = false;
	this->XWAMode = true;

	this->Concourse3DScale = 0.6f;

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
			name.erase(remove_if(name.begin(), name.end(), std::isspace), name.end());

			std::string value = line.substr(pos + 1);
			value.erase(remove_if(value.begin(), value.end(), std::isspace), value.end());

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
				this->Fullscreen = stoi(value) != 0;
			}
			else if (name == "XWAMode")
			{
				this->XWAMode = stoi(value) != 0;
			}
			else if (name == "Concourse3DScale")
			{
				this->Concourse3DScale = stof(value);
			}
		}
	}
}
