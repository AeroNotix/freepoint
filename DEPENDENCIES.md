C++
===

Prerequisites
-------------

Download the freepoint source and untar/unzip to your workspace.

Linux
-----

* Qt4
* GCC 4.7+
* QJson (http://qjson.sourceforge.net/)
* OpenSSL

Windows
-------

* QtSDK (4.8.2, compiled with Visual Studio 2010)
* QJson (http://qjson.sourceforge.net/)
* Microsoft Visual Studio 2010
* OpenSSL

Compiling
=========

Linux
-----

```bash

$ cd /your/project/directory/
$ make
```

Windows
-------

* Open the QtCreator application.
* Load the freepoint.pro file.
* Ensure the build-target lists MSVC2010 as the compiler.
* Hit the green arrow in the lower-left-hand side.

OpenSSL
=======

Linux
-----

Linux will come with OpenSSL pre-installed and working, no need to do
anything on Linux.

Windows
-------

OpenSSL will need to be downloaded and then Qt will need to be
recompiled to use the OpenSSL libraries.

You can download the OpenSSL library for Windows via this web page -
http://slproweb.com/products/Win32OpenSSL.html

Once the OpenSSL library is installed you will need to head to the
QtSDK installation directory:

By default this is C:\Qt. You then need to head to the version
directory and then the bin directory:

```shell

cd C:\Qt\4.8.3\bin
```

In here you can now reconfigure Qt to link in the OpenSSL libraries:

(this assumes that OpenSSL was installed to C:\OpenSSL)

```shell

configure -openssl -I C:\OpenSSL\include -L C:\OpenSSL
nmake
```

Once that is done Qt will be able to be compiled to use the SSL
enabled things (which is vital for HTTPS communication.)
