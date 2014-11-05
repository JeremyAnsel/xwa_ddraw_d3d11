#pragma once

#include <string>

void LogText(std::string text);

void LogObjFrame();
void LogObjObject(int vertLength, int instLength, int bufLength);
void LogObjVertex(float x, float y, float z);
void LogObjTriangle(int a, int b, int c);
void LogObjTexture(int handle);
