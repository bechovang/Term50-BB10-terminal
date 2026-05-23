# Term50 for BlackBerry 10

Term50 is a native terminal emulator fork for BlackBerry 10, derived from:

- `Term48` by Todd Mortimer (`mordak`)
- `Term49` by BerryFarm

This fork keeps the classic native BB10 terminal workflow and packages:

- an ECMA-48 / VT100 / VT220 compatible terminal
- `xterm-256color` support
- bundled `mksh`, `ssh`, `scp`, `sftp`, `ssh-keygen`
- scrollback support
- symbol and accent overlays
- config-driven macros
- Japanese font fallback support

## Project Status

This repository is considered closed/archived as of `2026-05-23`.

The codebase builds and packages successfully, but there is a hard distribution limit:

- BlackBerry signing infrastructure is gone, so this fork cannot produce a new broadly installable signed native BAR.
- The repo can still be used for source study, local rebuilding, and device-specific experimentation.
- For general end-user installation, the practical path is still the signed BerryFarm `Term49` release.

If you need a package that installs on arbitrary BB10 devices, use the BerryFarm release:

- `https://github.com/BerryFarm/Term49/releases`

## What Works

- Source builds with the legacy BB10 / QNX NDK
- Native BAR packaging
- Local development against BB10 hardware
- Bundled shell and SSH tooling inside the app

## What Does Not Work Anymore

- Obtaining new BlackBerry signing keys
- Producing a new universally installable signed release from this fork
- Relying on old official BlackBerry infrastructure for setup, signing, or deployment

## Installation Reality on BB10 in 2026

There are two separate issues that are easy to confuse:

### 1. Device setup / sideload state

Some BB10 devices that were reset after BlackBerry services shut down never complete setup correctly. In that state, sideload tools can fail with errors such as `881`, or the device may refuse native BAR installs until setup is completed through the community workaround.

Reference:

- CrackBerry backup PDF: `https://lunarproject.org/archive/BlackBerry%2010/OS%20Installs/%5BFix%5D%20BB10%20Devices%20Stuck%20at%20Setup_Sideloading%20Issue.pdf`

### 2. Package trust / signing state

Even on a correctly activated device, a native BAR can still fail because the package itself is not signed in a way BB10 accepts for general installation.

That means:

- if the device installs other native BAR files, the phone is probably already activated
- if this fork's BAR still fails while other native BARs install, the problem is the package trust/signing state, not device activation

In that scenario, the realistic answer is:

- use a signed BerryFarm `Term49` release for installation
- keep `Term50` as a source fork / build artifact / archival project

## Quick Start

### Build requirements

- Windows 10/11
- Java 8
- MSYS2
- legacy BlackBerry 10 NDK / QNX toolchain

### Build

```bash
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"

make clean
make
```

### Package

```bash
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& ^
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat ^
  -package Term50.bar bar-descriptor.xml -devMode"
```

Important:

- `-devMode` produces a development BAR, not a general public release
- on modern BB10 devices, successful installation still depends on device setup state and package trust state

## Configuration

User config lives at `~/.term50rc` on device and uses `libconfig` syntax.

Example:

```cfg
font_path = "/usr/fonts/font_repository/monotype/andalemo.ttf";
fallback_font_paths = ["/accounts/1000/shared/documents/fonts/NotoSansCJK-Regular.ttc"];
font_size = 24;

text_color = [255, 255, 255];
background_color = [0, 0, 0];

tty_encoding = "UTF-8";
background_keepalive = 1;

project_macros : (
    ("s", "SSH LAN", "ssh user@192.168.1.6\n"),
    ("l", "ls", "ls -la\n")
);
```

## Documentation

- Build pipeline: [BUILD.md](BUILD.md)
- Development environment and deployment notes: [DEVELOPMENT.md](DEVELOPMENT.md)

## Repository Layout

```text
src/                     main native source files
external/include/        third-party headers
external/lib/            bundled shared libraries
share/root/              mksh and ssh userland
share/terminfo/          terminal database
icons/                   application icons
signing/                 legacy signing/debug helpers
docker/                  container build helper files
```

## Sources and Historical References

- BerryFarm Term49 releases:
  `https://github.com/BerryFarm/Term49/releases`
- BB10 sideload/setup workaround backup:
  `https://lunarproject.org/archive/BlackBerry%2010/OS%20Installs/%5BFix%5D%20BB10%20Devices%20Stuck%20at%20Setup_Sideloading%20Issue.pdf`
- Original Term48 discussion archive:
  `https://forums.crackberry.com/blackberry-10-apps-f274/archive-term48-terminal-emulator-blackberry-1190358-print/`

## License

This project remains under `Apache License 2.0`. See [LICENSE](LICENSE).
