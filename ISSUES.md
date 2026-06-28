# 今後のタスク / Issue Tracker

GitHub Issuesの代替として、実装予定タスクをここで管理する。  
実際のIssueはリポジトリのIssuesタブへ移行予定。

---

## 進行中 / In Progress

なし

---

## 未着手 / Open

### Step 4: DRA818V UARTドライバ実装

**概要**: `dra818.h` / `dra818.cpp` のスタブを実機で検証し、`firmware.ino` に統合する。

**作業内容**:
- [ ] DRA818V実機でAT+DMOCONNECTの動作確認
- [ ] `dra818_init()` を `setup()` に組み込む
- [ ] `setSi5351Output()` の代替として `dra818_ptt_on/off()` を接続
- [ ] Si5351初期化ブロックをコンパイル時フラグ（`#ifdef USE_DRA818`）で分岐
- [ ] 設定変更時の再初期化対応（`FREQ_MHZ` 変更 → `dra818_set_group()` 再呼び出し）

**依存**: DRA818Vモジュール到着後に着手

---

### Step 5: PWM音声出力実装

**概要**: `pwm_audio.h` のスタブを実装し、`playAudio()` 内の `setFrequencyOffset()` を `pwm_audio_write()` に置き換える。

**作業内容**:
- [ ] `pwm_audio_init()` の実装（`pwm_gpio_to_slice_num` / `pwm_set_wrap` 設定）
- [ ] `pwm_audio_write(int16_t sample)` の実装
- [ ] `playAudio()` 内の `setFrequencyOffset(deviation)` を `pwm_audio_write(sample)` に置き換え
- [ ] LPF定数の実測（オシロスコープまたはSDRで波形確認）
- [ ] `AUDIO_PWM_SCALE` 定数の調整（FM偏移が ±2.5kHz に収まるよう調整）

**依存**: Step 4完了後、DRA818V実機でのテスト必要

---

### Step 6: 実機統合テスト

**概要**: Si5351構成とDRA818V構成を切り替えてARDF競技として動作することを確認する。

**作業内容**:
- [ ] 単体動作確認（Fox 1台、RTL-SDRで受信）
- [ ] 5台同時起動でスロット確認（[TESTING.md §2](TESTING.md) 参照）
- [ ] tinySAでスプリアス測定（[TESTING.md §3](TESTING.md) 参照）
- [ ] フィールドテスト（実際の競技コースで方向探知確認）
- [ ] バッテリー持続時間の実測

**依存**: Step 4・5完了後

---

## 将来の拡張 / Future

### 80m（3.5MHz）対応

**概要**: Si5351のCLK1出力を使って3.5MHz帯のARDFフォックスとして動作させる。

**検討事項**:
- Si5351 CLK1は拡張ヘッダ（J1）経由で利用可能
- 3.5MHz帯では出力レベルが低いため外部アンプが必要（例: IRF510を使ったAM/DSBアンプ）
- JARL ARDF規定では3.5MHz帯は80m部門として独立した競技
- 変調方式: AM（3.5MHz帯はAM変調が標準）← Si5351のPLL dithering方式では困難、DAC追加を検討

**参考**: Si5351 CLK1のピン番号 → 拡張ヘッダJ1のピン配置を確認すること

---

### GPS同期対応

**概要**: 1PPSシグナルを使ってすべてのフォックスを絶対時刻で同期する。現状は電源同時投入による相対同期。

**検討事項**:
- GPSモジュール（u-blox M8N等）を拡張ヘッダ経由で接続
- 1PPSパルスで `millis()` を補正 → 60秒サイクルの絶対アライメント
- 利点: 電源投入タイミングのズレを±1ms以内に補正可能
- 欠点: コスト増（GPSモジュール ≈ 1,500〜3,000円）・GPS取得まで送信遅延

---

### RTCによる自動電源制御

**概要**: 競技開始・終了時刻をあらかじめ設定し、RTC（DS3231等）で自動ON/OFFを制御する。

**検討事項**:
- DS3231はI2C接続（拡張ヘッダのI2C0を共有可能）
- 競技開始前にすべての局が送信しないよう時刻制御
- `settings.txt` に `START_TIME` / `END_TIME` を追加

---

### Webコンフィギュレータ

**概要**: USB MSCでの `settings.txt` 編集の代わりに、WebUSBまたはBLEでブラウザから設定変更できるUIを提供する。

**検討事項**:
- TinyUSBはWebUSBをサポート
- RP2040はBluetooth非搭載（外部モジュールまたはRP2040-Wへの換装が必要）
- 優先度: 低（settings.txtでも十分実用的）

---

## 完了 / Closed

- [x] `FOX_NUMBER` 設定の追加（settings.txt対応）
- [x] ARDF識別符号（MOE〜MO5）のモールスID先頭への付与
- [x] 60秒スロット制御の実装（`audioTask()` 置き換え）
- [x] ITU Zone 1・デフォルト周波数145.660MHzの設定
- [x] `BOM.md` 作成（日本の入手先付き）
- [x] `README.ja.md` 作成
- [x] `HARDWARE.md` 作成（RP2040↔DRA818V配線表）
- [x] `TESTING.md` 作成（RTL-SDR / tinySA / 5局テスト手順）
- [x] `dra818.h` / `dra818.cpp` スタブ作成
- [x] `pwm_audio.h` スタブ作成
