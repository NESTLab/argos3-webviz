# Installing from source


## Requirements
- A UNIX system (Linux or MacOSX; Microsoft Windows is not supported)
- ARGoS 3
- g++ >= 5.7 (on Linux)
- clang >= 3.1 (on MacOSX)
- cmake >= 3.5.1

**Optional dependency**
- OpenSSL >= 1.1 (for websockets over SSl)
- FreeImage >= 3.15 (for floor image, also need ARGoS 3 built with FreeImage support)

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

