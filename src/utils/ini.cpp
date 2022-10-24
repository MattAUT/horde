#include "ini.h"

#include "logmanager.h"

#include <fstream>
#include <algorithm>
#include <cstdio>
#include <cassert>

namespace INI
{
	bool 
	Settings::IsHeader(std::string line)
	{
		return line[0] = '[' && line[line.size() - 1] == ']';
	}

	KeyValPair 
	Settings::GetKeyValue(std::string line)
	{
		unsigned int equalsIndex = 0;
		line.erase(std::remove_if(line.begin(), line.end(), isspace));

		for (std::string::iterator c = line.begin(); c != line.end(); ++c)
		{
			if (*c == '=')
			{
				break;
			}

			++equalsIndex;
		}

		return KeyValPair(line.substr(0, equalsIndex), line.substr(equalsIndex + 1, line.size() - 1));
	}

	std::string 
	Settings::FetchValue(const char* sectionHeader, const char* key)
	{
		if (!m_bLoaded)
		{
			LogManager::GetInstance().Log("Attempted to fetch from unloaded .ini!");
			assert(0);
		}

		for (IniMap::iterator section = m_map.begin(); section != m_map.end(); ++section)
		{
			if (section->first == sectionHeader)
			{
				for
					(
						VectorKeyValPairs::iterator keyValuePair = section->second.begin();
						keyValuePair != section->second.end();
						++keyValuePair)
				{
					if (keyValuePair->first == key)
					{
						return keyValuePair->second;
					}
				}
			}
		}

		char message[256];
		sprintf_s(message, "Could not find key %s in section %s while fetching from INI", key, sectionHeader);
		LogManager::GetInstance().Log(message);
		assert(0);
		return "";
	}

	Settings::Settings()
		: m_bLoaded(false)
	{
		
	}

	Settings::~Settings()
	{

	}

	bool Settings::LoadFile(const char* fileName)
	{
		std::ifstream file(fileName);
		std::string line;
		VectorKeyValPairs keyValuePairs;
		std::string header = "";

		while (std::getline(file, line))
		{
			
			if (line.size() < 2 || line[0] == ';')
			{
				continue;
			}

			if (IsHeader(line))
			{
				if (!(header == ""))
				{
					m_map.insert(std::pair<std::string, VectorKeyValPairs>(header, keyValuePairs));
				}

				header = line.substr(1, line.size() - 2);
				keyValuePairs.clear();
				continue;
			}
			keyValuePairs.push_back(GetKeyValue(line));
		}

		m_map.insert(std::pair<std::string, VectorKeyValPairs>(header, keyValuePairs));
		file.close();
		m_bLoaded = true;
		return true;
	}

	bool Settings::Unload()
	{
		m_map.clear();
		m_bLoaded = false;
		return true;
	}

	std::string Settings::GetString(const char* sectionHeader, const char* key)
	{
		return FetchValue(sectionHeader, key);
	}

	int Settings::GetInt(const char* sectionHeader, const char* key)
	{
		return atoi(FetchValue(sectionHeader, key).c_str());
	}

	float Settings::GetFloat(const char* sectionHeader, const char* key)
	{
		return static_cast<float>(atof(FetchValue(sectionHeader, key).c_str()));
	}

	bool Settings::GetBool(const char* sectionHeader, const char* key)
	{
		return FetchValue(sectionHeader, key) == "true";
	}
}