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
	int GenerateMipMaps;
	bool WireframeFillMode;
	int ScalingType;

	int Fullscreen;
	int Width;
	int Height;
	int JoystickEmul;
	float MouseSensitivity;
	float KbdSensitivity;
	bool XWAMode;

	float Concourse3DScale;

	int PresentSleepTime;
};

extern Config g_config;
