# TBSK modem for C++


日本語ドキュメント 👉[README.ja.md](README.ja.md)


This is C++14 implementation of TBSKmodem. 

🐓[TBSKmodem](https://github.com/nyatla/TBSKmodem)



There are three build environments: Visual Studio, Gcc, and emscripten.


# License

This software is provided under the MIT license. For hobby and research purposes, use it according to the MIT license.

For industrial applications, be careful with patents.

This library is MIT licensed open source software, but not patent free.



## Setup
Clone the source code from Github.

```
>git clone https://github.com/nyatla/TBSKmodemCS.git
```


## Windows(Visual Studio)



### [libTbskModem](https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp/libTbskModem)
This is a project for a static library that is referenced by other projects.

### [Modulation](https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp/Modulation)
Saves modulated signals to a WAV file.

### [Demodulation](https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp/Demodulation)
Reads data from WAV files.

### [PcmTest](https://github.com/nyatla/TBSKmodemCpp/tree/master/windows/TBSKmodemCpp/PcmTest)
A test program for PCM file access.



## GCC

This is implemented in a GCC Makefile.
There are libraries and test programs included.


### [libtbskmodem.a](https://github.com/nyatla/TBSKmodemCpp/tree/master/linux/libtbskmodem)
Generate the static library libtbskmodem.a.
```
cd libtbskmodem
$make all
```
### [Modulation](https://github.com/nyatla/TBSKmodemCpp/tree/master/linux/Modulation)
Generate a test program to modulate a bit sequence. Please generate libtbskmodem.a first.
```
cd Modulation
$make all
```


## emscripten

Here's a Makefile for emscripten.

The JavaScript API to be published by emscripten is defined in [functions.cpp](https://github.com/nyatla/TBSKmodemCpp/blob/master/emscripten/tbskmodemjs/src/functions.cpp).


### [libtbskmodem](https://github.com/nyatla/TBSKmodemCpp/tree/master/emscripten/libtbskmodem)
Generate the static library libtbskmodem.a. This library is required to generate glue code.

```
cd libtbskmodem
$make all
```

### [tbskmodemjs](https://github.com/nyatla/TBSKmodemCpp/tree/master/emscripten/tbskmodemjs)
Generate glue code for wasm.

Standard glue code set.
```
cd tbskmodemjs
$make
```


Module glue code.
```
cd tbskmodemjs
$make mod

```

The modularized glue code is used in TbskmodemJS.

