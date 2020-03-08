# argos3-WebViz
A visualizer plugin for [ARGoS](https://www.argos-sim.info/) for controlling it over the web.

## Installation
Run the following in a folder of your choice (you can delete it once installed)

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

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)

Licenses of libraries used are in their respective folders inside `libs`


## Limitations
Loop functions are closely coupled with QT-OpenGL as it was meant to be used to draw over the OpenGL, hence it is neglected in this plugin