# Term49

Term49 is a terminal emulator for BlackBerry 10. It is a continuation of the amazing [Term48](https://github.com/mordak/Term48) project by [mordak](https://github.com/mordak).

It implements (relevant parts of) the [ECMA-48 standard][ecma], but also includes some other control sequences to make it compliant with the `xterm-256color` terminfo specification. It is a work in progress, but is good enough for daily use. Pull requests, feature requests and bug reports are welcome.

The [current release](https://github.com/BerryFarm/Term49/releases) requires OS version >= 10.3.

## Current Customizations

This tree is being extended with a BB10 keyboard-first workflow. The active
focus areas are:

* Japanese-safe text rendering via fallback fonts instead of replacing the
  primary Latin font.
* Scrollback and paste UX that works well with BlackBerry 10 physical
  keyboards first, with touch only as a secondary shortcut.
* A config-driven macro system that can be edited without rebuilding the app.
* Background keep-alive so the shell session stays alive when the app is sent
  to the background.

### Japanese Font Fallback

Term49 still uses a primary fixed-width font for normal terminal layout, but
can now be configured with fallback fonts for missing glyphs such as kana and
kanji. The recommended way to customize this is in `.term49rc` using:

```cfg
fallback_font_paths = [
  "/path/to/your/japanese-font.ttf"
];
```

This approach is safer than globally replacing the primary font because the
terminal still measures columns from the main font.

### Keyboard-First Scroll and Paste

The default metamode function map is being expanded with scroll helpers:

* `m`: open macro menu
* `u` / `d`: half-page scroll up/down
* `b` / `f`: full-page scroll up/down
* `g`: jump to top of scrollback
* `v`: paste clipboard

### Config-Driven Macros

Macros are intended to be edited from config, not source. The current format is:

```cfg
project_macros : (
  ("s", "SSH LAN", "ssh Admin@192.168.1.6\n"),
  ("x", "Example", "echo hello\n")
);
```

Each entry is:

* trigger key
* label shown in the menu
* text sent to the shell

Important: Term49 runs on BB10/QNX. Windows host commands like
`cd "C:\\Users\\..." && python ...` are useful as personal references, but they
will not execute correctly inside the device shell unless adapted for the
device environment.

### Background Keep-Alive

The target behavior is session persistence while the app is backgrounded:

* PTY and child shell remain alive
* rendering pauses while inactive
* returning to foreground resumes the same shell session

This is intentionally lighter-weight than a full headless service.

## Development

To compile Term49, you will need some additional libraries:

* [libSDL][libsdl]
* [Touch Control Overlay][tco]
* [libconfig][libconfig]

Prebuilt versions of these shared libraries are available in `external/lib` (see Makefile); to build from source you will need to check out the submodules (call `git clone` with the `--recursive` option) and build them with the Momentics IDE. Note that when compiling SDL, you must define `-D__PLAYBOOK__ -DRAW_KEYBOARD_EVENTS`.

You can build and deploy Term49 without using Momentics IDE:

* Load the proper `bbndk-env` file
* Copy your debug token to `signing/debugtoken.bar` (or see the section below on generating a debug token)
* Populate the `BBIP` and `BBPASS` fields in `signing/bbpass` with your device's dev-mode IP address and device password
* Update the `<author>` and `<authorId>` tags in `bar-descriptor.xml` to match the `Package-Author` and `Package-Author-Id` for your debug token: `unzip -p signing/debugtoken.bar META-INF/MANIFEST.MF | grep 'Package-Author:\|Package-Author-Id:'`
* `make`
* `make deploy`

## Generating a Debug Token

* Use this form to obtain your `bbidtoken.csk` file: https://developer.blackberry.com/codesigning/
* Copy `bbidtoken.csk` to `signing/bbidtoken.csk`
* In `signing/bbpass`, fill in:
  - `CNNAME`: the Common Name for your signing cert (usually your name)
  - `KEYSTOREPASS`: CSK password you entered in step 1 signup
  - `BBPIN`: target device's PIN
  - `BBPASS`: target device's password
* Run `make` in `signing/Makefile` to request and deploy the token to your device.

Important: any symbols need to be escaped according to bash / Makefile rules e.g. backslashes before symbols `\!` and double dollar signs `\$$`.

## Signing the release

To distribute Term49, you need to sign the application bar with BlackBerry. To do that, run `make sign`.

## Debugging with GDB

To connect to the target device and enable debug tools such as GDB, the `blackberry-connect` tool must be started with the right arguments. For this, two terminals must have the correct `bbndk-env` environment loaded (or run the `make connect` command in the background).

### Terminal 1: `blackberry-connect`
* Start in the Term49 root directory.
* `cd signing`
* If the SSH key hasn't been generated yet, run `make ssh-key`.
* `make connect`
* Leave terminal running until done debugging.

### Terminal 2: `gdb`
* Start in the Term49 root directory.
* `make launch-debug`
* The package will be built, deployed to target device, and launched stopped. On host, `ntoarm-gdb` will start, connect to target device, and attach to the application process. To continue execution, run the GDB command `continue`. Further information on GDB can be found online.

## See also

* [Term48 in BlackBerry AppWorld](http://appworld.blackberry.com/webstore/content/26272878/)

[ecma]: http://www.ecma-international.org/publications/standards/Ecma-048.htm
[libsdl]: https://github.com/mordak/SDL/tree/term48
[tco]: https://github.com/blackberry/TouchControlOverlay
[libconfig]: http://www.hyperrealm.com/libconfig/
