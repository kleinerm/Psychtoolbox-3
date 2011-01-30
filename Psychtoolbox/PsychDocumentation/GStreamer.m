% GStreamer - Installation instructions for the GStreamer media framework.
%
% Psychtoolbox in all versions for GNU/Linux, and in (future) 64 bit
% versions for Mac OS/X and MS-Windows uses the GStreamer multi-media
% framework for all multi-media related operations.
%
% All movie playback, movie creation, video capture and video recording
% operations are based on GStreamer. These functions won't work without a
% working GStreamer installation on your system.
%
% You will need at least version 0.10.24 of GStreamer.
%
% By default, the 32-bit versions of Psychtoolbox for Windows and MacOS/X
% use Apple's Quicktime for those operations, so GStreamer isn't needed
% there. You can however use GStreamer (once installed) instead of
% Quicktime if you add the following command to the top of your experiment
% script, before any other Screen() command:
%
% Screen('Preference', 'OverrideMultimediaEngine', 1);
%
% GStreamer (Webpage web: http://www.gstreamer.net ) is a free software /
% open-source multimedia framework. The summary on its webpage summarizes
% it as follows:
%
% "GStreamer is a library for constructing graphs of media-handling
% components. The applications it supports range from simple Ogg/Vorbis
% playback, audio/video streaming to complex audio (mixing) and video
% (non-linear editing) processing.
%
% Applications can take advantage of advances in codec and filter
% technology transparently. Developers can add new codecs and filters by
% writing a simple plugin with a clean, generic interface.
%
% License: GStreamer is released under the LGPL. The 0.10 series is API and
% ABI stable.
%
% In summary, GStreamer is for the free software world what Quicktime is
% for Apple's "walled garden". It is used a the standard media framework
% for many (most?) Linux desktop systems, netbooks, tablets and MeeGo,
% WebOS (and also Android) based tablets and Smartphones. It is also used
% on embedded devices.
%
% While GStreamer itself is very high performance, mature and stable,
% Psychtoolbox's support is in its early stage, so don't be surprised if you
% find bugs or limitations. Report them on the forum instead, so we can fix
% them.
%
% Installation instructions:
%
% GNU/Linux: Any half-way recent Linux distribution will include support
% for GStreamer in its package management system, so you can easily install
% it via the software management tool of your system.
%
% You will need at least the core GStreamer runtime and the gstreamer-base
% set of plugins. A simple way to get GStreamer at least on Ubuntu Linux is
% to install the "rhythmbox" or "totem" multimedia-players. You may need to
% install additional plugin packages to play back all common audio- and
% video file formats.
%
% E.g., a "sudo apt-get install totem" typed into a terminal window,
% followed by your administrator password, should do the trick. Most likely
% this isn't needed though, as you likely already have GStreamer installed
% if you ever used to playback any multimedia content.
%
% MS-Windows:
%
% We currently provide GStreamer support on Windows with 32-bit Matlab V7.4
% (R2007a) and later only. This is a sneak-preview for people interested in
% testing and giving feedback, not neccessarily for the average user.
%
% You can download and run a standard MS-Windows setup.exe installer from:
%
% web: http://code.google.com/p/ossbuild/
%
% For best results, you'd currently (beginning December 2010) choose this
% one: "GStreamer WinBuilds v0.10.6, LGPL (x86)"
%
% As our normal Screen() file (due to technical difficulties) doesn't
% contain GStreamer support on Windows, you'll need to use the
% ScreenGS.mexw32 file inside
% Psychtoolbox/PsychBasic/MatlabWindowsFilesR2007a/ instead. Copy it into
% a folder outside Psychtoolbox, rename it to "Screen.mexw32" and then
% add that folder with the renamed Screen.mexw32 somewhere at the top of
% your Matlab path, so it takes precedence over our standard Screen file.
% You will need to repeat this procedure after each update of Psychtoolbox
% and confirm you picked the right file by typing "which Screen" in Matlab
% to see if it points to your special file.
%
% OS/X:
%
% No support for GStreamer yet. Stay tuned!
%
%
