// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "config.h"

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
	this->WireframeFillMode = false;

	this->Concourse3DScale = 0.6f;

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
			else if (name == "FillWireframe")
			{
				this->WireframeFillMode = stoi(value) != 0;
			}
			else if (name == "Concourse3DScale")
			{
				this->Concourse3DScale = stof(value);
			}
		}
	}
}
