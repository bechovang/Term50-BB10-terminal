# Term50 — Terminal Emulator for BlackBerry 10

Term50 là terminal emulator cho BlackBerry 10, phát triển tiếp từ [Term48](https://github.com/mordak/Term48) bởi [mordak](https://github.com/mordak) và [Term49](https://github.com/BerryFarm/Term49) bởi BerryFarm.

Ứng dụng implement chuẩn [ECMA-48](http://www.ecma-international.org/publications/standards/Ecma-048.htm) và tương thích `xterm-256color`. Yêu cầu BlackBerry 10 OS >= 10.3.

---

## Tính năng

| Tính năng | Mô tả |
|-----------|-------|
| **Terminal emulator** | ECMA-48 / VT100 / VT220, 256 màu |
| **Japanese font fallback** | Hiển thị kana/kanji qua fallback fonts mà không cần thay font chính |
| **Scrollback** | Duyệt lịch sử output bằng phím (half-page, full-page, top/bottom) |
| **Symbol menu (Sym)** | Overlay bàn phím symbol tùy chỉnh, hỗ trợ touch |
| **Accent menus** | Hold phím vowel để nhập ký tự có dấu (á, à, ả, ã, ạ...) |
| **Altsym indicator** | Hiển thị trạng thái alt/sym trên màn hình |
| **Config-driven macros** | Chỉnh macro từ file config, không cần rebuild |
| **Background keep-alive** | Shell session tiếp tục chạy khi app bị minimise |
| **SSH/SCP/SFTP** | Bundled trong app, sẵn sàng dùng |
| **Keyboard-first** | Tối ưu cho physical keyboard BlackBerry 10 |

---

## Cài đặt lên thiết bị

### Yêu cầu
- BlackBerry 10 device (OS >= 10.3)
- Development Mode bật (Settings > App Manager > Development Mode)

### Cài qua Sachesi (khuyên dùng)
1. Tải [Sachesi](https://github.com/xsacha/Sachesi)
2. Kết nối BB10 qua USB
3. Tab **Install** > chọn file `.bar` > Install

### Cài qua command line
```bash
blackberry-deploy -installApp 169.254.0.1 -password <device_password> Term49.bar
```

---

## Cấu hình (.term49rc)

File config tại `~/.term49rc` trên device. Format: [libconfig](http://www.hyperrealm.com/libconfig/).

### Font
```cfg
font_path = "/usr/fonts/font_repository/monotype/andalemo.ttf";
fallback_font_paths = ["/path/to/japanese-font.ttf"];
font_size = 24;
```

### Màu sắc
```cfg
text_color = [255, 255, 255];
background_color = [0, 0, 0];
```

### Terminal
```cfg
tty_encoding = "UTF-8";
auto_show_vkb = 1;
screen_idle_awake = 0;
background_keepalive = 1;
```

### Bàn phím
```cfg
sticky_shift_key = 1;        # Double-tap shift = caps lock
sticky_alt_key = 1;          # Double-tap alt = alt lock
keyhold_actions = 1;         # Hold key cho accents
metamode_hold_key = 32;      # Space = metamode (hold)
metamode_doubletap_key = 60; # Right shift = metamode (double-tap)
```

### Macros
```cfg
project_macros : (
    ("s", "SSH LAN", "ssh user@192.168.1.6\n"),
    ("x", "Example", "echo hello\n")
);
```
Mỗi entry: `trigger key`, `label hiển thị`, `text gửi đến shell`.

---

## Metamode

Kích hoạt bằng **hold Space** hoặc **double-tap Right Shift**.

| Key | Action | Mô tả |
|-----|--------|-------|
| `e` | ESC | Gửi Escape |
| `t` | TAB | Gửi Tab |
| `a` | ALT | Alt modifier (sticky) |
| `c` | CTRL | Ctrl modifier (sticky) |
| `s` | Rescreen | Refresh màn hình |
| `v` | Paste | Paste clipboard |
| `m` | Macro menu | Mở macro menu |
| `u` | Scroll up | Nửa trang lên |
| `d` | Scroll down | Nửa trang xuống |
| `b` | Page up | Full trang lên |
| `f` | Page down | Full trang xuống |
| `g` | Top | Nhảy lên đầu scrollback |
| `k` | Up | Vim-style arrow (sticky) |
| `j` | Down | Vim-style arrow (sticky) |
| `h` | Left | Vim-style arrow (sticky) |
| `l` | Right | Vim-style arrow (sticky) |

---

## UI Overlay

Màn hình terminal có các nút touch:

| Vị trí | Màu | Chức năng |
|--------|-----|-----------|
| Góc trên phải | Vàng | Paste clipboard |
| Góc dưới phải | Xanh dương | Trở về live view |
| Thanh dọc phải | Xanh lá | Scrollback indicator (chỉ hiện khi đang xem history) |

---

## Phát triển

Hướng dẫn đầy tiết đầy đủ: **[DEVELOPMENT.md](DEVELOPMENT.md)**

### Quick start (Windows)

**Yêu cầu:**
- Windows 10/11 (64-bit)
- Java JDK 8
- MSYS2 ([msys2.org](https://www.msys2.org/))

**1. Cài BB10 NDK:**

Tải toolchain từ archive.org và extract vào `C:\bbndk`:
```powershell
# Tải standalone SDK files từ archive.org/download/bbdevtools/
# - bbndk.win32.tools.10.3.1.12.zip     (qcc, nativepackager)
# - bbndk.win32.libraries.10.3.1.995.zip (headers, libs)
# - bbndk.win32.cshost.10.3.1.995.zip
# - bbndk.win32.qconfigmk.10.3.1.995.zip
Expand-Archive -Path tools.zip -DestinationPath C:\bbndk -Force
Expand-Archive -Path libraries.zip -DestinationPath C:\bbndk -Force
Expand-Archive -Path cshost.zip -DestinationPath C:\bbndk -Force
Expand-Archive -Path qconfigmk.zip -DestinationPath C:\bbndk -Force
```

**2. Set môi trường:**
```bash
# MSYS2 terminal
export QNX_HOST="/c/bbndk/host_10_3_1_12/win32/x86"
export QNX_TARGET="/c/bbndk/target_10_3_1_995/qnx6"
export PATH="$QNX_HOST/usr/bin:$PATH"
```

**3. Build:**
```bash
git clone --recursive https://github.com/bechovang/Term50-BB10-terminal.git
cd Term50-BB10-terminal
make
```

**4. Package:**
```bash
cmd //c "set QNX_HOST=C:\bbndk\host_10_3_1_12\win32\x86&& ^
  C:\bbndk\host_10_3_1_12\win32\x86\usr\bin\blackberry-nativepackager.bat ^
  -package Term49.bar bar-descriptor.xml -devMode"
```

### Submodules

| Submodule | Source | Ghi chú |
|-----------|--------|---------|
| SDL | [mordak/SDL](https://github.com/mordak/SDL) (term48 branch) | Phải define `-D__PLAYBOOK__ -DRAW_KEYBOARD_EVENTS` |
| TouchControlOverlay | [blackberry/TouchControlOverlay](https://github.com/blackberry/TouchControlOverlay) | Touch input overlay |
| libconfig | [hyperrealm/libconfig](https://github.com/hyperrealm/libconfig) (tag f9f23d7) | Config file parser |

Prebuilt `.so` files có sẵn trong `external/lib/`, chỉ cần build từ submodule khi muốn thay đổi.

### Deploy & Debug

Chi tiết đầy đủ về debug token, signing, GDB debugging xem trong **[DEVELOPMENT.md](DEVELOPMENT.md)**.

---

## Cấu trúc dự án

```
src/
  main.c            # Entry point, SDL init, event loop, rendering
  buffer.c          # Terminal buffer, scrollback, cursor ops
  ecma48.c          # ANSI escape sequence parser
  io.c              # PTY I/O, Unicode conversion
  preferences.c     # Config file parser (.term49rc)
  symmenu.c         # Symbol keyboard overlay
  accent_menus.c    # Accent character menus
  SDL_ttf.c         # SDL TTF font rendering
external/lib/       # Prebuilt: libSDL12.so, libconfig.so, libTouchControlOverlay.so
share/root/bin/     # mksh, ssh, scp, sftp, ssh-keygen
share/terminfo/     # Terminal descriptions
icons/              # App icons (86x86 → 480x480)
signing/            # Code signing config
```

---

## Credits

- [Term48](https://github.com/mordak/Term48) — Todd Mortimer (mordak)
- [Term49](https://github.com/BerryFarm/Term49) — BerryFarm
- [sizeof(cat)](https://archive.org/details/bbdevtools) — Lưu trữ BB10 NDK trên archive.org

## License

Apache License 2.0
