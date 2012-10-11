NaClAMBase
==========

NaCl Acceleration Module Base.

Acceleration Modules allow JavaScript developers to use and interact with C/C++ code.

Try out a live demo here:

https://chrome.google.com/webstore/detail/oijilmepnojdffifoaoaeaemcnmcleld/

Read more here:

http://www.johnmccutchan.com/2012/10/bullet-native-client-acceleration-module.html

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
