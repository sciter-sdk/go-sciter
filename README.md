# Attention !!

This project is not maintained for a quite long time and is 
looking for a new project owner or some active contributors. 

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

    The runtime libraries lives in `bin` `bin.gtk` `bin.osx` with suffix
    like `dll` `so` or `dylib` (osx is not supported yet, due to the lack of osx computer)

    The `system PATH` is os specific, under Windows simply copying `sciter32.dll` or `sciter64.dll` to `c:\windows\system32` is just enough.

 3. Set up GCC envrionmnet for CGO

    [mingw64-gcc][] (5.2.0 is tested) is recommended for Windows users.

    Under Linux gcc(4.8 or above) and gtk+-3.0 are needed.
    Due to the dependencies of gkt version of [Sciter][] runtime library, only
    `debian 8` is tested but not for other distributions.

 4. `go get -x github.com/oskca/sciter`

 5. Run the example and enjoy :)

# Sciter Desktop UI Examples

![](http://sciter.com/screenshots/slide-norton360.png)

![](http://sciter.com/screenshots/slide-norton-nis.png)

![](http://sciter.com/screenshots/slide-surveillance.png)

![](http://sciter.com/screenshots/slide-technology.png)

![](http://sciter.com/screenshots/slide-sciter-ide.png)

![](http://sciter.com/screenshots/slide-sciter-osx.png)

![](http://sciter.com/screenshots/slide-sciter-gtk.png)


# Sciter Version Support
Currently supports [Sciter][] version `3.3.0.3` - `before 4.0.0.0`

[Sciter]: http://sciter.com/
[sciter-sdk]: http://sciter.com/sdk/sciter-sdk-3.zip

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
