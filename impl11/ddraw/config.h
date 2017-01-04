// Copyright (c) 2014 J�r�my Ansel
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
	bool isTIE;
	bool isXWing;

	float Concourse3DScale;

	int PresentSleepTime;
};

extern Config g_config;
