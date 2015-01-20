// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

class Config
{
public:
	Config();

	bool AspectRatioPreserved;
	bool MultisamplingAntialiasingEnabled;
	bool AnisotropicFilteringEnabled;
	bool WireframeFillMode;

	float Concourse3DScale;
};

extern Config g_config;
