# TBSK modem for C++


English documente 👉[README.md](README.md)


C++で実装したTBSKmodemです。Python版の同等のAPIを備えています。

🐓[TBSKmodem](https://github.com/nyatla/TBSKmodem)


ビルド環境は、VisualStudio,Gcc,emscriptenの３種類です。



# ライセンス

本ソフトウェアは、MITライセンスで提供します。ホビー・研究用途では、MITライセンスに従って適切に運用してください。

産業用途では、特許の取り扱いに注意してください。

このライブラリはMITライセンスのオープンソースソフトウェアですが、特許フリーではありません。



# GetStarted

## セットアップ
サンプルを含めたソースコードは、githubからcloneします。

```
>git clone https://github.com/nyatla/TBSKmodemCS.git
```


## Windows(Visual Studio)

VisualStadioのソリューションで実装されています。
https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp

### [libTbskModem](https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp/libTbskModem)
スタティックライブラリのプロジェクトです。他のプロジェクトから参照されます。

### [Modulation](https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp/Modulation)
変調した信号をwavファイルに保存します。

### [Demodulation](https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp/Demodulation)
wavファイルの信号からデータを読み出します。

### [PcmTest](https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp/PcmTest)
Pcmファイルアクセスのテストプログラムです。



## GCC

GCCのMakefileで実装されています。
ライブラリと、テスト用のプログラムがあります。


### [libtbskmodem.a](https://github.com/nyatla/TBSKmodemCpp/tree/master/linux/libtbskmodem)
スタティックライブラリのlibtbskmodem.aを生成します。
```
cd libtbskmodem
$make all
```
### [Modulation](https://github.com/nyatla/TBSKmodemCpp/tree/master/linux/Modulation)
ビット列を変調するテストプログラムを生成します。先にlibtbskmodem.aを生成してください。
```
cd Modulation
$make all
```


## emscripten

emscripten用Makefileです。

emscriptenで公開されるJavascriptAPIは、[functions.cpp](https://github.com/nyatla/TBSKmodemCpp/blob/master/emscripten/tbskmodemjs/src/functions.cpp)内で定義されます。


### [libtbskmodem](https://github.com/nyatla/TBSKmodemCpp/tree/master/emscripten/libtbskmodem)
スタティックライブラリのlibtbskmodem.aを生成します。このライブラリはグルーコードを生成するために必要です。

```
cd libtbskmodem
$make all
```

### [tbskmodemjs](https://github.com/nyatla/TBSKmodemCpp/tree/master/emscripten/tbskmodemjs)
wasmのグルーコードを生成します。

scriptで動作するグルーコード
```
cd tbskmodemjs
$make
```


モジュール化したグルーコード
```
cd tbskmodemjs
$make mod

```

モジュール化したグルーコードは、TbskmodemJSで使用しています。


