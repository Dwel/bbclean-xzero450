#ifndef __WOW64_ADAPTER_H__
#define __WOW64_ADAPTER_H__
#include "BBApiPluginLoader.h"

struct Message {
    char tag;
    unsigned int len;
    //char content[len-5];
};

#define MessageContent(msgPtr, idx) ((char*)(5 + idx + (size_t)msgPtr))

// Appends valueLen bytes from the content of valuePtr to the content of the message pointed to by msgPtr and increments offVar.
// offVar is relative to the start of the content.
#define MessageAppend(msgPtr, offVar, valuePtr, valueLen) { memcpy(MessageContent(msg, offVar), valuePtr, valueLen); offVar += valueLen; }

enum MessageTag {
    msgOk,
    msgErr,

    msgApplyPluginState,
    msgApplyPluginStateResult,

    msgQueryPlugin,
    msgQueryResult,

    msgFinalize,
};

#endif