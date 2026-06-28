# PicoFox — 日本ARDF向けフォーク

[English README](README.md)

## 概要

**PicoFox** は RP2040マイコンを搭載した、2mバンド（144MHz帯）FM フォックス送信機です。  
このリポジトリは [AI6YM/PicoFox](https://ai6ym.radio/picofox)（オリジナル作者: Justin / AI6YM）のフォークであり、**日本のJARL ARDF（アマチュア無線方向探知）競技**での使用を目的に改造・拡張しています。

### オリジナルからの主な変更点

| 変更項目 | 内容 |
|---------|------|
| デフォルト周波数 | 146.565 MHz → **145.660 MHz**（JARL ARDF競技周波数） |
| デフォルトITU Zone | 2（北米） → **1**（日本・欧州） |
| ARDF識別符号 | コールサイン前に **MOE〜MO5** を自動付与 |
| スロット制御 | フリーランニング → **60秒サイクル・12秒スロット** |
| 送信内容 | audio.wav → **キャリア + モールスIDのみ** |

---

## JARL ARDF規定への対応

### 60秒サイクル・スロット制御

JARL ARDF規定では5台のフォックスが60秒サイクルを分割して送信します。

| フォックス番号 | 識別符号 | 送信開始（電源投入からの経過時間） | 送信時間 |
|:---:|:---:|:---:|:---:|
| 1 | MOE | 0秒 | 12秒 |
| 2 | MOI | 12秒 | 12秒 |
| 3 | MOS | 24秒 | 12秒 |
| 4 | MOH | 36秒 | 12秒 |
| 5 | MO5 | 48秒 | 12秒 |

**同期方法**: GPSは使用せず、**全局同時電源投入**により相対的に同期します。RP2040のクリスタル精度（±20ppm）により60秒サイクルで約±1.2msの誤差が生じますが、実運用上は問題ありません。

### 送信シーケンス（各スロット内）

```
[キャリアON（無変調）]
  ↓
[モールスID送出: MOx + コールサイン]  ← 約10〜11秒
  ↓
[残り時間 キャリア継続（無変調）]
  ↓
[キャリアOFF] ← 12秒後
```

---

## 日本の使用周波数

| 周波数 | 用途 |
|--------|------|
| **145.660 MHz** | JARL ARDF競技（主要） |
| **145.780 MHz** | JARL ARDF競技（副次） |

`settings.txt` の `FREQ_MHZ` を書き換えるだけで切り替えられます。

---

## settings.txt 設定例

デバイスをPCにUSB接続するとマスストレージとして認識されます。`settings.txt` をテキストエディタで編集してください。

```ini
# コールサイン（必須。未設定だと送信しない）
CALLSIGN=JA1XXX

# ITU Zone（日本は1）
ITU_ZONE=1

# 送信周波数（MHz）
FREQ_MHZ=145.660000

# 送信デューティサイクル（ARDFスロット制御使用時は100推奨）
DUTY_CYCLE=100

# 減衰量（0=最大出力、127=最小出力）
ATTENUATION=0

# モールス速度（WPM）
MORSE_WPM=15

# Farnsworth間隔WPM（MORSE_WPMより低い値で文字間を広げる）
MORSE_FARNSWORTH_WPM=10

# モールストーン周波数（Hz）
MORSE_TONE=600

# モールストーン音量（%）
MORSE_TONE_VOL=70

# フォックス番号（1〜5）
# 1=MOE, 2=MOI, 3=MOS, 4=MOH, 5=MO5
FOX_NUMBER=1
```

設定変更後は、ドライブをイジェクトしてUSBを抜き、電源を入れ直してください。

---

## ビルド環境のセットアップ

### 必要なソフトウェア

- [Arduino IDE](https://www.arduino.cc/en/software) 2.3以上
- ボードパッケージ: [ai6ym/arduino-pico](https://github.com/ai6ym/arduino-pico)（オリジナルのフォーク）
- ライブラリ（Arduino IDE のライブラリマネージャからインストール）:
  - `Adafruit SPIFlash`
  - `Adafruit TinyUSB`
  - `Etherkit Si5351`

### ボードパッケージのインストール

Arduino IDE の `ファイル → 環境設定 → 追加のボードマネージャURL` に以下を追加：

```
https://github.com/ai6ym/arduino-pico/releases/download/global/package_rp2040_index.json
```

`ツール → ボード → ボードマネージャ` から `Raspberry Pi RP2040/RP2350 Boards` をインストール後、`AI6YM PicoFox` を選択します。

### コンパイル設定

| 設定項目 | 値 |
|---------|-----|
| Flash Size | `16MB (Sketch: 1MB, FS: 15MB)` |
| USB Stack | `Adafruit TinyUSB` |

その他の設定はデフォルト値のままで問題ありません。

---

## ファームウェアの書き込み

### 通常の書き込み

1. Arduino IDEでボードとCOMポートを選択
2. `スケッチ → マイコンボードに書き込む`

### 書き込みに失敗する場合

1. `RESET` ボタンを押してリセット後、再試行
2. `BOOTSEL` ボタンを押しながら `RESET` を押し、`BOOTSEL` を離す（BOOTSELモード）
3. BOOTSELモードで `ツール → 書き込み方法 → PicoTool` に変更して再試行
4. PicoProbe（別のPicoにPicoProbeファームウェアを書いたもの）でデバッグヘッダ経由で書き込む

### Linux でのudevルール設定（権限エラーが出る場合）

```bash
# /etc/udev/rules.d/99-pico.rules
SUBSYSTEM=="usb", ATTRS{idVendor}=="2e8a", ATTRS{idProduct}=="0003", MODE="0666"
SUBSYSTEM=="tty", ATTRS{idVendor}=="239a", ATTRS{idProduct}=="cafe", MODE="0666"
SUBSYSTEM=="usb", ATTRS{idVendor}=="239a", ATTRS{idProduct}=="cafe", MODE="0666"
```

```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```

---

## 工場出荷設定へのリセット

`settings.txt` を削除してイジェクト → USB抜去 → 電源入れ直しでデフォルト設定が再生成されます。  
**注意**: デフォルト設定ではコールサインが空のため送信しません。

---

## ライセンス

**Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0)**

- 非商用目的に限り使用・改変・再配布可能
- 改変・配布時は同ライセンスを適用し、原著作者（AI6YM）のクレジットを明記すること
- **商用利用は禁止**（商用利用の問い合わせ先: justin@ai6ym.radio）

---

## 電波法に関する注意

本機器を日本国内で使用する場合、電波法に基づく**アマチュア局免許**が必要です。  
DRA818Vモジュール等の技適未取得部品を使用する場合は、**実験試験局免許**または**特定実験試験局免許**が別途必要となります。法令を遵守して使用してください。
