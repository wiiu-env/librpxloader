#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum RPXLoaderStatus {
    RPX_LOADER_RESULT_SUCCESS                 = 0,
    RPX_LOADER_RESULT_INVALID_ARGUMENT        = -0x01,
    RPX_LOADER_RESULT_NOT_FOUND               = -0x06,
    RPX_LOADER_RESULT_UNSUPPORTED_API_VERSION = -0x10,
    RPX_LOADER_RESULT_UNSUPPORTED_COMMAND     = -0x11,
    RPX_LOADER_RESULT_LIB_UNINITIALIZED       = -0x20,
    RPX_LOADER_RESULT_MODULE_NOT_FOUND        = -0x30,
    RPX_LOADER_RESULT_MODULE_MISSING_EXPORT   = -0x31,
    RPX_LOADER_RESULT_UNKNOWN_ERROR           = -0x100,
} RPXLoaderStatus;

typedef uint32_t RPXLoaderVersion;
#define RPX_LOADER_MODULE_VERSION_ERROR 0xFFFFFFFF

const char *RPXLoader_GetStatusStr(RPXLoaderStatus status);

/**
 * This function has to be called before any other function of this lib (except RPXLoader_GetVersion) can be used.
 *
 * @return  RPX_LOADER_RESULT_SUCCESS:                 The library has been initialized successfully. Other functions can now be used.<br>
 *          RPX_LOADER_RESULT_MODULE_NOT_FOUND:        The module could not be found. Make sure the module is loaded.<br>
 *          RPX_LOADER_RESULT_MODULE_MISSING_EXPORT:   The module is missing an expected export.<br>
 *          RPX_LOADER_RESULT_UNSUPPORTED_API_VERSION: The version of the loaded module is not compatible with this version of the lib.
*/
RPXLoaderStatus RPXLoader_InitLibrary();

/**
 * Deinitializes the RPXLoader lib
 * @return RPX_LOADER_RESULT_SUCCESS
 */
RPXLoaderStatus RPXLoader_DeInitLibrary();

/**
 * Retrieves the API Version of the loaded RPXLoaderModule.<br>
 * <br>
 * @param outVersion pointer to the variable where the version will be stored.
 *
 * @return RPX_LOADER_RESULT_SUCCESS:               The API version has been store in the version ptr.<br>
 *         RPX_LOADER_RESULT_MODULE_NOT_FOUND:      The module could not be found. Make sure the module is loaded.<br>
 *         RPX_LOADER_RESULT_MODULE_MISSING_EXPORT: The module is missing an expected export.<br>
 *         RPX_LOADER_RESULT_INVALID_ARGUMENT:      Invalid version pointer.<br>
 *         RPX_LOADER_RESULT_UNKNOWN_ERROR:         Retrieving the module version failed.
 */
RPXLoaderStatus RPXLoader_GetVersion(uint32_t *outVersion);

/**
 * Sets the .rpx or .wuhb that will be loaded the next time the homebrew wrapper application is launched (e.g. Health & Safety or Daily Log).<br>
 * When a .wuhb will be loaded, accesses to /vol/content will be redirected to the .wuhb, /vol/save will be redirected to the sd card.<br>
 * <br>
 * The path is **relative** to the root of the given target device.<br>
 * <br>
 * To launch the prepared RPX call RPXLoader_LaunchPreparedHomebrew if this call was successful.<br>
 * <br>
 * Requires API version 1 or higher. <br>
 *
 * @param path: path to the .rpx/.wuhb that should be loaded.
 * @return  RPX_LOADER_RESULT_SUCCESS:              Loading of the next RPX will be redirected. <br>
 *          RPX_LOADER_RESULT_LIB_UNINITIALIZED:    Library was not initialized. Call RPXLoader_InitLibrary() before using this function.<br>
 *          RPX_LOADER_RESULT_UNSUPPORTED_COMMAND:  Command not supported by the currently loaded RPXLoaderModule version.<br>
 *          RPX_LOADER_RESULT_INVALID_ARGUMENT:     Given path was NULL<br>
 *          RPX_LOADER_RESULT_UNKNOWN_ERROR:        Unexpected error.<br>
 */
RPXLoaderStatus RPXLoader_PrepareLaunchFromSD(const char *path);

/**
 * Launches the wrapper app for launching the .rpx/.wuhb that has been set via RPXLoader_PrepareLaunchFromSD<br>
 * Works similar similar to the SYSLaunch* functions.<br>
 * <br>
 * see: `RPXLoader_LaunchHomebrew` to prepare and launch a RPX in one command. <br>
 * <br>
 * Requires API version 1 or higher. <br>
 *
 * @return RPX_LOADER_RESULT_SUCCESS:               App is launching<br>
 *         RPX_LOADER_RESULT_LIB_UNINITIALIZED:     Library was not initialized. Call RPX_LOADER_InitLibrary() before using this function.<br>
 *         RPX_LOADER_RESULT_UNSUPPORTED_COMMAND:   Command not supported by the currently loaded RPXLoaderModule version.<br>
 *         RPX_LOADER_RESULT_NOT_FOUND:             No application that can be used as homebrew wrapper found.<br>
 *         RPX_LOADER_RESULT_UNKNOWN_ERROR:         Unexpected error.
 */
RPXLoaderStatus RPXLoader_LaunchPreparedHomebrew();

/**
 * Launches a given .rpx/.wuhb by launching a wrapper application and replacing the RPX on the fly. <br>
 * See RPXLoader_PrepareLaunchFromSD for more information. <br>
 * <br>
 * Note: Combines RPXLoader_PrepareLaunchFromSD and RPXLoader_LaunchPreparedHomebrew. <br>
 * <br>
 * Requires API version 1 or higher. <br>
 *
 * @param bundle_path path to the .rpx/.wuhb that should be loaded.
 * @return RPX_LOADER_RESULT_SUCCESS:               Requested RPX/WUHB will be launched<br>
 *         RPX_LOADER_RESULT_LIB_UNINITIALIZED:     Library was not initialized. Call RPXLoader_InitLibrary() before using this function.<br>
 *         RPX_LOADER_RESULT_UNSUPPORTED_COMMAND:   Command not supported by the currently loaded RPXLoaderModule version.<br>
 *         RPX_LOADER_RESULT_INVALID_ARGUMENT:      The given path was NULL <br>
 *         RPX_LOADER_RESULT_NOT_FOUND:             No application that can be used as homebrew wrapper found.<br>
 *         RPX_LOADER_RESULT_UNKNOWN_ERROR:         Unexpected error.
 */
RPXLoaderStatus RPXLoader_LaunchHomebrew(const char *bundle_path);

/**
 * Enables the /vol/content redirection (is enabled by default if a .wuhb is running) <br>
 * <br>
 * Requires API version 1 or higher. <br>
 *
 * @return  RPX_LOADER_RESULT_SUCCESS:              /vol/content has been enabled or was already enabled. <br>
 *          RPX_LOADER_RESULT_LIB_UNINITIALIZED:    "RPXLoader_Init()" was not called. <br>
 *          RPX_LOADER_RESULT_UNKNOWN_ERROR:        Error
 */
RPXLoaderStatus RPXLoader_EnableContentRedirection();

/**
 * Disables the /vol/content redirection <br>
 * <br>
 * Requires API version 1 or higher. <br>
 *
 * @return  RPX_LOADER_RESULT_SUCCESS:              /vol/content has been disabled or was already disabled. <br>
 *          RPX_LOADER_RESULT_LIB_UNINITIALIZED:    "RPXLoader_Init()" was not called. <br>
 *          RPX_LOADER_RESULT_UNKNOWN_ERROR:        Error
*/
RPXLoaderStatus RPXLoader_DisableContentRedirection();

/**
 * Unmounts the currently running bundle. This also disables the /vol/content redirection <br>
 * <br>
 * Requires API version 1 or higher. <br>
 *
 * @return  RPX_LOADER_RESULT_SUCCESS:              /vol/content has been unmounted or was not previously mounted.<br>
 *          RPX_LOADER_RESULT_LIB_UNINITIALIZED:    "RPXLoader_Init()" was not called.<br>
 *          RPX_LOADER_RESULT_UNKNOWN_ERROR:        Unable to unmount the currently running bundle.<br>
*/
RPXLoaderStatus RPXLoader_UnmountCurrentRunningBundle();

#ifdef __cplusplus
} // extern "C"
#endif