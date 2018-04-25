# Go bindings for Sciter

[![Build status](https://ci.appveyor.com/api/projects/status/rphv883klffw9em9/branch/master?svg=true)](https://ci.appveyor.com/project/pravic/go-sciter)
[![Build Status](https://img.shields.io/travis/sciter-sdk/go-sciter/master.svg)](https://travis-ci.org/sciter-sdk/go-sciter)
[![License](https://img.shields.io/github/license/sciter-sdk/go-sciter.svg)](https://github.com/sciter-sdk/go-sciter)
[![Join the forums at https://sciter.com/forums](https://img.shields.io/badge/forum-sciter.com-orange.svg)](https://sciter.com/forums)

Check [this page](http://sciter.com/developers/sciter-sdk-bindings/) for other language bindings (Delphi / D / Go / .NET / Python / Rust).

----


# Attention

The ownership of project is transferred to this new organization.
Thus the `import path` for golang should now be `github.com/sciter-sdk/go-sciter`,
but the package name is still `sciter`.

# Introduction

This package provides a Golang bindings of [Sciter][] using cgo.
Using go sciter you must have the platform specified `sciter dynamic library`
downloaded from [sciter-sdk][], the library itself is rather small
 (under 5MB, less than 2MB when upxed) .

Most [Sciter][] API are supported, including:

 * Html string/file loading
 * DOM manipulation/callback/event handling
 * DOM state/attribute handling
 * Custom resource loading
 * Sciter Behavior
 * Sciter Options
 * Sciter Value support
 * NativeFunctor (used in sciter scripting)

And the API are organized in more or less a gopher friendly way.

Things that are not supported:

 * Sciter Node API
 * TIScript Engine API

# Getting Started

 1. Download the [sciter-sdk][]
 2. Extract the sciter runtime library from [sciter-sdk][] to system PATH

    The runtime libraries lives in `bin` `bin.gtk` `bin.osx` with suffix like `dll` `so` or `dylib`

    * Windows: simply copying `bin\64\sciter.dll` to `c:\windows\system32` is just enough
    * Linux:
      - `cd sciter-sdk/bin.gtk/x64`
      - `export LIBRARY_PATH=$PWD`
      - `echo $PWD >> libsciter.conf`
      - `sudo cp libsciter.conf /etc/ld.so.conf.d/`
      - `sudo ldconfig`
      - `ldconfig -p | grep sciter` should print libsciter-gtk.so location
    * OSX:
      - `cd sciter-sdk/bin.osx/`
      - `export DYLD_LIBRARY_PATH=$PWD`

 3. Set up GCC envrionmnet for CGO

    [mingw64-gcc][] (5.2.0 and 7.2.0 are tested) is recommended for Windows users.

    Under Linux gcc(4.8 or above) and gtk+-3.0 are needed.

 4. `go get -x github.com/sciter-sdk/go-sciter`

 5. Run the example and enjoy :)

# Sciter Desktop UI Examples

![](http://sciter.com/screenshots/slide-norton360.png)

![](http://sciter.com/screenshots/slide-norton-nis.png)

![](http://sciter.com/screenshots/slide-cardio.png)

![](http://sciter.com/screenshots/slide-surveillance.png)

![](http://sciter.com/screenshots/slide-technology.png)

![](http://sciter.com/screenshots/slide-sciter-ide.png)

![](http://sciter.com/screenshots/slide-sciter-osx.png)

![](http://sciter.com/screenshots/slide-sciter-gtk.png)


# Sciter Version Support
Currently supports [Sciter][] version `4.0.0.0` and higher.

[Sciter]: http://sciter.com/
[sciter-sdk]: http://sciter.com/download/

# About Sciter

[Sciter][] is an `Embeddable HTML/CSS/script engine for modern UI development, Web designers, and developers, can reuse their experience and expertise in creating modern looking desktop applications.`

In my opinion, [Sciter][] , though not open sourced, is an great
desktop UI development envrionment using the full stack of web technologies,
which is rather small (under 5MB) especially compared to [CEF][],[Node Webkit][nw] and [Atom Electron][electron]. :)

Finally, according to [Andrew Fedoniouk][author] the author and the Sciter
`END USER LICENSE AGREEMENT` , the binary form of the [Sciter][]
dynamic libraries are totally free to use for commercial or
non-commercial applications.

# The Tailored Sciter C Headers
This binding ueses a tailored version of the sciter C Headers, which lives in directory: `include`. The included c headers are a modified version of the
[sciter-sdk][] standard headers.

It seems [Sciter][] is developed using C++, and the included headers in the
[Sciter SDK][sciter-sdk] are a mixture of C and C++, which is not
quite suitable for an easy golang binding.

I'm not much fond of C++ since I started to use Golang, so I made this
modification and hope [Andrew Fedoniouk][author] the author would provide
pure C header files for Sciter. :)

[CEF]:https://bitbucket.org/chromiumembedded/cef
[nw]: https://github.com/nwjs/nw.js
[electron]:https://github.com/atom/electron

[author]: http://sciter.com/about/
[mingw64-gcc]: http://sourceforge.net/projects/mingw-w64/
