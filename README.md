# argos3-WebViz
A visualizer plugin for [ARGoS](https://www.argos-sim.info/) for controlling it over the web.

## Installation
Run the following in a folder of your choice

```bash
# Download
git clone https://github.com/NESTLab/argos3-webviz
cd argos3-webviz
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../src
make
sudo make install
```

## Usage
A test project can be run from `root folder` of this project,

```bash
argos3 -c src/testing/testexperiment.argos
```
This starts argos experiment with the webviz server.

### Client
The client code is placed in `client` directory. These files need to be *served* through an http server (for example `apache`, `nginx`, `lighthttpd`).

The easiest way is to use python's inbuilt server, as python is already installed in most of *nix systems.

Run these commands in the terminal
```bash
cd client
python3 -m http.server 8000
```
To host the files in folder client over http port 8000.


Now you can access the URL using any browser.

[http://localhost:8000](http://localhost:8000)



*Visit [http static servers one-liners](https://gist.github.com/willurd/5720255) for alternatives to the python3 server shown above.*

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)

Licenses of libraries used are in their respective directories.


## Limitations
OpenGL Loop functions are closely coupled with QT-OpenGL as they are meant to be used to draw using OpenGL, hence it is currently neglected in this plugin.