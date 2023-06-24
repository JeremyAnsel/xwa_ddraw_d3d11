// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

#include <string>

class Config
{
public:
	Config();

	bool AspectRatioPreserved;
	bool MultisamplingAntialiasingEnabled;
	bool AnisotropicFilteringEnabled;
	bool VSyncEnabled;
	bool WireframeFillMode;

	float Concourse3DScale;

	int ProcessAffinityCore;

	bool D3dHookExists;

	std::wstring TextFontFamily;
	int TextWidthDelta;

	bool Text2DRendererEnabled;
	bool Radar2DRendererEnabled;

	bool D3dRendererHookEnabled;
	bool D3dRendererHookShowNormals;

	bool HDConcourseEnabled;

	float ProjectionParameterA;
	float ProjectionParameterB;
	float ProjectionParameterC;

	std::string ScreenshotsDirectory;
};

extern Config g_config;
