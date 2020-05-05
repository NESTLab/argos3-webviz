## Basic usage
Edit your Argos Experiment file (.argos), and change the visualization tag as:
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
i.e. add `<webviz/>` in place of default `<qt-opengl />`


Then run the argos experiment as usual

```console
$ argos3 -c EXPERIMENT_FILE.argos
```
This starts argos experiment with the webviz server.

*Note:* If you do not have an experiment file, you can check [http://argos-sim.info/examples.php](http://argos-sim.info/examples.php)

or run an example project,
```console
$ argos3 -c src/testing/testexperiment.argos
```

### Web Client
The web client code is placed in `client` directory (or download it as zip from the [Releases](https://github.com/NESTLab/argos3-webviz/releases)). This folder needs to be *served* through an http server(for example `apache`, `nginx`, `lighthttpd`).

The easiest way is to use python's inbuilt server, as python is already installed in most of *nix systems.

Run these commands in the terminal
```bash
$ cd client
$ python3 -m http.server 8000
```
To host the files in folder client over http port 8000.


Now you can access the URL using any browser.

> [http://localhost:8000](http://localhost:8000)


*Visit [http static servers one-liners](https://gist.github.com/willurd/5720255) for alternatives to the python3 server shown above.*

<details>
<summary style="font-size:20px">Configuration</summary>
<br>

You can check more documentation in [docs](docs/README.md) folder

#### REQUIRED XML CONFIGURATION
```xml

  <visualization>
    <webviz />
  </visualization>
```

#### OPTIONAL XML CONFIGURATION 
with all the defaults:
```xml
  <visualization>
    <webviz port=3000
         broadcast_frequency=10
         ff_draw_frames_every=2
         autoplay="true"
         ssl_key_file="NULL"
         ssl_cert_file="NULL"
         ssl_ca_file="NULL"
         ssl_dh_params_file="NULL"
         ssl_cert_passphrase="NULL"
    />
  </visualization>
```

Where:

`port(unsigned short)`: is the network port to listen incoming traffic on (Websockets and HTTP both share the same port)
```
Default: 3000
Range: [1,65535]

Note: Ports less < 1024 need root privileges.
```

`broadcast_frequency(unsigned short)`: Frequency (in Hertz) at which to broadcast the updates(through websockets)
```
Default: 10
Range: [1,1000]
```
`ff_draw_frames_every(unsigned short)`: Number of steps to skip when in fast forward mode
```
Default: 2
```
`autoplay(bool)`: Allows user to auto-play the simulation at startup
```
Default: false
```

#### SSL CONFIGURATION

SSL can be used to host the server over "wss"(analogous to "https" for websockets).

**NOTE**: You need Webviz to be compiled with OpenSSL support to use SSL.

You might have to use any combination of the following to enable SSL, depending upon your implementation.

- ssl_key_file
- ssl_cert_file
- ssl_ca_file
- ssl_dh_params_file
- ssl_cert_passphrase

Where file parameters supports relative and absolute paths.

**NOTE**: Webviz need read access to the files.

You can check more documentation in [docs](docs/README.md) folder

</details>
