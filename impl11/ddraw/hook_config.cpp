#include "hook_config.h"
#include <fstream>

std::string Trim(const std::string& str)
{
	const char* ws = " \t\n\r\f\v";
	std::string s = str;

	s.erase(str.find_last_not_of(ws) + 1);
	s.erase(0, s.find_first_not_of(ws));

	return s;
}

std::string GetStringWithoutExtension(const std::string& str)
{
	auto b = str.find_last_of('.');

	return str.substr(0, b);
}

std::vector<std::string> GetFileLines(const std::string& path, const std::string& section)
{
	std::vector<std::string> values;

	std::ifstream file(path);

	if (file)
	{
		std::string line;
		bool readSection = section.empty();

		while (std::getline(file, line))
		{
			line = Trim(line);

			if (!line.length())
			{
				continue;
			}

			if (line[0] == '#' || line[0] == ';' || (line[0] == '/' && line[1] == '/'))
			{
				continue;
			}

			if (line[0] == '[' && line[line.length() - 1] == ']')
			{
				std::string name = line.substr(1, line.length() - 2);

				if (_stricmp(name.c_str(), section.c_str()) == 0)
				{
					readSection = true;
				}
				else
				{
					readSection = false;
				}
			}
			else
			{
				if (readSection)
				{
					values.push_back(line);
				}
			}
		}
	}

	return values;
}

std::string GetFileKeyValue(const std::vector<std::string>& lines, const std::string& key)
{
	for (const auto& line : lines)
	{
		int pos = line.find("=");

		if (pos == -1)
		{
			continue;
		}

		std::string name = Trim(line.substr(0, pos));

		if (!name.length())
		{
			continue;
		}

		if (_stricmp(name.c_str(), key.c_str()) == 0)
		{
			std::string value = Trim(line.substr(pos + 1));
			return value;
		}
	}

	return std::string();
}

int GetFileKeyValueInt(const std::vector<std::string>& lines, const std::string& key, int defaultValue)
{
	std::string value = GetFileKeyValue(lines, key);

	if (value.empty())
	{
		return defaultValue;
	}

	return std::stoi(value, 0, 0);
}

std::vector<std::string> Tokennize(const std::string& str)
{
	const char* delimiters = ",;";
	std::vector<std::string> tokens;

	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		std::string value = Trim(str.substr(lastPos, pos - lastPos));
		tokens.push_back(value);

		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

std::vector<std::vector<std::string>> GetFileListValues(const std::vector<std::string>& lines)
{
	std::vector<std::vector<std::string>> values;

	for (const std::string& line : lines)
	{
		values.push_back(Tokennize(line));
	}

	return values;
}

std::vector<int> GetFileListIntValues(const std::vector<std::string>& lines)
{
	std::vector<int> values;

	for (const std::string& line : lines)
	{
		int value = std::stoi(Trim(line));
		values.push_back(value);
	}

	return values;
}

std::vector<unsigned short> GetFileListUnsignedShortValues(const std::vector<std::string>& lines)
{
	std::vector<unsigned short> values;

	for (const std::string& line : lines)
	{
		unsigned short value = (unsigned short)std::stoi(Trim(line));
		values.push_back(value);
	}

	return values;
}
