# Term50 Development Guide

This document describes the practical development state of `Term50` in `2026`, including what is still possible, what is no longer possible, and how to rebuild the native BAR from source.

## Scope

This guide is for:

- rebuilding the app locally
- understanding the BB10 native toolchain
- packaging a development BAR
- preserving project knowledge after project closure

This guide is not a promise of broad end-user installability on every BB10 device.

## Current Status

The project is closed/archived.

Reasons:

- the codebase is in a buildable state
- the historical BB10 signing ecosystem is effectively dead
- the remaining blocker is no longer code quality but platform trust/distribution

## Toolchain

Expected legacy layout:

```text
C:\bbndk\
  host_10_3_1_12\win32\x86
  target_10_3_1_995\qnx6
```

Required host tools:

- `qcc`
- `make`
- `blackberry-nativepackager.bat`
- Java 8

Recommended host OS:

- Windows 10 or Windows 11

## Environment Variables

Use these in MSYS2:

```bash
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"
```

Sanity checks:

```bash
qcc -V
which qcc
```

You should see an ARM target such as `4.6.3,gcc_ntoarmv7le`.

## Build

```bash
make clean
make
```

Expected primary output:

- `Device-Debug/Term50`

## Package

Package the native app as a dev-mode BAR:

```bash
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& ^
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat ^
  -package Term50.bar bar-descriptor.xml -devMode"
```

Expected output:

- `Term50.bar`

## Important Distribution Limits

### Dev-mode BAR is not a public release

`-devMode` is useful for packaging and local testing, but it does not solve the 2026 BB10 trust problem by itself.

### New official signing is effectively unavailable

In practical terms:

- do not plan around getting fresh BlackBerry signing credentials
- do not assume a rebuilt fork can be distributed like an old official release

### Signed BerryFarm release remains the practical install target

For normal installation on end-user devices, prefer:

- `https://github.com/BerryFarm/Term49/releases`

## Native Sideload Reality

There are two separate failure classes.

### Class A: device setup not truly completed

Symptoms:

- DBBTool / DBBT / other sideload tools fail on native BAR installs
- error `881` may appear
- device may show `Complete setup` notifications or behave as if onboarding never fully finished

Community reference:

- `https://lunarproject.org/archive/BlackBerry%2010/OS%20Installs/%5BFix%5D%20BB10%20Devices%20Stuck%20at%20Setup_Sideloading%20Issue.pdf`

Interpretation:

- this is a device state problem
- fix the setup state first

### Class B: package signing / trust mismatch

Symptoms:

- the same device can install some native BAR files
- this fork's BAR still fails

Interpretation:

- the device is likely already activated
- the remaining problem is the package, not device activation

Practical answer:

- use the signed BerryFarm `Term49` build for actual installation
- keep `Term50` as a source archive or experimental fork

## How to Tell Which Problem You Have

Use this decision path:

1. Try installing another known-good native BAR.
2. If every native BAR fails similarly, suspect setup/sideload state.
3. If other native BAR files install but `Term50.bar` fails, suspect package trust/signing.

Additional signal:

- if the phone still nags about setup completion, suspect incomplete activation/setup

## Source Layout

```text
src/main.c             app startup, event loop, rendering
src/buffer.c           terminal buffer and scrollback
src/ecma48.c           escape sequence parser
src/io.c               PTY and process I/O
src/preferences.c      .term50rc parsing
src/symmenu.c          symbol overlay
src/accent_menus.c     accent overlay
src/SDL_ttf.c          font rendering support
external/lib/          bundled BB10 shared libraries
share/root/            bundled mksh and ssh tools
share/terminfo/        terminfo database
```

## Configuration

The runtime config file is `~/.term50rc`.

Common settings:

```cfg
font_path = "/usr/fonts/font_repository/monotype/andalemo.ttf";
fallback_font_paths = ["/accounts/1000/shared/documents/fonts/NotoSansCJK-Regular.ttc"];
font_size = 24;

tty_encoding = "UTF-8";
background_keepalive = 1;
sticky_shift_key = 1;
sticky_alt_key = 1;
```

## Troubleshooting

### `qcc: Command not found`

Your `PATH` or `QNX_HOST` is wrong.

### `fatal error: SDL.h: No such file or directory`

Check:

- `external/include/SDL.h`
- `QNX_TARGET`

### `blackberry-nativepackager: Could not find or load main class`

Run the batch file through `cmd //c`, not directly from MSYS2 bash.

### Native BAR install fails with `881`

Do not assume this automatically means the BAR itself is bad.

First determine:

- does the device install any other native BAR at all?
- does the device still appear stuck in incomplete setup?

If the device is stuck in setup, use the community workaround PDF above.

If the device installs other native BAR files but rejects this fork's BAR, the remaining issue is package trust/signing.

## Archive Note

This repository is being retained as:

- a working source snapshot
- a buildable preservation fork
- documentation for BB10 native terminal work

It is not an actively maintained product release channel.
