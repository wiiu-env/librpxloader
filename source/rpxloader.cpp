#include <coreinit/debug.h>
#include <coreinit/dynload.h>
#include <rpxloader/rpxloader.h>

static OSDynLoad_Module sModuleHandle = nullptr;

static RPXLoaderVersion (*sRLGetVersion)()      = nullptr;
static bool (*sRLLoadFromSDOnNextLaunch)()      = nullptr;
static bool (*sRLDisableContentRedirection)()   = nullptr;
static bool (*sRLEnableContentRedirection)()    = nullptr;
static bool (*sRLUnmountCurrentRunningBundle)() = nullptr;

RPXLoaderStatus RPXLoader_Init() {
    if (OSDynLoad_Acquire("homebrew_rpx_loader", &sModuleHandle) != OS_DYNLOAD_OK) {
        OSReport("RPXLoader_Init: OSDynLoad_Acquire failed.\n");
        return RPX_LOADER_RESULT_MODULE_NOT_FOUND;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_GetVersion", (void **) &sRLGetVersion) != OS_DYNLOAD_OK) {
        OSReport("RPXLoader_Init: RL_GetVersion failed.\n");
        return RPX_LOADER_RESULT_MODULE_MISSING_EXPORT;
    }
    auto res = RPXLoader_GetVersion();
    if (res != RPX_LOADER_MODULE_VERSION) {
        return RPX_LOADER_RESULT_UNSUPPORTED_VERSION;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_LoadFromSDOnNextLaunch", (void **) &sRLLoadFromSDOnNextLaunch) != OS_DYNLOAD_OK) {
        OSReport("RPXLoader_Init: RL_LoadFromSDOnNextLaunch failed.\n");
        return RPX_LOADER_RESULT_MODULE_MISSING_EXPORT;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_DisableContentRedirection", (void **) &sRLDisableContentRedirection) != OS_DYNLOAD_OK) {
        OSReport("RPXLoader_Init: RL_DisableContentRedirection failed.\n");
        return RPX_LOADER_RESULT_MODULE_MISSING_EXPORT;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_EnableContentRedirection", (void **) &sRLEnableContentRedirection) != OS_DYNLOAD_OK) {
        OSReport("RPXLoader_Init: RL_EnableContentRedirection failed.\n");
        return RPX_LOADER_RESULT_MODULE_MISSING_EXPORT;
    }

    if (OSDynLoad_FindExport(sModuleHandle, FALSE, "RL_UnmountCurrentRunningBundle", (void **) &sRLUnmountCurrentRunningBundle) != OS_DYNLOAD_OK) {
        OSReport("RPXLoader_Init: RL_UnmountCurrentRunningBundle failed.\n");
        return RPX_LOADER_RESULT_MODULE_MISSING_EXPORT;
    }

    return RPX_LOADER_RESULT_SUCCESS;
}


RPXLoaderVersion GetVersion();
RPXLoaderVersion RPXLoader_GetVersion() {
    if (sRLGetVersion == nullptr) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }

    return reinterpret_cast<decltype(&GetVersion)>(sRLGetVersion)();
}

bool LoadFromSDOnNextLaunch(const char *);
RPXLoaderStatus RPXLoader_LoadFromSDOnNextLaunch(const char *path) {
    if (sRLLoadFromSDOnNextLaunch == nullptr) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    if (path == nullptr) {
        return RPX_LOADER_RESULT_INVALID_ARG;
    }
    auto res = reinterpret_cast<decltype(&LoadFromSDOnNextLaunch)>(sRLLoadFromSDOnNextLaunch)(path);

    return res ? RPX_LOADER_RESULT_SUCCESS : RPX_LOADER_RESULT_UNKNOWN_ERROR;
}

bool DisableContentRedirection();
RPXLoaderStatus RPXLoader_DisableContentRedirection() {
    if (sRLDisableContentRedirection == nullptr) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }
    auto res = reinterpret_cast<decltype(&DisableContentRedirection)>(sRLDisableContentRedirection)();

    return res ? RPX_LOADER_RESULT_SUCCESS : RPX_LOADER_RESULT_UNKNOWN_ERROR;
}

bool EnableContentRedirection();
RPXLoaderStatus RPXLoader_EnableContentRedirection() {
    if (sRLEnableContentRedirection == nullptr) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }

    auto res = reinterpret_cast<decltype(&EnableContentRedirection)>(sRLEnableContentRedirection)();

    return res ? RPX_LOADER_RESULT_SUCCESS : RPX_LOADER_RESULT_UNKNOWN_ERROR;
}

bool UnmountCurrentRunningBundle();
RPXLoaderStatus RPXLoader_UnmountCurrentRunningBundle() {
    if (sRLUnmountCurrentRunningBundle == nullptr) {
        return RPX_LOADER_RESULT_LIB_UNINITIALIZED;
    }

    auto res = reinterpret_cast<decltype(&UnmountCurrentRunningBundle)>(sRLUnmountCurrentRunningBundle)();

    return res ? RPX_LOADER_RESULT_SUCCESS : RPX_LOADER_RESULT_UNKNOWN_ERROR;
}