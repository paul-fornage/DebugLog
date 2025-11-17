#ifndef DEBUGLOG_COLORS_H
#define DEBUGLOG_COLORS_H

#include "Types.h"

#ifndef DEBUGLOG_USE_COLORS
#define DEBUGLOG_USE_COLORS
#endif


// `\033[38;2;R;G;Bm` or
// `\033[31m` for preset colors
//
#ifndef DEBUGLOG_ERROR_COLOR_TAG
#define DEBUGLOG_ERROR_COLOR_TAG "\033[31m"
#endif
#ifndef DEBUGLOG_WARN_COLOR_TAG
#define DEBUGLOG_WARN_COLOR_TAG "\033[33m"
#endif
#ifndef DEBUGLOG_INFO_COLOR_TAG
#define DEBUGLOG_INFO_COLOR_TAG "\033[34m"
#endif
#ifndef DEBUGLOG_DEBUG_COLOR_TAG
#define DEBUGLOG_DEBUG_COLOR_TAG "\033[35m"
#endif
#ifndef DEBUGLOG_TRACE_COLOR_TAG
#define DEBUGLOG_TRACE_COLOR_TAG "\033[37m"
#endif
#ifndef DEBUGLOG_NONE_COLOR_TAG
#define DEBUGLOG_NONE_COLOR_TAG "\033[37m"
#endif
#ifndef DEBUGLOG_CLEAR_COLOR_TAG
#define DEBUGLOG_CLEAR_COLOR_TAG "\033[0m"
#endif

namespace arx {
namespace debug {
    static const char* generate_color_tag(const LogLevel lvl) {
        switch (lvl) {
        case LogLevel::LVL_ERROR: return DEBUGLOG_ERROR_COLOR_TAG;
        case LogLevel::LVL_WARN:  return DEBUGLOG_WARN_COLOR_TAG;
        case LogLevel::LVL_INFO:  return DEBUGLOG_INFO_COLOR_TAG;
        case LogLevel::LVL_DEBUG: return DEBUGLOG_DEBUG_COLOR_TAG;
        case LogLevel::LVL_TRACE: return DEBUGLOG_TRACE_COLOR_TAG;
        case LogLevel::LVL_NONE:  return DEBUGLOG_NONE_COLOR_TAG;
        }
        return "";
    }
}
}

#endif //DEBUGLOG_COLORS_H