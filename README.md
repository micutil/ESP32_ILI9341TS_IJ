#ESP32-ILI9341 Touchscreen for IchigoJam

IchigoJam (ファーム：1.4β20 for ILI9341）と、2.8インチ ILI9341 タッチスクリーン付きと、ESP32を使って、IchigoJamのタッチパネルをやってみました。

## 使うもの

- [2.8インチ ILI9341 320x240 タッチスクリーン付き](https://www.amazon.co.jp/gp/product/B072N551V3/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1)
- ESP32系のモジュール（例：[ESP32 DevKitC](https://www.amazon.co.jp/s?k=ESP32+DevKitC&__mk_ja_JP=カタカナ&ref=nb_sb_noss_2)）
- IchigoJam (ファーム：[1.4β20 for ILI9341](https://fukuno.jig.jp/2846)）
- ジャンパーワイヤー何本か
- ブレッドボード何個か

## 繋げ方

|  IchigoJam  |  ILI9341  | ESP32  |
| ---- | ---- | ---- |
|  VCC (3.3V)  | VCC |
|  GND  |  GND  | GND  |
|  GND  |  CS  | GND |
|  IN4  |  RESET  |  |
|  IN2  |  DC  |  |
|  VIDEO2  |  SDI(MOSI)  |  |
|  IN1  |  SCK  |  |
|  VCC  |  LCD  |  |
|   |  SDO(MISO)  |  |
|   |  T-CLK  | IO18 (SCK) |
|   |  T-CS  |  GND ... IO15 |
|   |  T-DIN  | IO23 (MOSI) |
|   |  T-DO  | IO19 (MISO) |
|   |  T-IRQ  |  |
| TX  |  | IO16 |
| RX  |  | IO17 |

## ESP32 arduinoスケッチ

- 画像はIchigoJamの方でやってるので、Adafruit_ILI9341のライブラリーは使いません。
- タッチのライブラリーは、XPT2046_Touchscreenです。
- タッチとESP32の接続はSPIのデフォルトの接続で、VSPI - SCK,MISO,MOSI,SS => 18,19,23,5番ピンになります。5は使いません。
- T_CSは15番ピンを指定してますが、繋げていません。

## ESP32 arduino binファイル

ESP32_ILI9341_TS_IJ.ino.esp32.binを同梱しています。[Flash Download Tools](https://www.espressif.com/en/support/download/other-tools)、[M5Burner_Mic](https://github.com/micutil/M5Burner_Mic)などを使ってインストールして下さい。M5Burner_Micを使う場合は、M5Burner_Micを起動して、そのウインドウに同梱の**DropToM5Burner_Mic.json**ファイルをドラッグ&ドロップして下さい。

## IchigoJam サンプルプログラム
- Kawakudari_ILI9341_ESP32.TXT： 川下りゲーム（[動画](https://www.facebook.com/groups/ichigojam/permalink/1779846782155067/)）
- TachDraw_ILI9341_ESP32.TXT： タッチ描画（[動画](https://www.facebook.com/groups/ichigojam/permalink/1781416798664732/)）

ESP32に左右の情報を得る場合

	20 UART1,2:?"TS H" 'ESP32に送信（HはHorizontal）
	30 K=INKEY(): UART0,0 'Kは28 or 29
	
ESP32に上下の情報を得る場合

	20 UART1,2:?"TS V" 'ESP32に送信（VはVertical）
	30 K=INKEY(): UART0,0 'Kは30 or 31

ESP32にX,Y座標と筆圧情報を得る場合

	20 UART1,2:?"TS XYZ":WAIT2 '少し待つ
	30 X=INKEY():Y=INKEY():Z=INKEY:UART0,0 'X=0-31, Y=0-23, Z=1-4

ESP32にX座標と筆圧を得る場合

	20 UART1,2:?"TS XZ":WAIT2 '少し待つ
	30 X=INKEY():Z=INKEY:UART0,0 'X=0-31,Z=1-4
	
コマンド一覧

| コマンド | レスポンス | 値 |
|---|---|---|
| TS XYZ | X,Y座標と筆圧 | X=0-31, Y=0-23, Z=1-4 |
| TS XY | X,Y座標 | X=0-31, Y=0-23 |
| TS XZ | X座標と筆圧 | X=0-31, Z=1-4 |
| TS YZ | Y座標と筆圧 | Y=0-23, Z=1-4 |
| TS X | X座標 | X=0-31 |
| TS Y | Y座標 | Y=0-23 |
| TS Z | 筆圧 | Z=1-4 |
| TS H | 左右キー | 左=28、右=29 |
| TS V | 上下キー | 上=30、下=31 |

## ライセンス
CC BY Micono
