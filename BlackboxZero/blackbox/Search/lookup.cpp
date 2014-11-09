#include "lookup.h"

namespace bb { namespace search {

	void stop_lookup ();

	bb::search::ProgramLookup * g_lookup = nullptr;

	ProgramLookup & getLookup() { return *g_lookup; }

	void startLookup (tstring const & path)
	{
		if (!g_lookup)
			g_lookup = new bb::search::ProgramLookup(path);
	}

	void stopLookup ()
	{
		if (g_lookup)
		{
			g_lookup->Stop();
			delete g_lookup;
			g_lookup = nullptr;
		}
	}

}}
