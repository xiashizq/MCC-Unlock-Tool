# Halo Unlock Tool

A lightweight desktop utility for **Halo: The Master Chief Collection (MCC)** that backs up, unlocks, and restores cosmetic unlock data (armor, nameplates, and related items) by editing the local `unlockdb.xml` file.

> **Version:** 1.3  
> **Author:** Mumian Youji (木棉优纪) — [Bilibili UID 2450808](https://space.bilibili.com/2450808)  
> **Platform:** Windows · Qt 5.15+ / Qt 6  
> **Default language:** English (switchable to 简体中文)

---

## Features

- Select the MCC install directory and remember it between sessions (`config.ini`)
- Preview unlock entries in a sortable table (ID / Type / Category / State)
- **Backup** `Data/ui/unlockdb.xml` to `Data/ui/filebackup/`
- **Unlock All** — set all `Unlock` nodes to `eUnlockState_Unlocked`
- **Restore** from the local backup
- High-DPI aware UI with adaptive, low-opacity background scaling
- Multi-language UI: **English** (default) / **简体中文** (`Language` menu)

---

## Requirements

- Windows 10/11
- Steam (or other) install of **Halo: The Master Chief Collection**
- Build: Qt 5.15+ (MinGW/MSVC) with modules `core`, `gui`, `widgets`, `xml`

Typical game path:

```text
D:/Steam/steamapps/common/Halo The Master Chief Collection
```

Target file:

```text
<GameRoot>/Data/ui/unlockdb.xml
```

---

## Build

### Qt Creator

1. Open `halounlocktool.pro`
2. Configure a Qt 5.15+ or Qt 6 kit
3. Build **Release**
4. Deploy Qt runtime DLLs next to the executable (e.g. `windeployqt`)

### Command line (example)

```bash
qmake halounlocktool.pro
nmake release   # or mingw32-make / jom depending on your kit
```

### Update translations

```bash
lupdate halounlocktool.pro
# edit translations/halounlocktool_zh_CN.ts
lrelease translations/halounlocktool_zh_CN.ts -qm translations/halounlocktool_zh_CN.qm
```

---

## Usage

1. Launch the tool (UI defaults to English)
2. Optional: **Language** menu → **简体中文**
3. Click **Browse** and select the MCC root folder  
   (the folder that contains `Data/ui/unlockdb.xml`)
4. Click **Backup** before unlocking
5. Click **Unlock All** and confirm
6. If needed, click **Restore** to roll back from the backup

After unlocking, restart MCC (or return to the menu) so the game reloads unlock data.

---

## CI / Releases

GitHub Actions workflow: `.github/workflows/build.yml`

- Builds Windows x64 Release with Qt 5.15.2 (MSVC)
- Runs `windeployqt` and uploads `HaloUnlockTool-windows-x64.zip`
- Pushing a tag like `v1.3.0` also creates a GitHub Release with the zip attached

---

## Notes & Disclaimer

- Always backup first. Unlocking overwrites `unlockdb.xml`.
- This tool only modifies **local** unlock database XML on your PC.
- Use at your own risk. The author is not responsible for save/profile issues or online policy outcomes.
- For questions or feedback, contact the author via Bilibili private message.

---

## Changelog

### v1.3

- Background image uses lower opacity and scales to the window size
- Multi-language support (English default, Simplified Chinese)
- GitHub Actions workflow for Windows packaging / release artifacts

### v1.2

- High-DPI / multi-monitor friendly window sizing
- Dark themed UI refresh (background image + styled controls)
- Clearer path / backup / unlock / restore flow
- Unlock only `Unlock` XML nodes; safer file I/O and path handling
- Table view polish (alternating rows, sorting, non-editable cells)
- Stop reloading XML on every keystroke; refresh on browse / edit finished

### v1.1

- Initial public feature set: browse path, backup, unlock, restore, unlock list preview
