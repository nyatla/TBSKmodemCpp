# TBSK modem for C++


English documente 👉[Readme.en.md](Readme.en.md)


C++で実装したTBSKmodemです。
🐓[TBSKmodem](https://github.com/nyatla/TBSKmodem)


TBSK (Trait Block Shift Keying) modemは、FFT/IFTTを使わない、低速、短距離の音響通信の実装です。
バイト/ビットストリームの振幅信号への変調、振幅信号からバイト/ビットストリームへの復調ができます。


![preview_tbsk](https://user-images.githubusercontent.com/2483108/194768184-cecddff0-1fa4-4df8-af3f-f16ed4ef1718.gif)


[Youtube](https://www.youtube.com/watch?v=4cB3hWATDUQ)でみる（信号音付きです。）

※Python版のプレビュー


## Python版との差分

APIは概ねPythonと同一です。一部、C++の標準クラスライブラリに適合させるための変更があります。
Emscripten/windows/linuxのプロジェクトファイルがあります。


## ライセンス

本ソフトウェアは、MITライセンスで提供します。ホビー・研究用途では、MITライセンスに従って適切に運用してください。
産業用途では、特許の取り扱いに注意してください。

このライブラリはMITライセンスのオープンソースソフトウェアですが、特許フリーではありません。

## GetStarted


### ソースコードのセットアップ
サンプルを含めたソースコードは、githubからcloneします。

```
>git clone https://github.com/nyatla/TBSKmodemCpp.git
```

### Windows

windows配下にVisualStadioのソリューションファイルがあります。

### linux

linux配下にmakeファイルがあります。


### emscripten

