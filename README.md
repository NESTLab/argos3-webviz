
<br/>
<p align="center" style="background:#edeff3;">
        <img width="80%" style="max-width:540px" src="client/images/banner_light.png" alt="Webviz banner">
</p>

<br/>
<p align="center">
    <a href="https://github.com/NESTLab/argos3-webviz/blob/master/LICENSE" target="_blank">
        <img src="https://img.shields.io/github/license/NESTLab/argos3-webviz.svg" alt="GitHub license">
    </a>
    <a href="https://github.com/NESTLab/argos3-webviz/releases" target="_blank">
        <img src="https://img.shields.io/github/tag/NESTLab/argos3-webviz.svg" alt="GitHub tag (latest SemVer)">
    </a>
    <a href="https://github.com/NESTLab/argos3-webviz/commits/master" target="_blank">
        <img src="https://img.shields.io/github/commit-activity/m/NESTLab/argos3-webviz.svg" alt="GitHub commit activity">
    </a>
        <img src="https://img.shields.io/github/last-commit/NESTLab/argos3-webviz" alt="GitHub last commit">

</p>
<br/>


# ARGoS3-Webviz
A Web interface plugin for [ARGoS 3](https://www.argos-sim.info/).

| All builds | Ubuntu 16.04  | Ubuntu 18.04  | Mac OSX |
|:-:|:-:|:-:|:-:|
| [![Travis build](https://img.shields.io/travis/com/NESTLab/argos3-webviz)](https://travis-ci.com/NESTLab/argos3-webviz) | [![Ubuntu 16.04 build](https://travis-matrix-badges.herokuapp.com/repos/NESTLab/argos3-webviz/branches/master/1?use_travis_com=true)](https://travis-ci.com/NESTLab/argos3-webviz) | [![Ubuntu 18.04 build](https://travis-matrix-badges.herokuapp.com/repos/NESTLab/argos3-webviz/branches/master/2?use_travis_com=true)](https://travis-ci.com/NESTLab/argos3-webviz) | [![MacOSX build](https://travis-matrix-badges.herokuapp.com/repos/NESTLab/argos3-webviz/branches/master/3?use_travis_com=true)](https://travis-ci.com/NESTLab/argos3-webviz) |

# Installing

You can [Download pre-compiled binaries](https://github.com/NESTLab/argos3-webviz/releases)

or

Follow [this guide to install from source](docs/INSTALLING_FROM_SOURCE.md)


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
The web client code is placed in `client` directory (or download it as zip from the [releases](https://github.com/NESTLab/argos3-webviz/releases)). This folder needs to be *served* through an http server(for example `apache`, `nginx`, `lighthttpd`).

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

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.
[Check full contributing info](docs/CONTRIBUTING.md)
## License
[MIT](https://choosealicense.com/licenses/mit/)

Licenses of libraries used are in their respective directories.


## Limitations
OpenGL Loop functions are closely coupled with QT-OpenGL as they are meant to be used to draw using OpenGL, hence it is currently neglected in this plugin.