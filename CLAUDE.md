# PicoFox — JF9SOM Fork

## プロジェクト概要 / Project Overview

[AI6YM/PicoFox](https://ai6ym.radio/picofox) のフォーク。
日本のARDF（アマチュア無線方向探知）競技向けに改造・拡張することを目的とする。

This is a fork of the original PicoFox 2m FM fox transmitter by AI6YM,
adapted for use in Japanese ARDF (Amateur Radio Direction Finding) competitions.

## 開発目的 / Development Goals

- 日本 ITU Zone 1 (144–146 MHz) でのARDF競技運用
- デュアルバンド対応予定（144 MHz + 3.5 MHz）
- JARL ARDF競技規則への準拠

## リポジトリ構成 / Repository Layout

| Path | Contents |
|------|----------|
| `firmware/firmware/firmware.ino` | Main Arduino sketch (C++) |
| `firmware/firmware/audio.h` | Default audio data (PCM) |
| `kicad/` | KiCad schematic & PCB project |
| `schematic.pdf` | Exported schematic PDF |
| `enclosure/` | 3D-printable enclosure STLs |
| `tools/convert_audio.py` | WAV conversion helper |

回路図は `schematic.pdf` または KiCad (`kicad/kicad.kicad_sch`) を参照。
BOMはKiCadの「Generate BOM」から生成する（独立ファイルなし）。

## Build Environment

- Arduino IDE >= 2.3
- Board package: [ai6ym/arduino-pico](https://github.com/ai6ym/arduino-pico) fork
- Board: `Raspberry Pi RP2040/RP2350 Boards → AI6YM PicoFox`
- Libraries: Adafruit SPIFlash, Adafruit TinyUSB, Etherkit Si5351
- Flash Size: `16MB (Sketch: 1MB, FS: 15MB)`
- USB Stack: `Adafruit TinyUSB`

## 日本ARDF向け変更点 / Japan ARDF Changes

オリジナルからの主な変更・変更予定箇所：

| File | Location | Change |
|------|----------|--------|
| `firmware/firmware/firmware.ino` | `DEFAULT_ITU_ZONE` (line 97) | `2` → `1` |
| `firmware/firmware/firmware.ino` | `DEFAULT_FREQ_MHZ` (line 96) | `146.565` → 144 MHz帯内の値（例: `144.500`） |

`ITU_ZONE=1` を設定すると上限が 146 MHz にハードロックされる（ロジック実装済み）。
デュアルバンド化は SI5351 の CLK1/CLK2 出力と追加フロントエンド回路が必要。

## ライセンス / License

**CC BY-NC-SA 4.0** — 非商用限定。改変・再配布時は同ライセンスを適用すること。
原著作者: Giorgi Enterprises LLC dba AI6YM.radio

## コミット規則 / Commit Convention

```
<type>(<scope>): <summary in English, max 50 chars>
```

**Types:** `feat` `fix` `refactor` `docs` `chore` `hw`

**Scopes:** `firmware` `kicad` `enclosure` `tools` `docs`

Examples:
```
feat(firmware): add ITU zone 1 default for Japan ARDF
fix(firmware): clamp default freq to zone 1 range
hw(kicad): add CLK1 output footprint for 3.5 MHz band
docs: add Japanese ARDF setup instructions
```

## コーディング規則 / Coding Conventions

- **コードコメント**: 英語のみ
- **ドキュメント（README, CLAUDE.md等）**: 日本語・英語併記
- Arduino/C++ スタイルはオリジナルに準拠（`camelCase` 変数、`ALL_CAPS` マクロ）
