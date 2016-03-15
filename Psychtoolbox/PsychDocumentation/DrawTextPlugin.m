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
% for drawing of the full international Unicode character set.
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
% OSX:
% ----
%
% On OSX with GNU/Octave, these are part of Octave, so you don't need
% to do anything. If you use OSX with Matlab then you need to install
% the XQuartz X11 X-Server. See this link for how to get it:
%
% https://support.apple.com/en-us/HT201341
%
% Some recent Matlab versions, e.g., R2015a and R2015b will contain an
% outdated and incompatible version of libfreetype.6.dylib which will
% cause problems. Delete (or rename) that file, or move it out of the way.
% E.g., for R2015b you'd have to delete or rename this file:
%
% /Applications/MATLAB_R2015b.app/bin/maci64/libfreetype.6.dylib
%
%
% Windows:
% --------
%
% On MS-Windows with GNU/Octave the libraries are bundled with Octave,
% no need for you to do anything.
%
% On MS-Windows with Matlab you will need to install the GStreamer multi-
% media framework - see "help GStreamer" for installation instructions -
% otherwise Psychtoolbox will use the old lower quality GDI text renderer
% instead.
%
% More background info about Psychtoolbox's standard text renderer:
% =================================================================
%
% On OSX one can still select Apple's CoreText text renderer via the command
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
% The FontConfig library is used to find the optimal font and font
% settings, given a specific font specification by your user code, a process
% known as "font matching". FontConfig internally uses the FreeType-2
% library to handle the font files on your system and to gather all needed
% information for the matching process.
%
% After a font and settings have been selected, FreeType-2 is used to load
% the font and convert it into high-quality character glyphs, then the
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
