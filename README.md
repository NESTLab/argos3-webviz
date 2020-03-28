# argos3-WebViz
A visualizer plugin for [ARGoS 3](https://www.argos-sim.info/) for controlling it over the web.

# Installing
## Binary Packages for Ubuntu 18.04 LTS 64bit

## Binary Packages for Ubuntu 16.04 LTS 64bit

## Homebrew Formula for MacOSX
On MacOSX, ARGoS3-Webviz can be installed through [Homebrew](http://brew.sh/).

### Configuring Homebrew
To get the Homebrew Formula working, you need to add the dedicated tap:

```console
$ brew tap NESTLab/argos3-webviz
```

This operation is necessary only once, to make the first installation of ARGoS3-Webviz possible.

### Installing ARGoS3-Webviz
To install ARGoS3-webviz for the first time, it is enough to write this command:

```console
$ brew install argos3-webviz
```

This command installs ARGoS3-webviz

### Updating ARGoS3-Webviz
To update an installed version of ARGoS3-webviz to the newest version, first update the formulas:
```console
$ brew update
```
and then upgrade ARGoS3-webviz:
```console
$ brew upgrade argos3-webviz
```

## Usage
A test project can be run from the root directory of this project,

```console
$ argos3 -c src/testing/testexperiment.argos
```
This starts argos experiment with the webviz server.

### Client
The client code is placed in `client` directory. These files need to be *served* through an http server (for example `apache`, `nginx`, `lighthttpd`).

The easiest way is to use python's inbuilt server, as python is already installed in most of *nix systems.

Run these commands in the terminal
```bash
$ cd client
$ python3 -m http.server 8000
```
To host the files in folder client over http port 8000.


Now you can access the URL using any browser.

[http://localhost:8000](http://localhost:8000)



*Visit [http static servers one-liners](https://gist.github.com/willurd/5720255) for alternatives to the python3 server shown above.*



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

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)

Licenses of libraries used are in their respective directories.


## Limitations
OpenGL Loop functions are closely coupled with QT-OpenGL as they are meant to be used to draw using OpenGL, hence it is currently neglected in this plugin.