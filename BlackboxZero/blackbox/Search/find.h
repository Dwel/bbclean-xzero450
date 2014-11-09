#include <algorithm>
#include <functional>
#include <vector>

#include <windows.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include "unicode.h"
#include "history.h"
#include "search.h"

namespace bb { namespace search {

// Search all configured m_locations for all includes except in excluded directories
/*struct SearchString
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
};*/

}}

