NaClAMBase
==========

NaCl Acceleration Module Base

Example
========

See NaClAMTest/NaClAMTest.cpp for C++ code

See NaClAMTest/NaClAMTest.js for JS code


Build Systems
==============

The preferred build system is Visual Studio based.

premake is offered as a limited alternative, to build run the following:

```
premake4 --file=premake32.lua gmake
make -R
premake4 --file=premake64.lua gmake
make -R
```

To test:

```
python ./http.py
```
