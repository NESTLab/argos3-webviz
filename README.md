# argos3-network-API
A visualizer plugin for [ARGoS](https://www.argos-sim.info/) for controlling it over the network.

## Installation
Run the following in a folder of your choice (you can delete it once installed)

```bash
# Download
git clone --recursive https://github.com/NESTLab/argos3-network-api
mkdir argos3-network-api/build
cd argos3-network-api/build
cmake ..
make -j 12
sudo make install
```

## Usage
A test project can be run from `root folder` of this project,

```bash
argos3 -c tests/diffusion_10.argos
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)

Licenses of libraries used are in their respective folders inside `libs`