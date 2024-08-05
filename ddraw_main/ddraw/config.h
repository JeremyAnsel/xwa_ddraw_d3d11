// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

#include <string>

class Config
{
public:
	Config();

	std::string DDrawType;
	std::string DDrawTypeDllPath;
};

extern Config g_config;
