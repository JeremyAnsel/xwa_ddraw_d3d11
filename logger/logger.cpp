#include "logger.h"

#include <fstream>
#include <ctime>

clock_t g_startTime = clock();

std::ofstream g_file;

void InitLogFile()
{
	if (g_file.is_open())
	{
		return;
	}

	std::ofstream file;
	file.open("logger.txt");

	if (file.is_open())
	{
		file.close();
	}

	g_file.open("logger.txt", std::ios::app);
}

void LogText(std::string text)
{
	InitLogFile();

	int currentTime = clock() - g_startTime;

	g_file << currentTime << '\t' << text.c_str() << std::endl;
}

std::ofstream g_obj;
int g_objFrameIndex;
int g_objObjectIndex;
int g_objVertexBaseIndex;
int g_objVertexIndex;

void InitObjFile()
{
	if (g_obj.is_open())
	{
		return;
	}

	std::ofstream file;
	file.open("logger.obj");

	if (file.is_open())
	{
		file.close();
	}

	g_obj.open("logger.obj", std::ios::app);

	g_obj.setf(std::ios::fixed, std::ios::floatfield);

	g_objFrameIndex = 0;
	g_objObjectIndex = 0;
	g_objVertexBaseIndex = 1;
	g_objVertexIndex = 1;
}

void LogObjFrame()
{
	g_objFrameIndex++;
	g_obj << "o \"frame" << g_objFrameIndex << "\"" << std::endl;
}

void LogObjObject(int vertLength, int instLength, int bufLength)
{
	InitObjFile();

	//if (g_objFrameIndex == 0)
	//{
	//	return;
	//}

	g_objObjectIndex++;
	g_objVertexBaseIndex = g_objVertexIndex;

	g_obj << "g \"obj"  << g_objObjectIndex << "\"" << std::endl;
	g_obj << "#buf " << vertLength << " " << instLength << " " << bufLength << std::endl;
}

void LogObjVertex(float x, float y, float z)
{
	InitObjFile();

	g_obj << "v " << x << " " << y << " " << z << std::endl;
	g_objVertexIndex++;
}

void LogObjTriangle(int v1, int v2, int v3)
{
	InitObjFile();

	v1 += g_objVertexBaseIndex;
	v2 += g_objVertexBaseIndex;
	v3 += g_objVertexBaseIndex;

	g_obj << "f " << v1 << " " << v2 << " " << v3 << std::endl;
}

void LogObjTexture(int handle)
{
	InitObjFile();

	g_obj << "usemtl " << handle << std::endl;
}
