# Developing Webviz

You need few extra packages namely 

- cppcheck (`sudo apt install cppcheck`)


and then configure cmake with Debug flag,

```console
$ cmake -DCMAKE_BUILD_TYPE=Debug ../src
```

And then build the solution as same,
```console
$ make
```

Also you need to run all the internal tests,
```console
$ GTEST_COLOR=1 ctest -V
```