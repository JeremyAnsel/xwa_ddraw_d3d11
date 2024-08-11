// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "config.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

Config g_config;

Config::Config()
{
	this->DDrawType = "Golden";

	ifstream file("DDrawType.cfg");

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

			if (name == "DDrawType")
			{
				this->DDrawType = value;
			}
		}
	}

	this->DDrawTypeDllPath = "DDraw_" + this->DDrawType + ".dll";

	if (!ifstream(this->DDrawTypeDllPath))
	{
		MessageBox(nullptr, (this->DDrawTypeDllPath + " was not found.").c_str(), "X-Wing Alliance DDraw", MB_ICONWARNING);
		exit(0);
	}
}
