Requires the [RPXLoadingModule](https://github.com/wiiu-env/RPXLoadingModule) to be running via [SetupPayload](https://github.com/wiiu-env/SetupPayload).
Requires [wut](https://github.com/decaf-emu/wut) for building.
Install via `make install`.

# Usage
When linking, make sure to add the librpxloader.ld file to the LDFLAGS.

Example:
```
LDFLAGS	=	-g $(ARCH) $(RPXSPECS) -Wl,-Map,$(notdir $*.map) -T$(WUMS_ROOT)/share/librpxloader.ld
```