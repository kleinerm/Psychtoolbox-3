FTGL based 'DrawText' text renderer plugin for Psychtoolbox-3:
--------------------------------------------------------------

This is the implementation / source code folder of libptbdrawtext_ftgl,
a dynamically loaded, shared library which acts as a plugin for
high-quality text rendering on behalf of the Screen('DrawText')
function.

It is the default text rendering method on GNU/Linux versions of
Psychtoolbox-3, but can also be used on MacOS/X versions for
Intel-based Macintosh computers.

Whenever Screen('Preference', 'TextRenderer', 2); is selected,
Screen('TextBounds') and Screen('DrawText') at first invocation
will dynamically load and link against this plugin module and use
it for actual font matching, font loading, typesetting / layout
and high-quality anti-aliased unicode text rendering.

The plugin itself is licensed under LGPLv2 license. Depending
on build configuration (see top of libptbdrawtext_ftgl.cpp), it
links statically or dynamically against OGLFT or even includes
the source code of OGLFT.cpp/h itself. OGLFT is the
OpenGL-FreeType text rendering library ( http://oglft.sourceforge.net/ ),
a high-quality text rendering library for use within OpenGL applications.

OGLFT is Copyright (C) 2002 lignum Computing, Inc. <oglft@lignumcomputing.com>
and Copyright (C) 2008 Allen Barnett. It is licensed under LGPL version 2.1,
or (at your option) any later version.

While this plugin would build and work with standard installations of OGLFT
- as provided by many popular Linux distributions in their default install
or via their packaging systems, it requires a OGLFT lib with QT-4 and Unicode
text rendering support. Many default builds of OGLFT come without these.

To avoid incompatibilities and installation issues for our users, as well
as potential version conflicts with OGLFT versions used by Matlab or
Octave themselves, we directly include the source code of a slightly
modified version of OGLFT (see files OGLFT.cpp / .h) into our build
which provides Unicode text rendering support. To get rid of the extremely
heavyweight dependency on the QT-4 toolkit for the sole purpose of Unicode
text handling, we provide our own bare-bones minimalistic reimplementation
of the QChar and QString classes -- API compatible to QT-4, but not
based on the QT-4 codebase and not ABI compatible in any way. This
emulation layer just provides a minimal subset of functionality needed
for this plugin to work.

Our build instructions in libptbdrawtext_ftgl.cpp explain how you can
switch between building against our own OGLFT source tree and against
a regular "official: SDK installed on your favorite Linux or OS/X system,
should you feel the need to do so.

Both, OGLFT and our own plugin code make use of the services provided
by the excellent Fontconfig library and FreeType-2 library for font matching,
font loading/handling and glyph rendering. The plugin dynamically links
against copies of those libraries on your system, so a working libfontconfig
and libfreetype setup is a requirement for this plugin to load & work
correctly.

Homepage of FreeType: http://www.freetype.org
Homepage of FontConfig: http://www.fontconfig.org/wiki
