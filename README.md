# ART

## AR Terminal Headset powered by Atmel ATTiny84
#### v0.1
#### Bogdan Vitel


## Contents

```
[root]
  |-[ARCentral] : The Central application responsible for sending data to the headset
  |    |- [CVProcessor] : The video acquisition and processing system
  |    |- [BLECentral]  : The BLE Central library responsible for sending data from the processor to the headset
  |    |- [GAPITests]   : Project used for testing GAPI logic
  |    \- [unittests]   : Circular Buffer and Circular Queue tests
  |-[docs] : doxygen documentation
  |
  \-[Terminal] : The headset firmware
```

## ARCentral requirements:

* QtCreator 4.9
* Qt Framework 5.12
* OpenCV 4.x latest: https://github.com/opencv
* ArUco latest: https://sourceforge.net/projects/aruco/

* Preferably a linux machine

## Terminal requirements:

* Atmel AVR Studio 6
* Windows machine (might work on wine, no guarantees there)

**NOTE:** If you know what you're doing, you should be able to setup an AVR-C toolchain on linux but you'd have to 
convert the .atsln and .atproj configurations to your buildchain manually.
