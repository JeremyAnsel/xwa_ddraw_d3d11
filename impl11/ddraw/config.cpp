// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "config.h"
#include "common.h"

#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

Config g_config;

Config::Config()
{
	this->AspectRatioPreserved = true;
	this->MultisamplingAntialiasingEnabled = false;
	this->AnisotropicFilteringEnabled = true;
	this->VSyncEnabled = true;
	this->WireframeFillMode = false;

	this->Concourse3DScale = 0.6f;

	this->ProcessAffinityCore = 2;

	this->EnhanceLasers = false;
	this->EnhanceIllumination = false;
	this->EnhanceEngineGlow = false;

	ifstream file("ddraw.cfg");

	if (file.is_open())
	{
		string line;

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

			string name = line.substr(0, pos);
			name.erase(remove_if(name.begin(), name.end(), std::isspace), name.end());

			string value = line.substr(pos + 1);
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
			else if (name == "EnableVSync")
			{
				this->VSyncEnabled = stoi(value) != 0;
			}
			else if (name == "FillWireframe")
			{
				this->WireframeFillMode = stoi(value) != 0;
			}
			else if (name == "Concourse3DScale")
			{
				this->Concourse3DScale = stof(value);
			}
			else if (name == "ProcessAffinityCore")
			{
				this->ProcessAffinityCore = stoi(value);
			}
			else if (name == "EnhanceLasers")
			{
				this->EnhanceLasers = (bool)stoi(value);
			}
			else if (name == "EnhanceIlluminationTextures")
			{
				this->EnhanceIllumination = (bool)stoi(value);
			}
			else if (name == "EnhanceEngineGlow")
			{
				this->EnhanceEngineGlow = (bool)stoi(value);
			}
		}
	}

	if (this->ProcessAffinityCore > 0)
	{
		HANDLE process = GetCurrentProcess();

		DWORD_PTR processAffinityMask;
		DWORD_PTR systemAffinityMask;

		if (GetProcessAffinityMask(process, &processAffinityMask, &systemAffinityMask))
		{
			int core = this->ProcessAffinityCore;
			DWORD_PTR mask = 0x01;

			for (int bit = 0, currentCore = 1; bit < 64; bit++)
			{
				if (mask & processAffinityMask)
				{
					if (currentCore != core)
					{
						processAffinityMask &= ~mask;
					}
	
					currentCore++;
				}

				mask = mask << 1;
			}

			SetProcessAffinityMask(process, processAffinityMask);
		}
	}

	DisableProcessWindowsGhosting();
}
