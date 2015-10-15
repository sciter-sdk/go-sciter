# About the binding
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

# The Tailered Sciter C Headers
This binding ueses a tailored version of the sciter C Headers, which lives in directory: `include`. The included c headers are a modified version of the 
[sciter-sdk][] standard headers.

It seems [Sciter][] is developed using C++, and the included headers in the 
[Sciter SDK][sciter-sdk] are a mixture of C and C++, which is not 
quite suitable for an easy golang binding. 

I'm not much fond of C++ since I started to use Golang, so I made this
modification and hope [Andrew Fedoniouk][author] the author would provide 
pure C header files for Sciter. :)

# Sciter Version Support
Currently supports [Sciter][] version `3.3.0.3` - `3.3.0.6`

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

[CEF]:https://bitbucket.org/chromiumembedded/cef
[nw]: https://github.com/nwjs/nw.js
[electron]:https://github.com/atom/electron

[author]: http://sciter.com/about/