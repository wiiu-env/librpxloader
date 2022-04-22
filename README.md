[![Publish Docker Image](https://github.com/wiiu-env/librpxloader/actions/workflows/push_image.yml/badge.svg)](https://github.com/wiiu-env/librpxloader/actions/workflows/push_image.yml)

# librpxloader
Requires the [RPXLoadingModule](https://github.com/wiiu-env/RPXLoadingModule) to be running via [WUMSLoader](https://github.com/wiiu-env/WUMSLoader).
Requires [wut](https://github.com/devkitPro/wut) for building.
Install via `make install`.

## Usage

Make sure to define 
```
WUMS_ROOT := $(DEVKITPRO)/wums
```
and add `-lrpxloader` to `LIBS` and `$(WUMS_ROOT)` to `LIBDIRS`.

After that you can simply include `<rpxloader/rpxloader.h>` to get access to the RPXLoader functions.

To init the library call `RPXLoader_Init()` and check for the `RPX_LOADER_RESULT_SUCCESS` return code.

## Use this lib in Dockerfiles.
A prebuilt version of this lib can found on dockerhub. To use it for your projects, add this to your Dockerfile.
```
[...]
COPY --from=wiiuenv/librpxloader:[tag] /artifacts $DEVKITPRO
[...]
```
Replace [tag] with a tag you want to use, a list of tags can be found [here](https://hub.docker.com/r/wiiuenv/librpxloader/tags). 
It's highly recommended to pin the version to the **latest date** instead of using `latest`.

## Format the code via docker

`docker run --rm -v ${PWD}:/src wiiuenv/clang-format:13.0.0-2 -r ./source ./include -i`
