# argos3-WebViz
A visualizer plugin for [ARGoS 3](https://www.argos-sim.info/) for controlling it over the web.

# Installing
## Binary Packages for Ubuntu 16.04 LTS 64bit
![Ubuntu 16.04 build](https://travis-matrix-badges.herokuapp.com/repos/NESTLab/argos3-webviz/branches/master/1?use_travis_com=true)

## Binary Packages for Ubuntu 18.04 LTS 64bit
![Ubuntu 18.04 build](https://travis-matrix-badges.herokuapp.com/repos/NESTLab/argos3-webviz/branches/master/2?use_travis_com=true)

## Homebrew Formula for MacOSX
![MacOSX build](https://travis-matrix-badges.herokuapp.com/repos/NESTLab/argos3-webviz/branches/master/3?use_travis_com=true)

On MacOSX, ARGoS3-Webviz can be installed through [Homebrew](http://brew.sh/).

**Configuring Homebrew**  
To get the Homebrew Formula working, you need to add the dedicated tap:

```console
$ brew tap NESTLab/argos3-webviz
```

This operation is necessary only once, to make the first installation of ARGoS3-Webviz possible.

**Installing ARGoS3-Webviz**  
To install ARGoS3-webviz for the first time, it is enough to write this command:

```console
$ brew install argos3-webviz
```

This command installs ARGoS3-webviz

**Updating ARGoS3-Webviz**  
To update an installed version of ARGoS3-webviz to the newest version, first update the formulas:
```console
$ brew update
```
and then upgrade ARGoS3-webviz:
```console
$ brew upgrade argos3-webviz
```

# Usage
Edit your Argos Experiment file (.argos), and change the visualization node to:
```xml
.. 
..
<visualization>
    <webviz />
    <!-- <qt-opengl /> -->
</visualization>
..
..
```

Then run the argos experiment as usual

```console
$ argos3 -c EXPERIMENT_FILE.argos
```
This starts argos experiment with the webviz server.

*Note:* If you do not have an experiment file, you can check [http://argos-sim.info/examples.php](http://argos-sim.info/examples.php)

### Web Client
The web client code is placed in `client` directory. This folder needs to be *served* through an http server(for example `apache`, `nginx`, `lighthttpd`).

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

# Installing for source
If you prefer to build it from source (for linux and MacOSX only), please follow [Installing from source guide](docs/INSTALLING_FROM_SOURCE.md)

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.
[Check full contributing info](docs/CONTRIBUTING.md)
## License
[MIT](https://choosealicense.com/licenses/mit/)

Licenses of libraries used are in their respective directories.


## Limitations
OpenGL Loop functions are closely coupled with QT-OpenGL as they are meant to be used to draw using OpenGL, hence it is currently neglected in this plugin.