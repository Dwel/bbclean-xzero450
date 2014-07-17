#pragma once

namespace trace {

	enum E_TraceLevel
	{
		e_Fatal = 0,
		e_Error,
		e_Warning,
		e_Info,
		e_Debug,
		e_Verbose,

		e_max_trace_level	/// last item of enum
	};

	typedef E_TraceLevel level_t;
}

struct LvlDictPair {
    char const * first;
    trace::level_t second;
};

inline size_t getLevelDictionnary (LvlDictPair const * & out)
{
    using namespace trace;
    static LvlDictPair s_dict[] = {
            { "FATAL",   e_Fatal   },
            { "ERROR",   e_Error   },
            { "WARN",    e_Warning },
            { "Normal",  e_Info    },
            { "debug",   e_Debug   },
            { "verbose", e_Verbose },
    };
    out = s_dict;
    return sizeof(s_dict) / sizeof(*s_dict);
}



