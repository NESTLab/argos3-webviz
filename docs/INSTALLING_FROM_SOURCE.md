# Installing from source


## Requirements
- A UNIX system (Linux or MacOSX; Microsoft Windows is not supported)
- ARGoS 3
- g++ >= 5.7 (on Linux)
- clang >= 3.1 (on MacOSX)
- cmake >= 3.5.1
- zlib >= 1.x
- libuv >= 1.3 or Boost.Asio 1.x (both optional on Linux)
- git (for autoinstalling dependencies)

**Optional dependency**
- OpenSSL >= 1.1 (for websockets over SSL)


For installing all dependencies
- Homebrew: `brew install cmake git zlib openssl libuv`  
- Debian: `sudo apt install cmake git zlib1g-dev libssl-dev`  
- Fedora: `sudo dnf install cmake git zlib-devel openssl-devel`  

## Downloading
```console
$ git clone https://github.com/NESTLab/argos3-webviz
```

## Compiling
The compilation configured through CMake.

```console
$ cd argos3-webviz
$ mkdir build
$ cd build
$ cmake ../src
$ make
$ sudo make install
```

You can use `-DCMAKE_BUILD_TYPE=Debug` with cmake command above to enable debugging while developing.

