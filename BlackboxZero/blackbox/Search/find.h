#include <algorithm>
#include <functional>
#include <vector>

#include <windows.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include "unicode.h"
#include "history.h"
#include "search.h"

struct Config
{
	Config () : m_locations(), m_history(24) { }
	std::vector<SearchLocationInfo> m_locations;
	History<tstring> m_history;
};

inline void defaultConfig (Config & cfg)
{
	cfg.m_locations.push_back(SearchLocationInfo(TEXT("C:\\Program Files\\"), TEXT("*.exe;*.com;*.bat;*.cmd"), TEXT(""), true));
	cfg.m_locations.push_back(SearchLocationInfo(TEXT("C:\\Program Files (x86)\\"), TEXT("*.exe;*.com;*.bat;*.cmd"), TEXT("C:\\Program Files (x86)\\Microsoft Visual Studio 12.0"), true));
	cfg.m_locations.push_back(SearchLocationInfo(TEXT("C:\\Windows\\"), TEXT("*.exe;*.com;*.bat;*.cmd"),
					TEXT("C:\\Windows\\winsxs\\;C:\\Windows\\Installer\\;C:\\Windows\\Microsoft.NET\\;C:\\Windows\\SoftwareDistribution\\"), true));
	// blackbox dir :)
	// c:\\bin
}

struct Index { };

// search all locations for all includes except includes
// TODO: cached db
struct SearchString
{
	tstring m_text;
	Config m_config;
	std::vector<tstring> m_result;

	SearchString (Config const & cfg)
		: m_config(cfg)
	{
		m_text.reserve(64);
		m_result.reserve(32);
	}
	
	void Execute ()
	{
		m_result.clear();
		for (SearchLocationInfo const & info : m_config.m_locations)
		{
			SearchDirectory(info, m_text, m_result);	
		}
	}

	void Accept (tstring const & str)
	{
		m_config.m_history.insert(str);
	}
};

/*int find_exe ()
{
	Config cfg;
	defaultConfig(cfg);

	//const tstring myFile2 = L"devenv.exe";
	const tstring myFile2 = L".exe";
	SearchString find(cfg);
	find.m_text = myFile2;
	find.Execute();
	std::wcout << "results:" << std::endl;
	if (find.m_result.size() > 0)
		find.Accept(find.m_result[0]);
	for (auto const & p : find.m_result)
		std::wcout << p << std::endl;
}*/
