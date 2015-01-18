#include "lookup.h"

namespace bb { namespace search {

	void stop_lookup ();

	bb::search::ProgramLookup * g_lookup = nullptr;

	ProgramLookup & getLookup() { return *g_lookup; }

	void startLookup (tstring const & path)
	{
		if (!g_lookup)
		{
			Config cfg;
			if (!readRC(path + TEXT("search.rc"), cfg))
			{
				defaultConfig(cfg);
			}

			g_lookup = new bb::search::ProgramLookup(path, cfg);
		}
	}

	void stopLookup ()
	{
		if (g_lookup)
		{
      //@NOTE: users says that writing rc on quit is a bad idea.. theyr probably right
			//tstring tmp = g_lookup->m_path;
			//tmp += TEXT("search.rc");
			//writeRC(tmp, g_lookup->m_index.m_cfg);
			g_lookup->Stop();
			delete g_lookup;
			g_lookup = nullptr;
		}
	}

}}
