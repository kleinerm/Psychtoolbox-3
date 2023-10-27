% GStreamer - Installation instructions for the GStreamer media framework.
%
% Psychtoolbox uses the GStreamer multi-media framework for all multi-media
% related operations. Always on MS-Windows, and on macOS when using Matlab,
% GStreamer is also needed for high quality text rendering via Screen('DrawText').
%
% All movie playback, movie creation, video capture and video recording
% operations are based on GStreamer. These functions won't work without a
% working GStreamer installation on your system (with the exception of
% video capture from firewire and USB-Vision DCAM/IIDC machine vision
% cameras on Linux, where high-performance multi-camera capture works via
% libdc1394, see 'help VideoCaptureDC1394', and GStreamer is only needed
% for video recording, not for live capture).
%
% You will need at least version 1.8 of GStreamer on Linux, and at least
% version 1.22.0 on MS-Windows and at least version 1.18.5 on macOS, but
% we recommend to use the latest stable release of the version 1 series
% tested by us (see links below).
%
% For limited HDR playback support on Linux, you will need at least GStreamer
% version 1.16 + some special modifications to your playback scripts, e.g., as
% contained in Ubuntu 20.04-LTS. For full HDR playback support on Linux, you will
% need at least version 1.18, e.g., as contained in Ubuntu 22.04-LTS. HDR playback
% is not yet supported on macOS due to macOS operating system deficiencies - an
% insufficiently advanced Apple OpenGL implementation.
%
% Installation instructions:
%
%
% GNU/Linux:
%
% Any decent Linux distribution will include support for GStreamer-1 in its
% package management system, so you can easily install it via the software
% management tools of your system. If you install PTB via NeuroDebian, then
% most GStreamer packages will get automatically installed, with the
% possible exception of some optional packages with potential license or
% patent restrictions, so read on.
%
% You may need to install those extra plugin packages to play back all
% common audio and video file formats like MP3 and MP4. Video or movie
% recording with high quality (DivX, H.264) may also require recent
% versions of additional plugin packages which contain support for these
% formats. These may not installed by default due to licensing and patent
% clauses in place for some territories on this planet. You may want to
% specifically add them to your system depending on your format needs.
%
% An easy test is to run SimpleMovieDemo. If it fails or only plays sound,
% but not video, then some of the plugins are missing, e.g., the important
% "gst-libav" plugins. For minimal HDR playback support, Ubuntu 20.04 LTS
% would suffice. For full HDR playback support, you need Ubuntu 22.04 LTS,
% or some 3rd party provided GStreamer 1.18+ packages, or packages built
% from source.
%
%
% MS-Windows and Apple macOS:
%
% macOS: You must install GStreamer if you want to use multi-media functions or
% if you want to use the high-quality text renderer with Matlab, which provides
% consistent, fast, high-quality text rendering, instead of the lower quality,
% officially unsupported, legacy Apple CoreText text renderer.
%
% If you don't intend to use such functionality then installation of GStreamer
% is optional on macOS. Screen will work normally, but abort with an error
% message if you try to use any multi-media functions.
%
% MS-Windows: You must install GStreamer, regardless if you want to use multi-media
% functionality or not, as the Screen() mex file won't work at all without GStreamer
% installed! You must install at least version 1.22.0 of the 64-Bit MSVC variant of
% GStreamer.
%
% Here is a FAQ entry on resolving GStreamer problems:
%
% <https://github.com/Psychtoolbox-3/Psychtoolbox-3/wiki/FAQ#how-to-resolve-gstreamer-problems>
%
% NOTE: If GStreamer complains about not being able to find some plugins,
% e.g., the video playback doesn't work because there is a GStreamer
% configuration problem, then it can help to delete the GStreamer plugin
% registry to force a rebuild of that database. E.g., under Windows-10, if
% your login user name would be Bob, you would probably have to delete the
% file:
%
% 'C:\Users\Bob\AppData\Local\Microsoft\Windows\INetCache\gstreamer-1.0\registry.x86_64-msvc.bin'
%
%
% Download and install the latest 64-Bit ("x86_64") GStreamer runtimes from:
%
% <http://gstreamer.freedesktop.org/download/>
%
% The following GStreamer runtime packages have been lightly tested for compatibility
% with Psychtoolbox 3.0.19.4, as of September 2023:
%
% For MS-Windows: 64-Bit Intel MSVC runtime v1.22.5 for use with 64-Bit Matlab/Octave.
%
% <https://gstreamer.freedesktop.org/data/pkg/windows/1.22.5/msvc/gstreamer-1.0-msvc-x86_64-1.22.5.msi>
%
% You *must* install the MSVC variants of GStreamer, not the MinGW64 variants!
%
%
% For Apple macOS: Oldest runtime v1.18.5:
%
% <https://gstreamer.freedesktop.org/data/pkg/osx/1.18.5/gstreamer-1.0-1.18.5-x86_64.pkg>
%
% also tested, probably higher performance and more modern, but known to crash with
% VideoRecordingDemo.m at least with the MacBookPro 2017 builtin camera, is v1.22.1.
%
% <https://gstreamer.freedesktop.org/data/pkg/osx/1.22.1/gstreamer-1.0-1.22.1-universal.pkg>
%
% When the installer asks you to select the components it should install,
% select a "Full installation" or "Complete installation", or if those
% options do not exist then select a "Custom installation" (instead of a
% "Basic installation" or such). Then, if you chose a "Custom installation"
% in the displayed list of packages to install, select *all* components
% manually, if you want support for all video formats and all
% functionality. Without this, many popular video formats like H264 video
% will not play at all, or video recording / video capture and similar
% functions may not work. In fact, even our own demos, e.g.,
% SimpleMovieDemo *will fail* if you don't have all codecs installed! -> If
% SimpleMovieDemo doesn't work, then the most likely cause is that you
% didn't select all GStreamer packages for installation, so restart the
% installer and repeat installation with the full set of packages.
%
