# PicoFox — JF9SOM Fork

## 1. プロジェクト概要 / Project Overview

[AI6YM/PicoFox](https://ai6ym.radio/picofox)（作者: Justin / AI6YM）のフォーク。  
RP2040を搭載した2mバンドFMフォックス送信機を、**日本のJARL ARDF競技規定**に準拠するよう改修・拡張している。

- 運用周波数: 145.660 MHz / 145.780 MHz（JARL ARDF競技周波数）
- ITU Zone 1（144〜146 MHz）対応
- 60秒サイクル・12秒スロット制御（MOE〜MO5）
- ハードウェアSYNCによる全局同時起動

**ライセンス: CC BY-NC-SA 4.0** — 非商用限定。改変・再配布時は同ライセンスを適用し、原著作者（AI6YM.radio）のクレジットを明記すること。商用利用禁止。

---

## 2. リポジトリ構成 / Repository Layout

```
PicoFox/
├── firmware/firmware/
│   ├── firmware.ino      # メインスケッチ（ARDF制御ロジック）
│   ├── audio.h           # デフォルト音声データ（PCM）
│   ├── dra818.h/cpp      # DRA818V UARTドライバ（スタブ）
│   └── pwm_audio.h       # PWM音声出力ドライバ（スタブ）
├── kicad/                # KiCad回路図・PCBプロジェクト（オリジナル）
├── enclosure/            # 3Dプリント用エンクロージャSTL（オリジナル）
├── tools/
│   └── convert_audio.py  # WAV変換ヘルパースクリプト
├── schematic.pdf         # エクスポート済み回路図PDF（オリジナル）
├── BOM.md                # 統合部品表（日本の入手先・合計概算付き）
├── HARDWARE.md           # ピン接続表・配線注意事項
├── SCHEMATIC.md          # テキストベースASCII回路図
├── TESTING.md            # テスト手順書（RTL-SDR・tinySA・5局テスト）
├── ISSUES.md             # 今後のタスク一覧
├── README.md             # 英語README（オリジナル）
└── README.ja.md          # 日本語README（このフォーク用）
```

---

## 3. 実装済み機能 / Implemented Features

### FOX_NUMBER 設定（settings.txt）

`settings.txt` に `FOX_NUMBER=1`〜`5` を追加。  
`Settings` 構造体・`loadSettings()`・`saveDefaultSettings()` に統合済み。バリデーション（範囲外はデフォルト値1）あり。

### MOE〜MO5 ARDF識別符号

`generateMorseAudio()` の冒頭に識別符号（MOE/MOI/MOS/MOH/MO5）を自動付与。  
送出順: **無音 → 識別符号 → 文字間無音 → コールサイン**  
既存の `writeTone()` / `writeSilence()` / `getMorse()` を再利用。

### 60秒スロット制御

`audioTask()` をフリーランニングから60秒サイクル制御に置き換え。  
Fox N の送信開始 = `(N-1) × 12,000 ms`（電源投入またはSYNCパルスからの経過時間）。  
送出順: **キャリアON → モールスID → 残時間キャリア継続 → キャリアOFF（12秒後）**

### ハードウェアSYNC（GPIO 13）

`SYNC_PIN = GPIO13`（INPUT_PULLUP）。アクティブLOWパルスで `syncEpochMs` をリセットし、全局のサイクルを同時再開始。  
スロット待機中は1msポーリングで常時SYNC検出。スタートボックスから3.5mmステレオケーブルで全局にデイジーチェーン接続。  
詳細: [`HARDWARE.md §6`](HARDWARE.md) / [`SCHEMATIC.md §4`](SCHEMATIC.md)

### DRA818V UARTドライバ（スタブ）

`dra818.h` / `dra818.cpp` — UART初期化・AT+DMOCONNECT・AT+DMOSETGROUP・PTT/PDN制御を実装。  
実機未検証。`firmware.ino` への統合はStep 4（[`ISSUES.md`](ISSUES.md)）。

---

## 4. 未実装・今後の作業 / Upcoming Work

詳細は [`ISSUES.md`](ISSUES.md) を参照。

| Step | 内容 | 依存 |
|------|------|------|
| Step 4 | DRA818Vドライバを`firmware.ino`に統合 | DRA818V実機 |
| Step 5 | PWM音声出力（`pwm_audio.h`）実装、`playAudio()`内の`setFrequencyOffset()`を置き換え | Step 4完了後 |
| Step 6 | 実機統合テスト（RTL-SDR・5局同時・tinySAスプリアス） | Step 5完了後 |
| 将来 | 80m（3.5MHz）対応、GPS同期、RTCによる自動電源制御 | — |

---

## 5. ハードウェア構成 / Hardware

DRA818V構成を前提とする。オリジナルPicoFox基板（Si5351構成）は現行ファームウェアで動作する。

- ピン接続・配線注意事項: [`HARDWARE.md`](HARDWARE.md)
- ASCII回路図（ブロック図・LPF・SYNC・電源・RF経路）: [`SCHEMATIC.md`](SCHEMATIC.md)
- 部品表（日本の入手先・価格・5台合計概算）: [`BOM.md`](BOM.md)

**GPIO割り当て（主要）**

| GPIO | 用途 |
|:----:|------|
| 0 | I2C0 SDA（Si5351・BVA1761） |
| 1 | I2C0 SCL |
| 2 | AMP_EN（Si5351構成用） |
| 4 | ATTN_LE（BVA1761、r3のみ） |
| 8 | UART1 TX → DRA818V RXD |
| 9 | UART1 RX ← DRA818V TXD |
| 10 | PTT → DRA818V |
| 11 | PDN → DRA818V |
| 12 | SQ ← DRA818V（未使用） |
| 13 | **SYNC_PIN**（INPUT_PULLUP） |
| 14 | TEST_MODE_PIN（INPUT_PULLDOWN） |
| 15 | PWM Audio → LPF → DRA818V MIC |
| 22〜25 | ボードリビジョンID（ID0〜ID3） |

---

## 6. ビルド環境 / Build Environment

- Arduino IDE >= 2.3
- ボードパッケージ: [ai6ym/arduino-pico](https://github.com/ai6ym/arduino-pico) フォーク
- ボード選択: `Raspberry Pi RP2040/RP2350 Boards → AI6YM PicoFox`
- ライブラリ: `Adafruit SPIFlash` / `Adafruit TinyUSB` / `Etherkit Si5351`
- Flash Size: `16MB (Sketch: 1MB, FS: 15MB)`
- USB Stack: `Adafruit TinyUSB`

---

## 7. 開発ルール / Development Rules

### コミット規則

```
<type>(<scope>): <summary in English, max 50 chars>
```

**Types:** `feat` `fix` `refactor` `docs` `chore` `hw`  
**Scopes:** `firmware` `kicad` `enclosure` `tools` `docs`

```
feat(firmware): add FOX_NUMBER setting for ARDF slot control
fix(firmware): set default freq and ITU zone for Japan
docs: add Japanese README and BOM with Japanese suppliers
hw(kicad): add CLK1 output footprint for 3.5 MHz band
```

### 作業フロー

- **変更後は必ず差分（`git diff`）を確認してからコミット**
- コミットは機能・目的ごとに1つ（Step単位が目安）
- プッシュは確認後に明示的に実施

### コーディング規則

- **コードコメント**: 英語のみ
- **ドキュメント（.md）**: 日本語・英語併記
- **スタイル**: オリジナルに準拠（`camelCase` 変数、`ALL_CAPS` マクロ、2スペースインデント）
- **リンター**: 不要（Arduinoプロジェクトのため `ruff` 等は使用しない）
- 新機能は `firmware.ino` に直接追加するか、`dra818.h` のように別ヘッダに分離してから `#include` する
