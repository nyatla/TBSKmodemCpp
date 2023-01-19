git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh


cd libtbskmodem make #.aの作成


テスト
python -m http.server 8000