% DrawTextPlugin -- The plugin-based Screen('DrawText') text renderer.
%
% You may have arrived here because Screen() instructed you to go here
% after Screen failed to load the external text renderer plugin.
%
% On all operating systems the functions Screen('DrawText') and
% Screen('TextBounds') try to use an external text rendering plugin for
% drawing and handling of high quality text. This allows for advanced text
% layout and formatting, high-quality anti-aliased rendering of text at
% arbitrary text sizes, support for modern fonts like TrueType, and support
% for drawing of the full international Unicode character set. Its use across
% all operating systems also allows for (more) consistent text appearance on
% different systems. Differences between installed fonts on different operating
% systems can still cause slight differences in text appearance if you don't
% manage this though.
%
% Which text renderer should be used can be set via
% Screen('Preference','TextRenderer', type), and after a first call to
% Screen('DrawText', ...), type = Screen('Preference','TextRenderer') will
% report which 'type' of text renderer is actually used. A 'type' of 1 means
% that the plugin based renderer is used, a 'type' of 0 or -1 would mean the
% use of a less high quality fallback renderer, e.g., because loading the
% text rendering plugin failed.
%
% If use of the drawtext plugin renderer fails then that likely means that
% some required 3rd party library is not installed, or of incompatible version.
% The plugin requires a working installation of the FreeType-2 and
% FontConfig libraries on your operating system, ie., somewhere stored in
% a folder on the system library search path.
%
% How to get those required 3rd party libraries:
% ==============================================
%
% Linux:
% ------
%
% Nothing to do. On Linux these libraries are part of the default installation
% of any decent GNU/Linux distribution, so there usually isn't any need for
% manual setup work on your part.
%
% macOS:
% ------
%
% On macOS with GNU/Octave, these are part of Octave, so you don't need to
% do anything. If you use macOS with Matlab then we recommend installing
% GStreamer (help GStreamer) which also provides multi-media support, as
% GStreamer provides the required libraries. Other sources than GStreamer
% are HomeBrew, or XQuartz (X11 for macOS). Other sources of this libraries
% may also work, as long as they are findable by the macOS linker, but this
% is only tested with GStreamer and HomeBrew.
%
% Some recent Matlab versions, e.g., R2015a and R2015b will contain an
% outdated and incompatible version of libfreetype.6.dylib which may
% cause problems. If you experience such problems, malfunctions or crashes,
% then delete (or rename) that file, or move it out of the way.
% E.g., for R2015b you'd have to delete or rename this file:
%
% /Applications/MATLAB_R2015b.app/bin/maci64/libfreetype.6.dylib
%
% Windows:
% --------
%
% On MS-Windows you will need to install the GStreamer multi- media framework -
% see "help GStreamer" for installation instructions - otherwise Psychtoolbox
% will use the old lower quality GDI text renderer instead. As of Psychtoolbox
% version 3.0.19, you *must* install GStreamer v1.22 or later (see the download
% link in "help GStreamer" for the latest tested version). All earlier versions
% will not work!
%
% The first time a script calls a text drawing function after an operating system
% update, or after the installation of new text fonts, a long pause of many seconds
% or even minutes may occur, while the so called fontconfig cache gets rebuilt.
% Patience is the key. If this pause happens not only once, but at each invocation
% of text drawing, your system may have developed a glitch, as described in GitHub
% issue #429 and #579 on our issue tracker:
%
% https://github.com/Psychtoolbox-3/Psychtoolbox-3/issues/429
% https://github.com/Psychtoolbox-3/Psychtoolbox-3/issues/579
%
% The solution is to manually delete the fontconfig cache. E.g., if your user
% name would be "paul", you'd likely need to delete the following file:
% "C:\Users\paul\AppData\Local\fontconfig\cache"
%
% Some users find that the location of the cache file could be also in a different
% place, e.g., following the above example for user "paul" it could be under:
% "C:\Users\paul\.cache\fontconfig\", so files in that folder would need to be
% deleted.
%
% More background info about Psychtoolbox's standard text renderer:
% =================================================================
%
% On macOS one can still select Apple's CoreText text renderer via the command
% Screen('Preference','TextRenderer', 0); although Apples text renderer is
% inferior in essentially any aspect. Apples CoreText renderer would also
% get automatically selected if the plugin renderer would not work for some
% reason. On MS-Windows one can still select the legacy MS-Windows GDI text
% renderer via Screen('Preference','TextRenderer', 0). This renderer provides
% lower quality anti-aliasing, less accurate computation of text bounding boxes
% via Screen('TextBounds'), and less accurate text positioning, as well as a
% lower text drawing speed. Additionally this legacy GDI text renderer has
% problems on HiDPI "Retina" displays and can misrender text in both size and
% appearance.
%
% The text renderer plugin implements a high-speed renderer based on a
% combination of multiple free software libraries for text rendering and
% text handling:
%
% * OGLFT (http://oglft.sourceforge.net/) the OpenGL-FreeType library.
% * The FreeType-2 (http://freetype.sourceforge.net/) library.
% * The FontConfig (http://www.fontconfig.org) library.
%
% The FontConfig library is used to find the optimal font and font settings,
% given a specific font specification by your user code, a process known as
% "font mapping". FontConfig internally uses the FreeType-2 library to handle
% the font files on your system and to gather all needed information for the
% matching process.
%
% After a font and settings have been selected, FreeType-2 is used to load
% the font, and to convert it into high-quality character glyphs. Then the
% OGLFT library is used to convert these glyphs into a format optimized for
% fast drawing with OpenGL. OGLFT also performs caching of glyphs, text
% layout, measurement of text dimensions and bounding boxes and the actual
% drawing of the text.
%
% Our actual plugin coordinates all these operations and communicates with
% Screen().
%
% The source code of the plugin can be found in the Psychtoolbox source
% tree under PsychSourceGL/Cohorts/FTGLTextRenderer/
%
% The plugins themselves are stored in the
% Psychtoolbox/PsychBasic/PsychPlugins folder of your Psychtoolbox
% installation. This is where Screen() expects to find the plugins for
% dynamic loading.
%
