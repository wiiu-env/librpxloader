#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef enum {
    BundleSource_FileDescriptor,
    BundleSource_FileDescriptor_CafeOS,
} BundleSource;

/**
 * Setups the given .rpx or .wuhb to be loaded on the next application restart.
 * Make sure to (re-)launch the H&S app after calling this function.
 *
 * Calling this for a .rpx while the .wuhb is running while keep the bundle mounted,
 * just the .rpx will be replaced.
 *
 * @param bundle_path relative path the root of the sd card (e.g. "/apps/test.wuhb")
 * @return
 */
bool RL_LoadFromSDOnNextLaunch(const char *bundle_path);

/**
 * Mounts a given bundle to a given mount path. Use RL_UnmountBundle to unmount it
 *
 * Caution: the mounted path is only available via the
 * RL_FileXXX functions
 *
 * @param name          path the bundle should be mounted to (e.g. "rom")
 * @param bundle_path   path to the bundle file  (e.g. "/vol/external01/apps/test.wuhb")
 * @param source        type of source
 * @return 0 on success, < 0 on error
 */
int32_t RL_MountBundle(const char *name, const char *bundle_path, BundleSource source);

/**
 * Unmounts a given mount path.
 *
 * @param name          given mount path that should be unmounted
 * @return 0 on success, < 0 on error
 */
int32_t RL_UnmountBundle(const char *name);

/**
 * Opens a file inside a mounted bundle.
 * (only read only is supported and is default)
 *
 * Make sure the bundle is mounted via RL_MountBundle.
 *
 * If a given files does not exists, it's checks for a compressed version
 * (at name + ".gz). If a compressed file was found, all file reads will be
 * decompressed on the fly.
 *
 * @param name
 * @param handle        handle to be used in other RL_FileXXX functions
 * @return
 */
int32_t RL_FileOpen(const char *name, uint32_t *handle);

/**
 * Reads from a given file.
 *
 * @param handle    File to be read from.
 * @param buffer    buffer where data will be written to.
 *                  Align to 0x40 for best performance
 * @param size      maximum bytes this function should read into buffer
 * @return  number of actually read bytes
 *          <0 on error
 */
int32_t RL_FileRead(uint32_t handle, uint8_t *buffer, uint32_t size);

/**
 * Closes a given file
 *
 * example: if(RL_FileClose("bundle:/meta/meta.ini") < 0) { //error while closing the file }
 *
 * @param handle    File to be closed
 * @return 0 on success, <0 on error
 */
int32_t RL_FileClose(uint32_t handle);

/**
 * Checks if a given file exists
 *
 * example: RL_FileExists("bundle:/meta/meta.ini");
 *
 * @param name      Paths to be checked
 * @return true if the files exists, false if the files does not exists
 */
bool RL_FileExists(const char *name);

/**
 * Redirects /vol/content to a given path.
 * If the application requests a file that is not present in the new path, 
 * it'll fallback to the original /vol/content.
 *
 * example: RL_RedirectContentWithFallback("fs:/vol/external01/sdcafiine/0005000010145000/content");
 *
 * @param pathToContent The path /vol/content should be redirected to.
 * @return true if the given path is valid and can be used, false on error.
 */
bool RL_RedirectContentWithFallback(const char *pathToContent);

/**
 * Disables the /vol/content redirection 
 *
 * @return true if /vol/content was previously redirected
 */
bool RL_DisableContentRedirection();

#ifdef __cplusplus
} // extern "C"
#endif