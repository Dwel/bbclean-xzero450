#include "WOW64Adapter.h"
#include <strsafe.h>
#include <stdlib.h>

//----------- Messaging.h ------------
void PipeSendMessage(char messageTag, ...);
int PipeGetMessage(char messageTag, int timeoutMillis, char** errMsg, ...);
//------------------------------------

const char* name = "WOW64Adapter";

const int plugInfuBuffersize = 1023;
char plugInfoBuffer[plugInfuBuffersize];

static const int timeout = 500;

// TODO: check for memleaks. esp make sure about these error strings :3
// TODO: re-initialize upon timeout
// TODO: check if we need specific error codes for timeout messages.
bool Init() {
    // TODO: 
    //    var name = GeneratePipeName()
    //    var pipe = CreatePipe(name);
    //    var host = exec("pluginHost32.exe", name);
    //    
    //    var msg = ReadPipe(name, timeout: 500)
    //    
    //    if(!msg || msg != "OK") {
    //        CloseHandle(pipe);
    //        kill(host);
    //        return false;
    //    }
    //    
    //    return true;
    return true;
}

void Finalize() {
    PipeSendMessage(msgFinalize);
    PipeGetMessage(msgOk, 500, nullptr);

    //   CloseHandle(pipe);
    //   KillProcess(host);
}

const char *GetName() {
    return name;
}

const char *GetApi() {
    // TODO: concatenate apis of loaded 32bit pluginloaders in host
    return "32bit multi";
}

const char *GetPluginInfo(struct PluginList* plugin, int factId) {
    PipeSendMessage(msgQueryPlugin, plugin, factId);
    
    auto err = PipeGetMessage(msgQueryResult, timeout, nullptr, plugInfoBuffer, plugInfuBuffersize);

    if(err == 0)
        return plugInfoBuffer;
    else if(err == 1) {
        // TODO: handle timeout
    }
    
    return nullptr;
}

int LoadPlugin(struct PluginList* plugin, HWND hSlit, char** errorMsg) {
    plugin->isEnabled = true;
    
    PipeSendMessage(msgApplyPluginState, plugin, hSlit);
    
    auto err = PipeGetMessage(msgApplyPluginStateResult, timeout, errorMsg, plugin);
    
    if(err == 0)
        return 0;

    if(err == 1) {
        // TODO: handle timeout
    }

    plugin->isEnabled = false;
    plugin->inSlit = false;

    return !!errorMsg ? error_plugin_message : error_plugin_does_not_load;
}

int UnloadPlugin(struct PluginList* plugin, char** errorMsg) {
    bool wasEnabled = plugin->isEnabled;
    
    plugin->isEnabled = false;
    
    PipeSendMessage(msgApplyPluginState, plugin);
    
    auto err = PipeGetMessage(msgApplyPluginStateResult, timeout, errorMsg, plugin);

    if(err == 0)
        return 0;

    if(err == 1) {
        // TODO: handle timeout
    }

    plugin->isEnabled = wasEnabled;

    return !!errorMsg ? error_plugin_message : error_plugin_crash_on_unload;
}

//----------- Messaging.cpp ------------
// TODO: move this into a shared .h/.cpp pair when it's done

/// <summary>Creates a message which instructs the 32bit host to update the pluginstate of a plugin.</summary>
/// <param name="plugin">The plugin which should be updates.</param>
/// <param name="hSlit">The handle of the slit if it exists, otherwise NULL.</param>
/// <returns>A pointer to the newly allocated message, the caller should free the message once done with it.</returns>
struct Message* encodeApplyPluginState(struct PluginList* plugin, HWND hSlit) {
    struct Message *msg;
    unsigned int msgLen, off = 0;
    char flags;

    flags = (plugin->isEnabled << 3) | (plugin->canUseSlit << 2) | (plugin->useSlit << 1) | (char)plugin->inSlit;

    msgLen = sizeof(struct Message) + sizeof(char) + strlen(plugin->path) + 1 + sizeof(HWND);
    msg = (struct Message*)calloc(1, msgLen);

    msg->tag = msgApplyPluginState;
    msg->len = msgLen;
    
    MessageAppend(msg, off, &flags, sizeof(char));
    MessageAppend(msg, off, plugin->path, strlen(plugin->path) + 1);
    MessageAppend(msg, off, &hSlit, sizeof(HWND));

    return msg;
}

/// <summary>Creates a message which instructs the 32bit host to query information from the plugin.</summary>
/// <param name="plugin">The plugin whose information to query.</param>
/// <param name="factId">
///     Determines which datum is queried.
///     Typical values: 1: name, 2: version, 3: author, 4: release. For other values see BBApi.h.
/// </param>
/// <returns>A pointer to the newly allocated message, the caller should free the message once done with it.</returns>
struct Message* encodeQueryPlugin(struct PluginList* plugin, int factId) {
    struct Message *msg;
    unsigned int msgLen;

    msgLen = sizeof(struct Message) + strlen(plugin->path) + 1;
    msg = (struct Message*)calloc(1, msgLen);

    msg->tag = msgQueryPlugin;
    msg->len = msgLen;

    strcpy(MessageContent(msg, 0), plugin->path);

    return msg;
}

/// <summary>Creates a message which instructs the 32bit host to unload all plugins and terminate.</summary>
/// <returns>A pointer to the newly allocated message, the caller should free the message once done with it.</returns>
struct Message* encodeFinalize() {
    struct Message* msg = (struct Message*)calloc(1, sizeof(struct Message*));
    msg->tag = msgFinalize;

    return msg;
}

/// <summary>Decodes the result message of a load instruction.</summary>
/// <param name="msg">The message to decode.</param>
/// <param name="plugin">The plugin entry which receives the updated values.</param>
/// <remarks>The resulting value of plugin->loaderInfo is not a valid pointer and should not be dereferenced.</remarks>
void decodeApplyPluginStateResult(struct Message* msg, struct PluginList* plugin) {
    if(!msg || !plugin)
        return;
    
    char flags = *MessageContent(msg, 0);
    int info = *(int*)(MessageContent(msg, 1));

    plugin->isEnabled  = !!(flags & 0x08);
    plugin->canUseSlit = !!(flags & 0x04);
    plugin->useSlit    = !!(flags & 0x02);
    plugin->inSlit     = !!(flags & 0x01);
    plugin->loaderInfo = (void*)info;
}

/// <summary>Decodes the result message of a query instruction.</summary>
/// <param name="msg">The message to decode.</param>
/// <param name="buf">The buffer which will receive the message content.</param>
/// <param name="bufSize">The size of the target buffer.</param>
/// <remarks>If bufSize is less than the message content, the copy will be truncated. The method guarantees a terminating NUL character.</remarks>
void decodeQueryResult(struct Message* msg, char* buf, int bufSize) {
    if(!msg || !buf)
        return;

    int contentLen = (msg->len - sizeof(struct Message) + 1);
    int len = bufSize == contentLen ? bufSize :
              bufSize < contentLen ? bufSize - 1 :
              contentLen;

    memcpy(buf, MessageContent(msg, 0), len);
    buf[bufSize - 1] = 0;
}

/// <summary></summary>
/// <param name="messageTag">The tag of the message.</param>
/// <param name="...">Parameters depending on the type of the message. See the corresponding encode* methods.</param>
void PipeSendMessage(char messageTag, ...) {
    struct Message *msg;

    va_list va; va_start(va, messageTag);

    if(messageTag == msgApplyPluginState) {
        struct PluginList *plugin = va_arg(va, struct PluginList*);
        HWND hSlit = va_arg(va, HWND);
        
        msg = encodeApplyPluginState(plugin, hSlit);
    } else if(messageTag == msgQueryPlugin) {
        struct PluginList *plugin = va_arg(va, struct PluginList*);
        int factId = va_arg(va, int);

        msg = encodeQueryPlugin(plugin, factId);
    } else if(messageTag == msgFinalize) {
        msg = encodeFinalize();
    }
    
    va_end(va);

    // TODO: actually send the message
    free(msg);
}

/// <summary>Get a formatted message of a specified type from the named pipe.</summary>
/// <param name="messageTag">The expected tag of the next message.</param>
/// <param name="timeoutMillis">The maximum time in milliseconds to wait before the operation times out.</param>
/// <param name="errMsg">(OUT) Receives the error message upon error. Can be NULL.</param>
/// <param name="...">Arguments specific to the messageTag, see the corresponding decode* methods. Ignored upon error.</param>
/// <returns>0 upon success, 1 if a timeout occured and 2 if the message tag does not match the expected type or the host returned an error.</returns>
int PipeGetMessage(char messageTag, int timeoutMillis, char** errMsg, ...) {
    struct Message* msg = (struct Message*)calloc(1, sizeof(struct Message*)); // HACK: fix compiler warning for now
    // TODO: actually get the message

    bool timedOut = false;

    if(timedOut) {
        if(errMsg) {
            const char* err = "Error communicating with Wow64 host: response timeout";

            *errMsg = (char*)calloc(1, strlen(err)+1);
            strcpy(*errMsg, err);
        }

        free(msg);
        return 1;
    }

    // Check for errors
    if(msg->tag != messageTag) {
        if(msg->tag == msgErr && errMsg) {
            *errMsg = (char*)calloc(1, msg->len);
            memcpy(*errMsg, MessageContent(msg, 0), msg->len);
        } else if(errMsg) {
            char err[1024];
            sprintf(err, "Error communicating with Wow64 host: unexpected return message received: 0x%02X", msg->tag);

            *errMsg = (char*)calloc(1, strlen(err)+1);
            strcpy(*errMsg, err);
        }

        free(msg);
        return 2;
    }

    // va convenience
    va_list va;
    va_start(va, errMsg);
    switch(msg->tag) {
        case msgApplyPluginStateResult: {
            struct PluginList* plugin = va_arg(va, struct PluginList*);

            decodeApplyPluginStateResult(msg, plugin);
            break;
        } case msgQueryResult: {
            char* buf = va_arg(va, char*);
            int bufSize = va_arg(va, int);

            decodeQueryResult(msg, buf, bufSize);
            break;
        } case msgErr: {
            if(msg->tag == msgErr && errMsg) {
                *errMsg = (char*)calloc(1, msg->len);
                memcpy(*errMsg, MessageContent(msg, 0), msg->len);
            }
        } case msgOk:
            break;
    }
    va_end(va);

    return 0;
}
