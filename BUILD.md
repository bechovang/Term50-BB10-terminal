# Term50 — Build Process Documentation

Tài liệu mô tả chi tiết toàn bộ quá trình compile, link, và package Term50 từ source code thành file `.bar` cài đặt được trên BlackBerry 10.

---

## Mục lục

1. [Tổng quan build pipeline](#1-tổng-quan-build-pipeline)
2. [Toolchain — QNX/BB10 NDK](#2-toolchain--qnxbb10-ndk)
3. [Biến môi trường](#3-biến-môi-trường)
4. [Phân tích Makefile chi tiết](#4-phân-tích-makefile-chi-tiết)
5. [Quá trình Compile — từng bước](#5-quá-trình-compile--từng-bước)
6. [Quá trình Link](#6-quá-trình-link)
7. [Package .bar — blackberry-nativepackager](#7-package-bar--blackberry-nativepackager)
8. [bar-descriptor.xml — manifest chi tiết](#8-bar-descriptorxml--manifest-chi-tiết)
9. [Kết quả build](#9-kết-quả-build)
10. [Lỗi thường gặp khi build](#10-lỗi-thường-gặp-khi-build)

---

## 1. Tổng quan build pipeline

```
┌──────────────────────────────────────────────────────────────────┐
│                     BUILD PIPELINE TERM50                        │
│                                                                  │
│  src/*.c ──→ [qcc compile] ──→ src/*.o ──→ [qcc link] ──→      │
│                                                      │           │
│                         Device-Debug/Term50  ◄────────┘           │
│                              │                                   │
│                              ▼                                   │
│              [blackberry-nativepackager]                          │
│              + bar-descriptor.xml                                 │
│              + external/lib/*.so                                  │
│              + share/* (terminfo, root, icons, ...)               │
│                              │                                   │
│                              ▼                                   │
│                        Term50.bar (~3MB)                          │
└──────────────────────────────────────────────────────────────────┘
```

3 giai đoạn chính:

| # | Giai đoạn | Tool | Input | Output |
|---|-----------|------|-------|--------|
| 1 | **Compile** | `qcc` (GCC 4.6.3) | `src/*.c` (8 files C) | `src/*.o` (8 object files ARM) |
| 2 | **Link** | `qcc` (ld) | `src/*.o` + libraries | `Device-Debug/Term50` (ARM ELF binary) |
| 3 | **Package** | `blackberry-nativepackager` (Java) | binary + assets + descriptor | `Term50.bar` (ZIP archive) |

---

## 2. Toolchain — QNX/BB10 NDK

### 2.1. Vị trí cài đặt

```
C:\bbndk\
├── host_10_3_1_12\win32\x86\          ← QNX_HOST
│   ├── etc\qcc\                        # Compiler config files
│   └── usr\
│       ├── bin\                        # qcc.exe, blackberry-*.bat, ntoarm-gdb.exe
│       └── lib\                        # GCC internal libs, JAR files (BarPackager, ...)
│
└── target_10_3_1_995\qnx6\            ← QNX_TARGET
    ├── armle-v7\
    │   ├── lib\                        # Target ARM .so/.a (libc, libm, libgcc, ...)
    │   └── usr\lib\                    # Target ARM .so/.a (libbps, libscreen, libfreetype, ...)
    └── usr\include\                    # Target headers (bps/, screen/, freetype2/, ...)
```

### 2.2. Compiler: qcc

`qcc` là wrapper của QNX xung quanh GCC, tự động thêm system include paths và lib paths dựa trên target.

```bash
qcc -V  # Liệt kê targets available
```

Targets có sẵn:

| Target | Arch | GCC | Dùng cho |
|--------|------|-----|----------|
| `4.6.3,gcc_ntoarmv7le` | ARMv7 LE | 4.6.3 | **Term50 dùng cái này** |
| `4.8.3,gcc_ntoarmv7le` | ARMv7 LE | 4.8.3 | Có sẵn nhưng không dùng |
| `4.6.3,gcc_ntox86` | x86 | 4.6.3 | Simulator (default) |

Dùng GCC 4.6.3 vì nó là version mà BB10 SDK 10.3 ship với. GCC 4.8.3 cũng available nhưng Term50 không dùng.

### 2.3. blackberry-nativepackager

Java tool để đóng gói binary + assets thành `.bar` file.

```
blackberry-nativepackager.bat
  → java -cp BarPackager.jar;BarSigner.jar;...
     com.qnx.bbt.nativepackager.BarNativePackager
```

Yêu cầu: **JDK 8** phải có sẵn trong PATH.

### 2.4. Binary format

Binary output là **QNX Neutrino (QNX 6) ELF** cho ARM:

```
ELF 32-bit LSB executable, ARM, version 1 (SYSV)
Dynamically linked (uses shared libs)
Stripped: no (debug build)
```

---

## 3. Biến môi trường

### 3.1. Biến bắt buộc

```bash
# MSYS2 / bash
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"
```

| Biến | Dùng cho | Ví dụ mở rộng |
|------|----------|----------------|
| `QNX_HOST` | Tìm `qcc.exe`, `blackberry-nativepackager.bat` | `-L$(QNX_TARGET)/armle-v7/lib` |
| `QNX_TARGET` | Tìm target headers và libraries | `-I$(QNX_TARGET)/usr/include` |
| `PATH` | Chạy `qcc`, `blackberry-nativepackager` trực tiếp | |

### 3.2. Cách Makefile dùng

Makefile truy cập biến thông qua `$(QNX_TARGET)` và `$(QNX_HOST)`:

```makefile
INCLUDE := -I$(QNX_TARGET)/usr/include                # → -IC:/bbndk/target_10_3_1_995/qnx6/usr/include
INCLUDE += -I$(QNX_TARGET)/usr/include/freetype2       # → -IC:/bbndk/.../usr/include/freetype2
LIBPATHS := -L$(QNX_TARGET)/armle-v7/lib              # → -LC:/bbndk/.../armle-v7/lib
LIBPATHS += -L$(QNX_TARGET)/armle-v7/usr/lib          # → -LC:/bbndk/.../armle-v7/usr/lib
```

Nếu không set biến → Makefile sẽ trỏ sai path → `qcc: Command not found` hoặc header not found.

---

## 4. Phân tích Makefile chi tiết

### 4.1. Compiler & flags

```makefile
CC := qcc
```

`qcc` là QNX C Compiler wrapper. Khi gọi `qcc -V4.6.3,gcc_ntoarmv7le`, nó chọn GCC 4.6.3 targeting ARM QNX Neutrino.

### 4.2. Include paths

```makefile
INCLUDE := -I$(QNX_TARGET)/usr/include          # BPS, Screen, ICU, libc, ...
INCLUDE += -I$(QNX_TARGET)/usr/include/freetype2 # FreeType headers
INCLUDE += -I./external/include                  # SDL.h, libconfig.h, TouchControlOverlay.h
```

Thứ tự resolve: `external/include` → `QNX_TARGET/usr/include` → `QNX_TARGET/usr/include/freetype2`

External headers override system headers khi có trùng tên.

### 4.3. Library paths

```makefile
LIBPATHS := -L$(QNX_TARGET)/armle-v7/lib      # libc, libm, libgcc, libicu*, ...
LIBPATHS += -L$(QNX_TARGET)/armle-v7/usr/lib   # libbps, libscreen, libfreetype, ...
LIBPATHS += -L./external/lib                    # libSDL12.so, libconfig.so, libTouchControlOverlay.so
```

### 4.4. Libraries

```makefile
# System libraries (link lúc runtime từ QNX_TARGET)
LIBS := -lbps -licui18n -licuuc -lscreen -lm -lfreetype -lclipboard
LIBS += -lconfig

# Bundled libraries (link từ external/lib, đóng gói vào .bar)
LIBS += -lconfig -lSDL12 -lTouchControlOverlay
```

| Library | Nguồn | Mô tả |
|---------|-------|-------|
| `-lbps` | System | BlackBerry Platform Services — events, screen, virtual keyboard |
| `-licui18n` | System | ICU internationalization — Unicode processing |
| `-licuuc` | System | ICU common — UChar, UTF conversion |
| `-lscreen` | System | Screen rendering API |
| `-lm` | System | Math library |
| `-lfreetype` | System | FreeType font rendering |
| `-lclipboard` | System | Clipboard access |
| `-lconfig` | System + Bundled | Config file parser |
| `-lSDL12` | Bundled | SDL 1.2 — window, events, surfaces |
| `-lTouchControlOverlay` | Bundled | Touch input overlay UI |

**Lưu ý:** `-lconfig` xuất hiện 2 lần — 1 từ system lib, 1 từ `external/lib`. Linker ưu tiên path `external/lib` trước (xuất hiện sau trong `LIBPATHS`), nên bản bundled được dùng.

### 4.5. Defines

```makefile
DEFINES := -D_FORTIFY_SOURCE=2 -D__PLAYBOOK__ -fstack-protector-strong
```

| Define | Ý nghĩa |
|--------|----------|
| `-D_FORTIFY_SOURCE=2` | Buffer overflow protection cho libc calls (memcpy, strcpy, ...) |
| `-D__PLAYBOOK__` | Kích hoạt BlackBerry-specific code paths trong SDL và app |
| `-fstack-protector-strong` | Stack canary cho functions dùng buffers/addresses |

### 4.6. Debug vs Release

```makefile
# Debug (hiện tại)
DEBUGFLAGS := -O0 -g -DDEBUGMSGS

# Release (commented out)
#DEBUGFLAGS := -O2
```

| Mode | `-O` | `-g` | `-DDEBUGMSGS` | Kết quả |
|------|------|------|---------------|----------|
| Debug | `-O0` (no optimization) | Yes (debug symbols) | Yes (fprintf stderr) | Binary ~278KB, dễ debug |
| Release | `-O2` (optimize) | No | No | Binary nhỏ hơn, nhanh hơn |

### 4.7. Linker options

```makefile
LDOPTS := -Wl,-z,relro -Wl,-z,now
```

| Option | Ý nghĩa |
|--------|----------|
| `-z,relro` | Mark GOT as read-only sau khi resolved ( Partial RELRO) |
| `-z,now` | Resolve all symbols lúc load, không lazy binding (Full RELRO) |

Security hardening — ngăn các kỹ thuật ROP/LOB.

### 4.8. CFLAGS đầy đủ (mở rộng)

Lệnh compile hoàn chỉnh cho mỗi `.c` file:

```
qcc \
  -IC:/bbndk/target_10_3_1_995/qnx6/usr/include \
  -IC:/bbndk/target_10_3_1_995/qnx6/usr/include/freetype2 \
  -I./external/include \
  -V4.6.3,gcc_ntoarmv7le \
  -Wc,-std=gnu99 \
  -O0 -g -DDEBUGMSGS \
  -c \
  -D_FORTIFY_SOURCE=2 -D__PLAYBOOK__ -fstack-protector-strong \
  src/main.c \
  -o src/main.o
```

Giải thích từng flag:

| Flag | Ý nghĩa |
|------|----------|
| `-V4.6.3,gcc_ntoarmv7le` | Chọn compiler target: GCC 4.6.3, ARM QNX Neutrino |
| `-Wc,-std=gnu99` | Pass `-std=gnu99` cho GCC C compiler (C99 + GNU extensions) |
| `-O0` | Không tối ưu — giữ nguyên structure, dễ debug |
| `-g` | Generate DWARF debug info |
| `-DDEBUGMSGS` | Enable `PRINT()` macro → `fprintf(stderr, ...)` |
| `-c` | Compile only, không link |
| `-D__PLAYBOOK__` | BlackBerry platform define |
| `-fstack-protector-strong` | Stack canary |

---

## 5. Quá trình Compile — từng bước

### 5.1. Source files (thứ tự build)

```
src/symmenu.c        → src/symmenu.o
src/preferences.c    → src/preferences.o      (warnings: pointer type mismatches)
src/buffer.c         → src/buffer.o
src/main.c           → src/main.o             (warning: incompatible pointer type)
src/SDL_ttf.c        → src/SDL_ttf.o
src/accent_menus.c   → src/accent_menus.o
src/io.c             → src/io.o
src/ecma48.c         → src/ecma48.o
```

### 5.2. Compile command cho mỗi file

Pattern rule trong Makefile:

```makefile
%.o: %.c
	$(CC) $(CFLAGS) -c $(DEFINES) $< -o $@
```

Mở rộng thành:

```bash
qcc -I... -V4.6.3,gcc_ntoarmv7le -Wc,-std=gnu99 -O0 -g -DDEBUGMSGS \
    -c -D_FORTIFY_SOURCE=2 -D__PLAYBOOK__ -fstack-protector-strong \
    src/main.c -o src/main.o
```

### 5.3. Quá trình biên dịch bên trong

```
src/main.c
    │
    ├── Preprocessor (#include, #define, #ifdef)
    │   ├── #include "SDL.h"          → external/include/SDL.h
    │   ├── #include "terminal.h"     → src/terminal.h
    │   ├── #include "buffer.h"       → src/buffer.h
    │   ├── #include "preferences.h"  → src/preferences.h
    │   ├── #include "io.h"           → src/io.h
    │   ├── #include "colors.h"       → src/colors.h
    │   └── + system headers từ QNX_TARGET
    │
    ├── Compiler (C → ARM assembly)
    │   └── GCC 4.6.3 targeting armle-v7 QNX
    │
    └── Assembler (assembly → object)
        └── src/main.o  (ARM ELF relocatable)
```

### 5.4. Warnings (không phải lỗi)

```
src/preferences.c:471: warning: incompatible pointer type
src/preferences.c:513: warning: passing argument 3 of 'config_lookup_string'
src/main.c:839:      warning: passing argument 2 of 'keystroke_lookup'
```

Đây là warnings về pointer type mismatch giữa `char**` vs `const char**` và `keymap_t*` vs `symmenu_t*`. Không ảnh hưởng runtime trên ARM 32-bit (tất cả pointer cùng size).

---

## 6. Quá trình Link

### 6.1. Link command

```makefile
$(BINARY): $(OBJS)
	mkdir -p $(ASSET)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDOPTS) $(OBJS) -o $(BINARY_PATH)
```

Mở rộng:

```bash
qcc \
  -I... -V4.6.3,gcc_ntoarmv7le -Wc,-std=gnu99 -O0 -g -DDEBUGMSGS \
  -LC:/bbndk/target_10_3_1_995/qnx6/armle-v7/lib \
  -LC:/bbndk/target_10_3_1_995/qnx6/armle-v7/usr/lib \
  -L./external/lib \
  -lbps -licui18n -licuuc -lscreen -lm -lfreetype -lclipboard \
  -lconfig -lconfig -lSDL12 -lTouchControlOverlay \
  -Wl,-z,relro -Wl,-z,now \
  src/symmenu.o src/preferences.o src/buffer.o src/main.o \
  src/SDL_ttf.o src/accent_menus.o src/io.o src/ecma48.o \
  -o Device-Debug/Term50
```

### 6.2. Linker hoạt động

```
8 object files (.o)
    │
    ├── Resolve symbols giữa các .o
    │   main.c gọi buf_init() → tìm trong buffer.o
    │   main.c gọi ecma48_parse() → tìm trong ecma48.o
    │   ecma48.c gọi buf_increment_line() → tìm trong buffer.o
    │   ...
    │
    ├── Link shared libraries (.so) — dynamic linking
    │   -lbps      → libbps.so         (không embed, chỉ ghi SONAME)
    │   -lSDL12    → libSDL12.so        (từ external/lib, bundled trong .bar)
    │   -lfreetype → libfreetype.so     (system lib trên device)
    │   ...
    │
    └── Output: Device-Debug/Term50
        ELF 32-bit ARM executable, dynamically linked
```

### 6.3. Runtime library loading

Khi chạy trên device, loader tìm `.so` theo `LD_LIBRARY_PATH`:

```
LD_LIBRARY_PATH=app/native/lib
```

Xem trong `bar-descriptor.xml`:
```xml
<env var="LD_LIBRARY_PATH" value="app/native/lib"/>
```

Libraries bundled trong `.bar` (`external/lib/*.so`) được extract vào `app/native/lib/`.
System libraries (bps, screen, freetype, icu...) có sẵn trên device OS.

### 6.4. Kết quả link

```
Device-Debug/Term50
  Size: 278,180 bytes (~278 KB)
  Format: ELF 32-bit LSB executable, ARM, version 1
  Dynamically linked, not stripped (debug symbols included)
```

---

## 7. Package .bar — blackberry-nativepackager

### 7.1. Lệnh package

```bash
blackberry-nativepackager -package Term50.bar bar-descriptor.xml -devMode
```

Thực tế chạy qua `cmd` vì `.bat` file cần Windows path expansion:

```bash
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& \
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat \
  -package Term50.bar bar-descriptor.xml -devMode"
```

### 7.2. `.bar` file là gì?

`.bar` = **ZIP archive** chứa app binary + assets + manifest. Tương tự `.apk` (Android) hay `.ipa` (iOS).

```
Term50.bar (3,064,951 bytes ≈ 3MB)
├── META-INF/
│   ├── MANIFEST.MF              # App metadata (name, author, permissions)
│   └── ...
├── Term50                       # ARM ELF binary (278KB)
├── lib/
│   ├── libconfig.so.11          # 77KB
│   ├── libSDL12.so              # 534KB
│   └── libTouchControlOverlay.so # 46KB
├── terminfo/                    # Terminal descriptions (~200KB)
├── root/
│   ├── bin/                     # mksh, ssh, scp, sftp, ssh-keygen (~2MB)
│   └── libexec/                 # scp, sftp helpers
├── term5086x86.png ... term50144x144.png  # App icons
├── NOTICE                       # License notices
├── LICENSE                      # Apache 2.0
├── README
└── README45
```

### 7.3. Quá trình đóng gói

```
blackberry-nativepackager đọc bar-descriptor.xml:
    │
    ├── 1. Parse XML — lấy danh sách assets, permissions, config
    │
    ├── 2. Validate binary (Device-Debug/Term50)
    │   └── Kiểm tra ELF format, architecture armle-v7
    │
    ├── 3. Copy assets vào archive
    │   ├── Binary → Term50
    │   ├── external/lib/*.so → lib/
    │   ├── share/terminfo → terminfo/
    │   ├── share/root → root/
    │   └── icons/*.png → ./
    │
    ├── 4. Generate META-INF/MANIFEST.MF
    │   ├── App name, id, version
    │   ├── Permissions list
    │   ├── Environment variables (LD_LIBRARY_PATH)
    │   └── Author info
    │
    └── 5. ZIP everything → Term50.bar
        └── Output: "Info: Package created: Term50.bar"
```

### 7.4. DevMode vs Release

| Mode | Flag | Cần token | Chạy trên device |
|------|------|-----------|-------------------|
| **DevMode** | `-devMode` | Cần debug token đã deploy | Chỉ device có debug token |
| **Release** | (không flag) | Cần signing certificate | Bất kỳ device nào |

### 7.5. `-devMode` flag thêm gì?

- Bỏ qua signing requirement
- Thêm `debug-token-ref` vào MANIFEST.MF
- Cho phép GDB attach
- Thêm flag `run_native` permission tự động

---

## 8. bar-descriptor.xml — manifest chi tiết

### 8.1. App identity

```xml
<id>com.example.Term49</id>     → Unique app ID, phải đổi khi fork
<name>Term50</name>             → Tên hiển thị trên device
<versionNumber>1.0.0</versionNumber>
<buildId>1</buildId>
<author>Term50</author>
```

### 8.2. Binary entry point

```xml
<configuration name="Device-Debug">
    <platformArchitecture>armle-v7</platformArchitecture>
    <asset path="Device-Debug/Term50" entry="true" type="Qnx/Elf">Term50</asset>
</configuration>
```

- `entry="true"` — file này là executable chính
- `type="Qnx/Elf"` — native binary, không phải AIR/Android
- `platformArchitecture` — phải khớp binary (`armle-v7`)

### 8.3. Bundled libraries

```xml
<asset path="external/lib/libconfig.so" type="Qnx/Elf">lib/libconfig.so.11</asset>
<asset path="external/lib/libSDL12.so" type="Qnx/Elf">lib/libSDL12.so</asset>
<asset path="external/lib/libTouchControlOverlay.so" type="Qnx/Elf">lib/libTouchControlOverlay.so</asset>
```

`path` = nguồn trên host, nội dung = đích trong `.bar`. Thư mục `lib/` khớp với `LD_LIBRARY_PATH=app/native/lib`.

### 8.4. Permissions

```xml
<permission system="true">run_native</permission>        ← Bắt buộc cho native app
<permission>run_when_backgrounded</permission>            ← Giữ app chạy khi minimise
<permission system="true">_sys_headless_nostop</permission> ← Không bị OS kill
<permission>access_shared</permission>                    ← Đọc/write shared files
<permission>access_internet</permission>                  ← Network (SSH)
<!-- ... nhiều permission khác -->
```

### 8.5. Window config

```xml
<initialWindow>
    <autoOrients>true</autoOrients>    ← Xoay theo device orientation
    <systemChrome>none</systemChrome>  ← Không có title bar, full screen
</initialWindow>
```

---

## 9. Kết quả build

### 9.1. Output files

```
Term50-BB10-terminal/
├── src/*.o                        # 8 object files (có thể xóa)
├── Device-Debug/Term50            # ARM ELF binary (278 KB)
└── Term50.bar                     # Installable package (3 MB)
```

### 9.2. Binary info

```
Device-Debug/Term50:
  Format:   ELF 32-bit LSB executable, ARM
  Arch:     armle-v7 (ARMv7 little-endian)
  OS:       QNX Neutrino 6
  Linking:  Dynamic (cần .so lúc runtime)
  Symbols:  Included (debug build)
  Size:     278,180 bytes
```

### 9.3. .bar info

```
Term50.bar:
  Format:   ZIP archive
  Contents: binary + 3 .so + terminfo + root/ + icons + docs
  Size:     3,064,951 bytes (~2.9 MB)
  Signing:  Unsigned (devMode)
```

### 9.4. Dependency chain lúc runtime

```
Term50 (binary)
  ├── libSDL12.so          (bundled, từ external/lib)
  ├── libTouchControlOverlay.so (bundled, từ external/lib)
  ├── libconfig.so.11      (bundled, từ external/lib)
  ├── libbps.so            (system, có sẵn trên BB10 OS)
  ├── libscreen.so         (system)
  ├── libfreetype.so       (system)
  ├── libicui18n.so        (system)
  ├── libicuuc.so          (system)
  ├── libclipboard.so      (system)
  ├── libc.so              (system, C runtime)
  └── libm.so              (system, math)
```

---

## 10. Lỗi thường gặp khi build

### `qcc: Command not found`

**Makefile line 47:** `$(CC) $(CFLAGS) -c $(DEFINES) $< -o $@`

**Nguyên nhân:** `PATH` không chứa `QNX_HOST/usr/bin`.

```bash
# Fix
export PATH="/c/bbndk/host_10_3_1_12/win32/x86/usr/bin:$PATH"
which qcc  # Phải tìm thấy
```

### `SDL_MapRGB: expected expression before '{' token`

**Nguyên nhân:** GCC 4.6.3 không parse designated initializer `{.r=X, .g=Y, .b=Z}` trong expression context. Macro `UI_SCROLL` mở rộng thành `{.r=123, .g=202, .b=136}` và `.r` không valid ngoài struct initializer.

```c
// LỖI với GCC 4.6.3
SDL_MapRGB(fmt, UI_SCROLL.r, UI_SCROLL.g, UI_SCROLL.b);
// mở rộng thành: SDL_MapRGB(fmt, {.r=123,...}.r, ...) ← lỗi cú pháp

// FIX: Dùng giá trị trực tiếp
SDL_MapRGB(fmt, 123, 202, 136);
```

### Compound literal struct lỗi

```c
// LỖI: non-constant initializer
SDL_Rect rect = {variable_x, 0, width, height};

// FIX: Gán từng field
SDL_Rect rect;
rect.x = variable_x;
rect.y = 0;
rect.w = width;
rect.h = height;
```

### `blackberry-nativepackager: Could not find or load main class`

**Nguyên nhân:** Chạy `.bat` từ MSYS2/bash — `%~dp0` không resolve.

```bash
# FIX: Chạy qua cmd.exe
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& \
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat \
  -package Term50.bar bar-descriptor.xml -devMode"
```

### `Invalid value 'Package-Author-Id' for attribute`

**Nguyên nhân:** `bar-descriptor.xml` chứa placeholder `<authorId>Package-Author-Id</authorId>`.

```xml
<!-- FIX: Xóa dòng authorId nếu dùng devMode -->
<author>Term50</author>
<!-- authorId removed for devMode -->
```

### `fatal error: SDL.h: No such file or directory`

**Nguyên nhân:** Thiếu `external/include/` hoặc `QNX_TARGET` sai.

```bash
# Verify
ls external/include/SDL.h              # Phải tồn tại
ls $QNX_TARGET/usr/include/bps/bps.h   # Phải tồn tại
```

### `cannot find -lbps`

**Nguyên nhân:** `QNX_TARGET` sai hoặc thiếu `armle-v7/usr/lib`.

```bash
ls $QNX_TARGET/armle-v7/usr/lib/libbps.so  # Phải tồn tại
```

### `make clean` fails với `rmdir: Device-Debug: No such file`

Không ảnh hưởng. Directory chưa tạo vì chưa build. Ignore.

### `Debug token error` khi `make package-debug`

Makefile yêu cầu `signing/debugtoken.bar` tồn tại cho target `package-debug`. 

**Fix 1:** Dùng `blackberry-nativepackager` trực tiếp với `-devMode` (không cần token file):

```bash
cmd //c "set QNX_HOST=C:\bbndk\...&& blackberry-nativepackager.bat -package Term50.bar bar-descriptor.xml -devMode"
```

**Fix 2:** Tạo debug token rỗng: `touch signing/debugtoken.bar`

---

## Appendix: Lệnh build nhanh (copy-paste)

```bash
# === MSYS2 terminal ===

# 1. Set môi trường
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"

# 2. Compile + Link
cd /c/Users/Admin/Desktop/GIT\ CLONE/Term50-BB10-terminal
rm -f src/*.o Term50.bar
make

# 3. Package .bar (devMode)
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat -package Term50.bar bar-descriptor.xml -devMode"

# 4. Verify
ls -la Device-Debug/Term50 Term50.bar
```
