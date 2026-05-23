# Term50 Build Notes

This file records the actual build pipeline for `Term50` and the assumptions needed to reproduce it.

## Summary

Pipeline:

1. compile native C sources with `qcc`
2. link against BB10 system libraries and bundled shared objects
3. package a native BAR with `blackberry-nativepackager`

## Inputs

- source files in `src/`
- headers in `external/include/`
- bundled libs in `external/lib/`
- runtime assets in `share/`
- app metadata in `bar-descriptor.xml`

## Required Environment

```bash
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"
```

## Build Command

```bash
make clean
make
```

Primary binary:

- `Device-Debug/Term50`

## Packaging Command

```bash
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& ^
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat ^
  -package Term50.bar bar-descriptor.xml -devMode"
```

Package output:

- `Term50.bar`

## Linked Libraries

System libraries from the BB10 target SDK:

- `bps`
- `screen`
- `freetype`
- `clipboard`
- `icu`
- `m`

Bundled libraries from this repository:

- `libSDL12.so`
- `libTouchControlOverlay.so`
- `libconfig.so`

## Packaging Behavior

The BAR includes:

- the native executable
- bundled shared libraries
- icons
- terminfo database
- bundled shell and SSH tools under `share/root`

## Practical Limitation

A successful local package build does not imply universal installability on BB10 hardware in `2026`.

Two external constraints remain:

- some devices are still broken by incomplete post-shutdown setup state
- unsigned or dev-mode packages are not substitutes for a historically signed public native release

Reference material:

- BerryFarm signed release channel:
  `https://github.com/BerryFarm/Term49/releases`
- setup/sideload workaround backup:
  `https://lunarproject.org/archive/BlackBerry%2010/OS%20Installs/%5BFix%5D%20BB10%20Devices%20Stuck%20at%20Setup_Sideloading%20Issue.pdf`

## Known Good Outcome for This Repo

As preserved here, the repo is useful for:

- source review
- native rebuilds
- archival packaging
- experimenting on compatible BB10 devices

It should not be treated as a reliable path to a new public signed BB10 release.
