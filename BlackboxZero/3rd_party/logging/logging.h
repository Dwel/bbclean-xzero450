#pragma once

#if defined TRACE_ENABLED

# define TRACE_WINDOWS_SOCKET_FAILOVER_NOTIFY_MSVC 1
# define TRACE_WINDOWS_SOCKET_FAILOVER_TO_FILE 1
# if !defined TRACE_STATIC
#   define TRACE_STATIC
# endif

# include <3rd_party/flogging/trace_client/trace.h>

#else
// no tracing at all
# include "trace_dummy.h"
#endif
