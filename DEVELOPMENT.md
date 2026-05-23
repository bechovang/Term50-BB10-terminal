# Term50 - Development Guide

Hướng dẫn đầy đủ về cài đặt môi trường phát triển, build, deploy và debug ứng dụng Term50 (terminal emulator cho BlackBerry 10).

---

## Mục lục

1. [Tổng quan dự án](#1-tổng-quan-dự-án)
2. [Yêu cầu hệ thống](#2-yêu-cầu-hệ-thống)
3. [Cài đặt BB10 NDK (QNX Toolchain)](#3-cài-đặt-bb10-ndk-qnx-toolchain)
4. [Cấu hình biến môi trường](#4-cấu-hình-biến-môi-trường)
5. [Build project](#5-build-project)
6. [Đóng gói file .bar](#6-đóng-gói-file-bar)
7. [Deploy lên thiết bị](#7-deploy-lên-thiết-biết)
8. [Signing (phát hành chính thức)](#8-signing-phát-hành-chính-thức)
9. [Debug với GDB](#9-debug-với-gdb)
10. [Cấu trúc dự án](#10-cấu-trúc-dự-án)
11. [Kiến trúc source code](#11-kiến-trúc-source-code)
12. [Config file (.term49rc)](#12-config-file-term49rc)
13. [Build với Docker (Linux)](#13-build-với-docker-linux)
14. [Troubleshooting](#14-troubleshooting)

---

## 1. Tổng quan dự án

Term50 là terminal emulator cho BlackBerry 10, phát triển tiếp từ [Term48](https://github.com/mordak/Term48) và [Term49](https://github.com/BerryFarm/Term49). Ứng dụng implement chuẩn ECMA-48 và tương thích `xterm-256color`.

**Tính năng chính:**
- Terminal emulator đầy đủ ECMA-48 / VT100 / VT220
- Hỗ trợ 256 màu (xterm-256color)
- Japanese font fallback (hiển thị kana/kanji)
- Scrollback buffer với keyboard shortcuts
- Config-driven macro system
- Symbol menu (sym) và accent menu
- Background keep-alive (giữ session khi app bị minimise)
- SSH/SCP/SFTP tích hợp (bundle trong app)

**Yêu cầu thiết bị:** BlackBerry 10 OS >= 10.3

---

## 2. Yêu cầu hệ thống

### Host machine (Windows)

| Yêu cầu | Chi tiết |
|---------|----------|
| **OS** | Windows 10/11 (64-bit) |
| **Java** | JDK 8 (cần cho `blackberry-nativepackager`) |
| **MSYS2** | [msys2.org](https://www.msys2.org/) - cung cấp `make`, `sed`, `curl` |
| **Git** | Phiên bản bất kỳ |

### BB10 NDK Toolchain

| Component | Phiên bản | Ghi chú |
|-----------|-----------|---------|
| **QNX Compiler (qcc)** | GCC 4.6.3 | Cross-compiler cho ARM |
| **Target architecture** | `armle-v7` | ARMv7 little-endian |
| **BB10 SDK** | 10.3.1.995 | Headers + libraries |
| **blackberry-nativepackager** | - | Đóng gói .bar |
| **blackberry-deploy** | - | Deploy lên device |

### Thiết bị BlackBerry 10

- Bật Developer Mode: **Settings > App Manager > Development Mode**
- Ghi lại IP address và password hiển thị trong Development Mode

---

## 3. Cài đặt BB10 NDK (QNX Toolchain)

BB10 NDK toolchain không còn available trên server chính thức của BlackBerry. Phải dùng archive từ [archive.org](https://archive.org).

### 3.1. Tải Momentics IDE

Momentics IDE là Eclipse-based IDE chứa SDK installer. Bản này chỉ cần để có cấu trúc thư mục và `sdkinstall.bat`.

```powershell
# Tải Momentics IDE 2.1.2 cho Windows
curl -L -o momentics-setup.exe `
  "https://archive.org/download/bbdevtools/momentics-2.1.2-201503050937.win32.x86_64.setup_2.exe"
```

Chạy installer, cài vào `C:\bbndk`.

### 3.2. Tải standalone SDK files

SDK toolchain cần 4 file ZIP, tải từ `archive.org/download/bbdevtools/`:

```powershell
# Tạo thư mục download
mkdir C:\bbndk-downloads

# Tools: chứa qcc.exe, blackberry-nativepackager.exe, bbndk-env scripts
curl -L -o C:\bbndk-downloads\tools.zip `
  "https://archive.org/download/bbdevtools/bbndk.win32.tools.10.3.1.12.zip"

# Libraries: chứa headers, .a, .so (bps, screen, freetype, icu, ...)
curl -L -o C:\bbndk-downloads\libraries.zip `
  "https://archive.org/download/bbdevtools/bbndk.win32.libraries.10.3.1.995.zip"

# CSHost: host-side runtime
curl -L -o C:\bbndk-downloads\cshost.zip `
  "https://archive.org/download/bbdevtools/bbndk.win32.cshost.10.3.1.995.zip"

# QConfig: build configuration files
curl -L -o C:\bbndk-downloads\qconfigmk.zip `
  "https://archive.org/download/bbdevtools/bbndk.win32.qconfigmk.10.3.1.995.zip"
```

### 3.3. Giải nén vào thư mục BBNDK

Giải nén tất cả vào `C:\bbndk` (nếu đã cài Momentics IDE ở bước 3.1 thì extract đè lên).

```powershell
# Giải nén từng file
Expand-Archive -Path C:\bbndk-downloads\tools.zip -DestinationPath C:\bbndk -Force
Expand-Archive -Path C:\bbndk-downloads\libraries.zip -DestinationPath C:\bbndk -Force
Expand-Archive -Path C:\bbndk-downloads\cshost.zip -DestinationPath C:\bbndk -Force
Expand-Archive -Path C:\bbndk-downloads\qconfigmk.zip -DestinationPath C:\bbndk -Force
```

### 3.4. Xác minh cài đặt

Kiểm tra cấu trúc thư mục sau khi cài:

```
C:\bbndk\
├── host_10_3_1_12\
│   └── win32\x86\
│       ├── etc\qcc\              # QNX compiler configs
│       └── usr\
│           ├── bin\              # qcc.exe, blackberry-nativepackager.bat, ...
│           └── lib\              # GCC libraries, JAR files
└── target_10_3_1_995\
    └── qnx6\
        ├── armle-v7\
        │   ├── lib\              # Target ARM libraries (libbps, libscreen, ...)
        │   └── usr\lib\          # Additional ARM libraries
        └── usr\include\          # Target headers (bps, screen, freetype2, ...)
```

Verify:

```bash
# MSYS2 terminal
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"

# Kiểm tra qcc
qcc -V
# Output phải có: 4.6.3,gcc_ntoarmv7le

# Kiểm tra nativepackager
which blackberry-nativepackager
```

---

## 4. Cấu hình biến môi trường

### 4.1. Biến bắt buộc

| Biến | Giá trị | Mô tả |
|------|---------|-------|
| `QNX_HOST` | `C:\bbndk\host_10_3_1_12\win32\x86` | Đường dẫn host toolchain |
| `QNX_TARGET` | `C:\bbndk\target_10_3_1_995\qnx6` | Đường dẫn target SDK |
| `PATH` | Thêm `%QNX_HOST%\usr\bin` | Chứa `qcc.exe`, `blackberry-nativepackager.bat` |

### 4.2. Thiết lập trong MSYS2 (tạm thời)

```bash
# Chạy mỗi khi mở MSYS2 terminal mới
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"
```

### 4.3. Thiết lập vĩnh viễn (Windows)

Thêm vào System Environment Variables:

```
QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86
QNX_TARGET=C:\bbndk\target_10_3_1_995\qnx6
PATH=...;C:\bbndk\host_10_3_1_12\win32\x86\usr\bin
```

Hoặc thêm vào `~/.bashrc` của MSYS2:

```bash
echo 'export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"' >> ~/.bashrc
echo 'export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"' >> ~/.bashrc
echo 'export PATH="$QNX_HOST/usr/bin:$PATH"' >> ~/.bashrc
```

---

## 5. Build project

### 5.1. Clone source code

```bash
git clone --recursive https://github.com/bechovang/Term50-BB10-terminal.git
cd Term50-BB10-terminal
```

Flag `--recursive` để clone các submodules: SDL, TouchControlOverlay, libconfig.

### 5.2. Build binary

```bash
# Thiết lập môi trường (nếu chưa set vĩnh viễn)
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"

# Compile
make clean
make
```

Kết quả: `Device-Debug/Term49` (ARM binary, ~278KB)

### 5.3. Build flags

Makefile có 2 chế độ build, chỉnh trong `Makefile`:

```makefile
# Debug (mặc định)
DEBUGFLAGS := -O0 -g -DDEBUGMSGS

# Release (tối ưu)
DEBUGFLAGS := -O2
```

### 5.4. Chi tiết compiler flags

```
qcc -V4.6.3,gcc_ntoarmv7le     # Target ARM, GCC 4.6.3
     -Wc,-std=gnu99             # C99 standard (GNU extensions)
     -D_FORTIFY_SOURCE=2        # Buffer overflow protection
     -D__PLAYBOOK__             # BlackBerry platform define
     -fstack-protector-strong   # Stack canary protection
```

### 5.5. Libraries được link

**System libraries** (từ QNX_TARGET):
- `-lbps` — BlackBerry Platform Services
- `-licui18n -licuuc` — ICU Unicode support
- `-lscreen` — Screen rendering
- `-lfreetype` — Font rendering
- `-lclipboard` — Clipboard access
- `-lm` — Math

**Bundled libraries** (từ `external/lib/`):
- `-lSDL12` — SDL 1.2 (custom build)
- `-lTouchControlOverlay` — Touch input overlay
- `-lconfig` — libconfig (file config parser)

---

## 6. Đóng gói file .bar

### 6.1. Package dev-mode (không cần signing)

```bash
# Từ MSYS2 terminal
cd /path/to/Term50-BB10-terminal
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& ^
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat ^
  -package Term49.bar bar-descriptor.xml -devMode"
```

Output: `Term49.bar` (~3MB)

### 6.2. Package release (cần signing)

```bash
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& ^
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat ^
  -package Term49.bar bar-descriptor.xml"
```

### 6.3. bar-descriptor.xml

File mô tả ứng dụng BB10, các field quan trọng:

```xml
<qnx xmlns="http://www.qnx.com/schemas/application/1.0">
    <id>com.example.Term49</id>
    <name>Term49</name>
    <versionNumber>1.0.0</versionNumber>
    <author>Term50</author>

    <!-- Binary -->
    <configuration name="Device-Debug">
        <platformArchitecture>armle-v7</platformArchitecture>
        <asset path="Device-Debug/Term49" entry="true" type="Qnx/Elf">Term49</asset>
    </configuration>

    <!-- Assets -->
    <asset path="share/terminfo">terminfo</asset>
    <asset path="share/root">root</asset>           <!-- mksh, ssh, scp, sftp -->
    <asset path="external/lib/libconfig.so" type="Qnx/Elf">lib/libconfig.so.11</asset>
    <asset path="external/lib/libSDL12.so" type="Qnx/Elf">lib/libSDL12.so</asset>
    <asset path="external/lib/libTouchControlOverlay.so" type="Qnx/Elf">lib/libTouchControlOverlay.so</asset>

    <!-- Permissions -->
    <permission system="true">run_native</permission>
    <permission>run_when_backgrounded</permission>
    <permission system="true">_sys_headless_nostop</permission>
    <permission>access_shared</permission>
    <permission>access_internet</permission>
    <!-- ... -->

    <!-- Library path -->
    <env var="LD_LIBRARY_PATH" value="app/native/lib"/>
</qnx>
```

---

## 7. Deploy lên thiết bị

### 7.1. Chuẩn bị device

1. Bật **Development Mode**: Settings > App Manager > Development Mode = ON
2. Kết nối USB hoặc WiFi
3. Ghi lại **Device IP** và **Device Password** từ màn hình Development Mode

### 7.2. Cấu hình signing/bbpass

Chỉnh file `signing/bbpass`:

```makefile
BBPIN  := YOUR_DEVICE_PIN     # ví dụ: BLACKBRY
BBPASS := YOUR_DEVICE_PASSWORD # ví dụ: 1234
BBIP   := 169.254.0.1         # USB: 169.254.0.1, WiFi: IP của device
```

### 7.3. Deploy qua USB

```bash
# Từ MSYS2, sử dụng blackberry-deploy
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& ^
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-deploy ^
  -installApp 169.254.0.1 -password YOUR_PASSWORD Term49.bar"
```

### 7.4. Deploy qua Sachesi (GUI)

Nếu không muốn dùng command line:
1. Tải [Sachesi](https://github.com/xsacha/Sachesi)
2. Kết nối BB10 qua USB
3. Tab **Install** > chọn file `Term49.bar` > Install

### 7.5. Debug Token (cần cho dev-mode .bar)

File .bar build với `-devMode` cần debug token trên device.

**Tạo debug token:**

1. Lấy `bbidtoken.csk` từ: https://developer.blackberry.com/codesigning/
2. Copy vào `signing/bbidtoken.csk`
3. Chỉnh `signing/bbpass`:
   ```
   CNNAME       := "Your Name"
   KEYSTOREPASS := "your_csk_password"
   BBPIN        := YOUR_DEVICE_PIN
   BBPASS       := YOUR_DEVICE_PASSWORD
   ```
4. Chạy: `cd signing && make debugtoken`
5. Deploy token: `cd signing && make deploy-token`
6. Copy token: `cp signing/debugtoken.bar signing/debugtoken.bar`

---

## 8. Signing (phát hành chính thức)

Để phân phối file .bar cho người khác cài (không cần debug token):

1. Lấy signing certificate từ BlackBerry
2. Điền thông tin signing vào `signing/bbpass`
3. Build và sign:

```bash
make package-release
make sign
```

Lệnh `make sign` chạy `blackberry-signer` để ký file .bar bằng certificate.

---

## 9. Debug với GDB

### 9.1. Terminal 1: blackberry-connect

```bash
cd signing
make ssh-key    # Chỉ chạy lần đầu
make connect    # Giữ terminal mở
```

### 9.2. Terminal 2: GDB

```bash
make launch-debug
# Sẽ tự động: build > deploy > launch stopped > attach GDB
# Trong GDB prompt:
(gdb) continue   # Chạy tiếp
(gdb) break main # Đặt breakpoint
```

---

## 10. Cấu trúc dự án

```
Term50-BB10-terminal/
│
├── src/                          # Source code chính
│   ├── main.c                    # Entry point, SDL init, event loop, rendering
│   ├── buffer.c                  # Terminal buffer, scroll, cursor, char ops
│   ├── ecma48.c                  # ECMA-48 parser (ANSI escape sequences)
│   ├── io.c                      # PTY I/O, Unicode conversion
│   ├── preferences.c             # Config file parser (.term49rc)
│   ├── symmenu.c                 # Symbol keyboard overlay
│   ├── accent_menus.c            # Accent character menus
│   ├── SDL_ttf.c                 # SDL TTF font rendering (inline)
│   ├── terminal.h                # Main header, colors, constants
│   ├── buffer.h                  # Buffer structures, scroll functions
│   ├── ecma48.h                  # ECMA-48 function declarations
│   ├── preferences.h             # Config defaults, font size table
│   ├── colors.h                  # 256-color palette
│   ├── types.h                   # Core types (keymap_t, pref_t, symmenu_t)
│   ├── io.h                      # I/O function declarations
│   ├── accent_menus.h            # Accent menu declarations
│   └── symmenu.h                 # Symmenu declarations
│
├── external/                     # External dependencies (prebuilt .so)
│   ├── include/                  # Headers: SDL, libconfig, TouchControlOverlay
│   └── lib/                      # Shared libs: libSDL12.so, libconfig.so, libTouchControlOverlay.so
│
├── share/                        # Application assets
│   ├── root/                     # Shell + SSH binaries
│   │   ├── bin/                  # mksh, ssh, scp, sftp, ssh-keygen
│   │   └── libexec/              # scp, sftp helpers
│   ├── terminfo/                 # Terminal descriptions (uniterm49, xterm-256color, ...)
│   ├── NOTICE                    # License notices
│   ├── LICENSE                   # Apache 2.0
│   ├── README                    # User-facing readme
│   └── README45                  # Term45 readme (legacy)
│
├── icons/                        # App icons (86x86 to 480x480)
├── scripts/                      # GDB debug setup script
├── signing/                      # Code signing config + debug tokens
│   ├── bbpass                    # Signing credentials (KHÔNG commit)
│   ├── Makefile                  # Debug token + SSH key generation
│   └── debugtoken.bar            # Debug token (KHÔNG commit)
│
├── docker/                       # Docker build environment
├── SDL/                          # Git submodule: SDL 1.2 (term48 branch)
├── TouchControlOverlay/          # Git submodule: BB10 touch overlay
├── libconfig/                    # Git submodule: libconfig
│
├── Makefile                      # Build system
├── bar-descriptor.xml            # BB10 app manifest
├── .gitmodules                   # Git submodules
├── .gitignore                    # Ignored files (*.bar, Device-*, signing secrets)
└── README.md                     # Project overview
```

---

## 11. Kiến trúc source code

### 11.1. Luồng hoạt động chính

```
┌─────────────────────────────────────────────────┐
│                   main.c                         │
│                                                  │
│  SDL_Init → init() → Main Loop:                 │
│    ├── SDL Events (keyboard, touch)              │
│    │     ├── handleKeyboardEvent()               │
│    │     │     ├── Metamode processing           │
│    │     │     ├── Symmenu toggle                │
│    │     │     └── PTY input via io.c            │
│    │     └── Touch events                        │
│    │           └── Symmenu key selection          │
│    │                                              │
│    ├── PTY Read → ecma48_parse()                 │
│    │     └── Buffer updates (buffer.c)           │
│    │                                              │
│    └── render()                                  │
│          ├── Terminal text (buffer → SDL surfaces)│
│          ├── Cursor rendering                     │
│          ├── Symmenu overlay                      │
│          ├── Macro menu                           │
│          └── UI indicators (scroll, paste, accent)│
│                                                   │
│  → SDL_Flip() → Screen update                    │
└───────────────────────────────────────────────────┘
```

### 11.2. Module descriptions

#### `main.c` - Entry point và main loop
- `init()`: Khởi tạo SDL, font, PTY, buffer, preferences
- `handleKeyboardEvent()`: Xử lý phím, metamode, symmenu, macro
- `render()`: Render toàn bộ màn hình terminal + UI overlays
- `setup_screen_size()`: Tính toán font size cho screen dimensions

#### `buffer.c` - Terminal buffer
- Text buffer 2D: `struct screenchar` (char + style + rendered surface)
- Scrollback buffer (TEXT_BUFFER_SIZE lines)
- Scroll regions (ANSI scroll region support)
- Tab stops (horizontal + vertical)
- Character insert/delete operations
- Dual buffer system (NUM_BUFFERS = 2, cho alternate screen)

#### `ecma48.c` - Terminal escape sequence parser
- Parses ECMA-48 control sequences (CSI, OSC, etc.)
- VT100/VT220 compatibility
- xterm-256color support
- Handles: cursor movement, colors, scroll regions, screen clear, etc.

#### `io.c` - Input/Output
- PTY (pseudo-terminal) management
- Unicode UTF-8 ↔ UChar conversion
- Shell process spawning (fork + exec mksh)
- Read/write from PTY file descriptors

#### `preferences.c` - Configuration
- Parse `.term49rc` config file (libconfig format)
- Default values for all settings
- Keymap management (symmenu, metamode, macros)
- Font fallback paths
- Save preferences back to file

#### `symmenu.c` - Symbol menu overlay
- Render keyboard overlay với symbols
- Touch hitbox detection
- Multiple rows of symbol keys

#### `accent_menus.c` - Accent menus
- Hold-to-accent cho vowels (á, à, ả, ã, ạ, etc.)
- Per-character accent maps

### 11.3. Key data structures

```c
// Terminal character
struct screenchar {
    UChar c;                    // Unicode codepoint
    struct font_style style;    // FG/BG color, bold, reverse
    SDL_Surface* surface;       // Pre-rendered glyph
};

// Font style
struct font_style {
    SDL_Color fg_color;
    SDL_Color bg_color;
    int style;                  // Bold, underline, etc.
    char reverse;               // Reverse video
};

// Key mapping
typedef struct _keymap_t {
    char from;                  // Trigger key
    char *label;                // Display label (nullable)
    char *to;                   // Output sequence
} keymap_t;

// Preferences
typedef struct _pref_t {
    char *font_path;
    char **fallback_font_paths;  // Japanese fallback
    int font_size;
    int *text_color, *background_color;
    keymap_t *metamode_keys, *metamode_func_keys;
    keymap_t *project_macros;
    symmenu_t *main_symmenu;
    symmenu_t *accent_menus[26][2];
    // ... nhiều field khác
} pref_t;

// Symbol menu
typedef struct _symmenu_t {
    symkey_t **keys;             // 2D grid of keys
    keymap_t *entries;           // Key mappings
    SDL_Surface *surface;        // Rendered surface
} symmenu_t;
```

---

## 12. Config file (.term49rc)

File config được đặt tại home directory trên device: `~/.term49rc`

Format: [libconfig](http://www.hyperrealm.com/libconfig/)

```cfg
# Font
font_path = "/usr/fonts/font_repository/monotype/andalemo.ttf";
fallback_font_paths = ["/path/to/japanese-font.ttf"];
font_size = 24;

# Colors
text_color = [255, 255, 255];
background_color = [0, 0, 0];

# Terminal
tty_encoding = "UTF-8";
auto_show_vkb = 1;           # Auto-show virtual keyboard
screen_idle_awake = 0;       # Keep screen on
background_keepalive = 1;    # Keep shell alive when minimized

# Keyboard
sticky_shift_key = 1;        # Double-tap shift = caps lock
sticky_alt_key = 1;          # Double-tap alt = alt lock
keyhold_actions = 1;         # Hold key for accents
metamode_hold_key = 32;      # Space key = metamode (hold)
metamode_doubletap_key = 60; # Right shift = metamode (double-tap)
metamode_doubletap_delay = 500000000;  # ns

# Macros
project_macros : (
    ("s", "SSH LAN", "ssh user@192.168.1.6\n"),
    ("x", "Example", "echo hello\n")
);

# Symbol menu entries
symmenu_entries : (
    {"q", NULL, "~"}, {"w", NULL, "`"}, {"e", NULL, "{"},
    # ... thêm entries
);
```

### 12.1. Metamode keys (mặc định)

| Key | Action | Mô tả |
|-----|--------|-------|
| `e` | ESC | Gửi Escape |
| `t` | TAB | Gửi Tab |
| `a` | ALT | Alt modifier |
| `c` | CTRL | Ctrl modifier |
| `s` | Rescreen | Refresh màn hình |
| `v` | Paste | Paste clipboard |
| `m` | Macro menu | Mở macro menu |
| `u` | Scroll up | Nửa trang lên |
| `d` | Scroll down | Nửa trang xuống |
| `b` | Page up | Full trang lên |
| `f` | Page down | Full trang xuống |
| `g` | Top | Nhảy lên đầu scrollback |
| `k/j/h/l` | Arrow keys | Vim-style navigation |

### 12.2. Sticky navigation keys

Khi metamode active, `k/j/h/l` thành sticky arrow keys:
- Nhấn metamode + `k` = Up arrow
- Nhấn metamode + `j` = Down arrow
- Nhấn metamode + `h` = Left arrow
- Nhấn metamode + `l` = Right arrow

---

## 13. Build với Docker (Linux)

Nếu không có Windows, có thể build bằng Docker.

### 13.1. Dockerfile

Tạo `docker/Dockerfile`:

```dockerfile
FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential curl make ca-certificates \
        libx11-6:i386 libxext6:i386 libncurses5:i386 \
        libstdc++6:i386 zlib1g:i386 && \
    rm -rf /var/lib/apt/lists/*

# Download + extract Momentics IDE
RUN curl -sL -o /tmp/momentics.run \
    "https://archive.org/download/native-SDK-for-blackberry10/Momentics%20IDE/momentics-2.1.2-201503050937.linux.x86_64.run" \
    && chmod +x /tmp/momentics.run \
    && echo "y" | /tmp/momentics.run --target /opt/bbndk \
    && rm /tmp/momentics.run

# Install SDK (nếu sdkinstall hoạt động)
RUN cd /opt/bbndk && ./sdkinstall --install 10.3.1.995 || true

RUN echo 'source /opt/bbndk/bbndk-env_*.sh 2>/dev/null || true' >> ~/.bashrc

WORKDIR /build
```

**Lưu ý:** `sdkinstall` yêu cầu server đã tắt. Phải tải standalone SDK ZIP từ `archive.org/download/bbdevtools/` và extract thủ công.

### 13.2. Build image và chạy

```bash
docker build -t bb10-builder docker/
docker run --rm -v $(pwd):/build bb10-builder \
    bash -c "source /opt/bbndk/bbndk-env_*.sh; cd /build && make && \
    blackberry-nativepackager -package Term49.bar bar-descriptor.xml -devMode"
```

---

## 14. Troubleshooting

### `qcc: Command not found`

**Nguyên nhân:** Biến môi trường chưa được set đúng.

**Fix:**
```bash
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"
```

Verify: `qcc -V` phải hiển thị `4.6.3,gcc_ntoarmv7le`.

### `SDL_MapRGB: expected expression before '{' token`

**Nguyên nhân:** GCC 4.6.3 không hỗ trợ designated initializer trong expression context (`UI_SCROLL.r`).

**Fix:** Thay bằng giá trị RGB trực tiếp:
```c
// Sai (GCC 4.6.3)
SDL_MapRGB(screen->format, UI_SCROLL.r, UI_SCROLL.g, UI_SCROLL.b);

// Đúng
SDL_MapRGB(screen->format, 123, 202, 136);
```

### Compound literal struct initializer lỗi

**Nguyên nhân:** GCC 4.6.3 không hỗ trợ compound literal trong local variable initializer khi có non-constant expression.

**Fix:** Khởi tạo từng field:
```c
// Sai
SDL_Rect rect = {x, 0, w, h};

// Đúng
SDL_Rect rect;
rect.x = x; rect.y = 0; rect.w = w; rect.h = h;
```

### `blackberry-nativepackager: Could not find or load main class`

**Nguyên nhân:** Chạy `.bat` file từ MSYS2 bash, `%~dp0` không resolve đúng.

**Fix:** Chạy qua `cmd`:
```bash
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& \
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat \
  -package Term49.bar bar-descriptor.xml -devMode"
```

### `Invalid value 'Package-Author-Id' for attribute`

**Nguyên nhân:** `bar-descriptor.xml` chứa placeholder values.

**Fix:** Sửa `<author>` và xóa `<authorId>`:
```xml
<author>Term50</author>
<!-- Xóa dòng authorId nếu dùng devMode -->
```

### `Debug token error: place debug token in signing/debugtoken.bar`

**Nguyên nhân:** Chưa có debug token.

**Fix:** Xem section [7.5 Debug Token](#75-debug-token-cần-cho-dev-mode-bar).

### `make clean` fails: `rmdir: Device-Debug: No such file`

**Nguyên nhân:** Directory chưa tồn tại.

**Fix:** Không ảnh hưởng, ignore error. Hoặc: `mkdir -p Device-Debug` trước khi `make clean`.

### Library not found khi build

**Nguyên nhân:** Thiếu thư mục target hoặc path sai.

**Fix:** Verify `QNX_TARGET` trỏ đúng:
```bash
ls $QNX_TARGET/armle-v7/lib/libbps.so    # Phải tồn tại
ls $QNX_TARGET/usr/include/freetype2/    # Phải tồn tại
```

### Device không kết nối được

1. Kiểm tra USB connection hoặc WiFi IP
2. Development Mode phải ON
3. IP mặc định USB: `169.254.0.1`
4. Password là password hiển thị trên màn hình Development Mode

### .bar không cài được trên device

- DevMode .bar cần debug token trên device
- Release .bar cần signing certificate
- Kiểm tra version number trong `bar-descriptor.xml`

---

## Ghi chú về file .gitignore

```
*.bar              # Không commit binary packages
Device-Debug       # Build output
Device-Release     # Build output
src/*.o            # Object files
excalidraw.log     # Excalidraw editor log
signing/bbpass     # Chứa password
signing/*.csk      # Signing keys
signing/*.bar      # Debug tokens
signing/*.p12      # Keystore
signing/ssh-key*   # SSH keys
```

## Git submodules

```bash
# Clone kèm submodules
git clone --recursive https://github.com/bechovang/Term50-BB10-terminal.git

# Update submodules (nếu đã clone trước)
git submodule update --init --recursive
```

| Submodule | Source | Branch/Tag |
|-----------|--------|------------|
| SDL | github.com/mordak/SDL | `term48` branch |
| TouchControlOverlay | github.com/blackberry/TouchControlOverlay | default |
| libconfig | github.com/hyperrealm/libconfig | `f9f23d7` tag |
