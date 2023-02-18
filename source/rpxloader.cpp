#include "logger.h"
#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <rpxloader/rpxloader.h>

static OSDynLoad_Module sModuleHandle = nullptr;

static RPXLoaderVersion (*sRLGetVersion)(uint32_t *version) = nullptr;

static RPXLoaderStatus (*sRLPrepareLaunchFromSD)(const char *path)   = nullptr;
static RPXLoaderStatus (*sRLLaunchPreparedHomebrew)()                = nullptr;
static RPXLoaderStatus (*sRLLaunchHomebrew)(const char *bundle_path) = nullptr;

static RPXLoaderStatus (*sRLDisableContentRedirection)()                                    = nullptr;
static RPXLoaderStatus (*sRLEnableContentRedirection)()                                     = nullptr;
static RPXLoaderStatus (*sRLUnmountCurrentRunningBundle)()                                  = nullptr;
static RPXLoaderStatus (*sRL_GetPathOfRunningExecutable)(char *outBuffer, uint32_t outSize) = nullptr;

const char *RPXLoader_GetStatusStr(RPXLoaderStatus status) {
    switch (status) {
        case RPX_LOADER_RESULT_SUCCESS:
            return "RPX_LOADER_RESULT_SUCCESS";
        case RPX_LOADER_RESULT_MODULE_NOT_FOUND:
            return "RPX_LOADER_RESULT_MODULE_NOT_FOUND";
        case RPX_LOADER_RESULT_MODULE_MISSING_EXPORT:
            return "RPX_LOADER_RESULT_MODULE_MISSING_EXPORT";
        case RPX_LOADER_RESULT_LIB_UNINITIALIZED:
            return "RPX_LOADER_RESULT_LIB_UNINITIALIZED";
        case RPX_LOADER_RESULT_UNKNOWN_ERROR:
            return "RPX_LOADER_RESULT_UNKNOWN_ERROR";
        case RPX_LOADER_RESULT_INVALID_ARGUMENT:
            return "RPX_LOADER_RESULT_INVALID_ARGUMENT";
        case RPX_LOADER_RESULT_NOT_FOUND:
            return "RPX_LOADER_RESULT_NOT_FOUND";
        case RPX_LOADER_RESULT_UNSUPPORTED_API_VERSION:
            return "RPX_LOADER_RESULT_UNSUPPORTED_API_VERSION";
        case RPX_LOADER_RESULT_UNSUPPORTED_COMMAND:
            return "RPX_LOADER_RESULT_UNSUPPORTED_COMMAND";
        case RPX_LOADER_RESULT_NOT_AVAILABLE:
            return "RPX_LOADER_RESULT_NOT_AVAILABLE";
    }
    return "RPX_LOADER_RESULT_UNKNOWN_ERROR";
}

static RPXLoaderVersion rpxLoaderVersion = RPX_LOADER_MODULE_VERSION_ERROR;

RPXLoaderStatus RPXLoader_InitLibrary() {
    if (OSDynLoad_Acquire("homebrew_rpx_loader", &sModuleHandle) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("OSDynLoad_Acquire homebrew_rpx_loader failed.");
        return RPX_LOADER_RESULT_MODULE_NOT_FOUND;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_GetVersion", (void **) &sRLGetVersion) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_ERR("FindExport RL_GetVersion failed.");
        return RPX_LOADER_RESULT_MODULE_MISSING_EXPORT;
    }

    auto res = RPXLoader_GetVersion(&rpxLoaderVersion);
    if (res != RPX_LOADER_RESULT_SUCCESS) {
        return RPX_LOADER_RESULT_UNSUPPORTED_API_VERSION;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_PrepareLaunchFromSD", (void **) &sRLPrepareLaunchFromSD) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport RL_PrepareLaunchFromSD failed.");
        sRLPrepareLaunchFromSD = nullptr;
    }
    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_LaunchPreparedHomebrew", (void **) &sRLLaunchPreparedHomebrew) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport RL_LaunchPreparedHomebrew failed.");
        sRLLaunchPreparedHomebrew = nullptr;
    }
    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_LaunchHomebrew", (void **) &sRLLaunchHomebrew) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport RL_LaunchHomebrew failed.");
        sRLLaunchHomebrew = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_DisableContentRedirection", (void **) &sRLDisableContentRedirection) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport RL_DisableContentRedirection failed.");
        sRLDisableContentRedirection = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_EnableContentRedirection", (void **) &sRLEnableContentRedirection) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport RL_EnableContentRedirection failed.");
        sRLEnableContentRedirection = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_UnmountCurrentRunningBundle", (void **) &sRLUnmountCurrentRunningBundle) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport RL_UnmountCurrentRunningBundle failed.");
        sRLUnmountCurrentRunningBundle = nullptr;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_GetPathOfRunningExecutable", (void **) &sRL_GetPathOfRunningExecutable) != OS_DYNLOAD_OK) {
        DEBUG_FUNCTION_LINE_WARN("FindExport RL_GetPathOfRunningExecutable failed.");
        sRL_GetPathOfRunningExecutable = nullptr;
    }

    return RPX_LOADER_RESULT_SUCCESS;
}

RPXLoaderStatus RPXLoader_DeInitLibrary() {
    return RPX_LOADER_RESULT_SUCCESS;
}

RPXLoaderStatus RPXLoader_GetVersion(uint32_t *version) {
    if (sRLGetVersion == nullptr) {
        if (OSDynLoad_Acquire("homebrew_rpx_loader", &sModuleHandle) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("OSDynLoad_Acquire failed.");
            return RPX_LOADER_RESULT_MODULE_NOT_FOUND;
        }

        if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_GetVersion", (void **) &sRLGetVersion) != OS_DYNLOAD_OK) {
            DEBUG_FUNCTION_LINE_WARN("FindExport RL_GetVersion failed.");
            return RPX_LOADER_RESULT_MODULE_MISSING_EXPORT;
        }
    }
    if (version == nullptr) {
        return RPX_LOADER_RESULT_INVALID_ARGUMENT;
    }

    return reinterpret_cast<decltype(&RPXLoader_GetVersion)>(sRLGetVersion)(version);
}

RPXLoaderStatus RPXLoader_PrepareLaunchFromSD(const char *path) {
    if (rpxLoaderVersion == RPX_LOADER_MODULE_VERSION_ERROR) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sRLPrepareLaunchFromSD == nullptr || rpxLoaderVersion < 1) {
        return RPX_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }
    if (path == nullptr) {
        return RPX_LOADER_RESULT_INVALID_ARGUMENT;
    }
    return reinterpret_cast<decltype(&RPXLoader_PrepareLaunchFromSD)>(sRLPrepareLaunchFromSD)(path);
}

RPXLoaderStatus RPXLoader_LaunchPreparedHomebrew() {
    if (rpxLoaderVersion == RPX_LOADER_MODULE_VERSION_ERROR) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sRLLaunchPreparedHomebrew == nullptr || rpxLoaderVersion < 1) {
        return RPX_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }
    return reinterpret_cast<decltype(&RPXLoader_LaunchPreparedHomebrew)>(sRLLaunchPreparedHomebrew)();
}

RPXLoaderStatus RPXLoader_LaunchHomebrew(const char *bundle_path) {
    if (rpxLoaderVersion == RPX_LOADER_MODULE_VERSION_ERROR) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sRLLaunchHomebrew == nullptr || rpxLoaderVersion < 1) {
        return RPX_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }
    return reinterpret_cast<decltype(&RPXLoader_LaunchHomebrew)>(sRLLaunchHomebrew)(bundle_path);
}

RPXLoaderStatus RPXLoader_EnableContentRedirection() {
    if (rpxLoaderVersion == RPX_LOADER_MODULE_VERSION_ERROR) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sRLEnableContentRedirection == nullptr || rpxLoaderVersion < 1) {
        return RPX_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }

    return reinterpret_cast<decltype(&RPXLoader_EnableContentRedirection)>(sRLEnableContentRedirection)();
}

RPXLoaderStatus RPXLoader_DisableContentRedirection() {
    if (rpxLoaderVersion == RPX_LOADER_MODULE_VERSION_ERROR) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sRLDisableContentRedirection == nullptr || rpxLoaderVersion < 1) {
        return RPX_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }
    return reinterpret_cast<decltype(&RPXLoader_DisableContentRedirection)>(sRLDisableContentRedirection)();
}

RPXLoaderStatus RPXLoader_UnmountCurrentRunningBundle() {
    if (rpxLoaderVersion == RPX_LOADER_MODULE_VERSION_ERROR) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sRLUnmountCurrentRunningBundle == nullptr || rpxLoaderVersion < 1) {
        return RPX_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }

    return reinterpret_cast<decltype(&RPXLoader_UnmountCurrentRunningBundle)>(sRLUnmountCurrentRunningBundle)();
}

RPXLoaderStatus RPXLoader_GetPathOfRunningExecutable(char *outBuffer, uint32_t outSize) {
    if (rpxLoaderVersion == RPX_LOADER_MODULE_VERSION_ERROR) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (sRL_GetPathOfRunningExecutable == nullptr || rpxLoaderVersion < 2) {
        return RPX_LOADER_RESULT_UNSUPPORTED_COMMAND;
    }

    return reinterpret_cast<decltype(&RPXLoader_GetPathOfRunningExecutable)>(sRL_GetPathOfRunningExecutable)(outBuffer, outSize);
}