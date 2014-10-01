PegVM - a virtual machine interpreter for peg4d
====
[![Build Status](https://secure.travis-ci.org/shun-honda/simplevm.png)](http://travis-ci.org/shun-honda/simplevm)
## Description
PegVM is ...

## Requirement
Need to install [Boehm-Demers-Weiser GC](http://www.hboehm.info/gc/)
### OSX
```
brew install bohemgc cmake
```
### Ubuntu
```
sudo apt-get install cmake libgc-dev
```


## Build
You'll have to make a few steps. Type in:
```
	$ mkdir build
	$ cd build
	$ cmake ..
	$ make
	$ make install # or execute ./build/pegvm
```
