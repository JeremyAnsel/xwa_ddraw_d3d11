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

	this->ProcessAffinityCore = 0;

	this->D3dHookExists = false;

	this->TextFontFamily = L"Verdana";
	this->TextWidthDelta = 0;

	this->Text2DRendererEnabled = true;
	this->Radar2DRendererEnabled = true;

	if (ifstream("Hook_D3d.dll"))
	{
		this->D3dHookExists = true;
	}

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
			value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
			value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

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
			else if (name == "TextFontFamily")
			{
				this->TextFontFamily = string_towstring(value);
			}
			else if (name == "TextWidthDelta")
			{
				this->TextWidthDelta = stoi(value);
			}
			else if (name == "Text2DRendererEnabled")
			{
				this->Text2DRendererEnabled = stoi(value) != 0;
			}
			else if (name == "Radar2DRendererEnabled")
			{
				this->Radar2DRendererEnabled = stoi(value) != 0;
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
