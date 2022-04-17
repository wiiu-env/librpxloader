#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum RPXLoaderStatus {
    RPX_LOADER_RESULT_SUCCESS               = 0,
    RPX_LOADER_RESULT_MODULE_NOT_FOUND      = -1,
    RPX_LOADER_RESULT_MODULE_MISSING_EXPORT = -2,
    RPX_LOADER_RESULT_UNSUPPORTED_VERSION   = -3,
    RPX_LOADER_RESULT_INVALID_ARG           = -10,
    RPX_LOADER_RESULT_LIB_UNINITIALIZED     = -20,
    RPX_LOADER_RESULT_UNKNOWN_ERROR         = -1000,
};

typedef uint32_t RPXLoaderVersion;
#define RPX_LOADER_MODULE_VERSION 0x00000001

/**
 * This function has to be called before any other function of this lib (except RPXLoader_GetVersion) can be used.
 *
 * @return  RPX_LOADER_RESULT_SUCCESS:                 The library has been initialized successfully. Other functions can now be used.
 *          RPX_LOADER_RESULT_MODULE_NOT_FOUND:        The module could not be found. Make sure the module is loaded.
 *          RPX_LOADER_RESULT_MODULE_MISSING_EXPORT:   The module is missing an expected export.
 *          RPX_LOADER_RESULT_UNSUPPORTED_VERSION:     The version of the loaded module is not compatible with this version of the lib.
*/
RPXLoaderStatus RPXLoader_Init();

/**
 * Returns the API Version of the RPXLoader Module.
 * @return The RPXLoaderVersion of the Module.
 */
RPXLoaderVersion RPXLoader_GetVersion();

/**
 * Setups the given .rpx or .wuhb to be loaded on the next application restart.
 * Make sure to (re-)launch the H&S app after calling this function.
 *
 * Calling this for a .rpx while the .wuhb is running while keep the bundle mounted,
 * just the .rpx will be replaced.
 *
 * @param bundle_path relative path the root of the sd card (e.g. "/apps/test.wuhb")
 * @return  RPX_LOADER_RESULT_SUCCESS:              The given homebrew will be executed on the next Launch of Health and Safety
 *          RPX_LOADER_RESULT_LIB_UNINITIALIZED:    "RPXLoader_Init()" was not called.
 *          RPX_LOADER_RESULT_INVALID_ARG:          "bundle_path" was NULL
 *          RPX_LOADER_RESULT_UNKNOWN_ERROR:        Error. No effect.
*/
RPXLoaderStatus RPXLoader_LoadFromSDOnNextLaunch(const char *bundle_path);

/**
 * Enables the /vol/content redirection (is enabled by default if a .wuhb is running)
 *
* @return  RPX_LOADER_RESULT_SUCCESS:              /vol/content has been enabled or was already enabled.
*          RPX_LOADER_RESULT_LIB_UNINITIALIZED:    "RPXLoader_Init()" was not called.
*          RPX_LOADER_RESULT_UNKNOWN_ERROR:        Error
 */
RPXLoaderStatus RPXLoader_EnableContentRedirection();

/**
 * Disables the /vol/content redirection
 *
 * @return  RPX_LOADER_RESULT_SUCCESS:              /vol/content has been disabled or was already disabled.
 *          RPX_LOADER_RESULT_LIB_UNINITIALIZED:    "RPXLoader_Init()" was not called.
 *          RPX_LOADER_RESULT_UNKNOWN_ERROR:        Error
*/
RPXLoaderStatus RPXLoader_DisableContentRedirection();

/**
 * Unmounts the currently running bundle. This also disables the /vol/content redirection
 *
 * @return  RPX_LOADER_RESULT_SUCCESS:              /vol/content has been unmounted or was not previously mounted.
 *          RPX_LOADER_RESULT_LIB_UNINITIALIZED:    "RPXLoader_Init()" was not called.
 *          RPX_LOADER_RESULT_UNKNOWN_ERROR:        Unable to unmount the currently running bundle.
*/
RPXLoaderStatus RPXLoader_UnmountCurrentRunningBundle();

#ifdef __cplusplus
} // extern "C"
#endif