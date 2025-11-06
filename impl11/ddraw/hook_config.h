#pragma once

#include <string>
#include <vector>

std::string Trim(const std::string& str);
std::string GetStringWithoutExtension(const std::string& str);
std::vector<std::string> GetFileLines(const std::string& path, const std::string& section = std::string());
std::string GetFileKeyValue(const std::vector<std::string>& lines, const std::string& key, const std::string& defaultValue = "");
int GetFileKeyValueInt(const std::vector<std::string>& lines, const std::string& key, int defaultValue = 0);
float GetFileKeyValueFloat(const std::vector<std::string>& lines, const std::string& key, float defaultValue = 0.0f);
std::vector<std::string> Tokennize(const std::string& str);
std::vector<std::vector<std::string>> GetFileListValues(const std::vector<std::string>& lines);
std::vector<int> GetFileListIntValues(const std::vector<std::string>& lines);
std::vector<unsigned short> GetFileListUnsignedShortValues(const std::vector<std::string>& lines);
