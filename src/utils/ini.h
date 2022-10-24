#ifndef __INI_H_
#define __INI_H_

#include <string>
#include <vector>
#include <map>

typedef std::pair<std::string, std::string> KeyValPair;
typedef std::vector<KeyValPair> VectorKeyValPairs;
typedef std::map<std::string, VectorKeyValPairs> IniMap;

namespace INI
{
	class Settings
	{
	public:
		Settings();
		~Settings();

		bool LoadFile(const char* fileName);
		bool Unload();
		std::string GetString(const char* sectionHeader, const char* key);
		int GetInt(const char* sectionHeader, const char* key);
		float GetFloat(const char* sectionHeader, const char* key);
		bool GetBool(const char* sectionHeader, const char* key);

	protected:
		static bool IsHeader(std::string line);
		static KeyValPair GetKeyValue(std::string line);

		std::string FetchValue(const char* sectionHeader, const char* key);
	private:

	public:
	protected:
		IniMap m_map;
		bool m_bLoaded;
	private:
	};
}

#endif // __INI_H_