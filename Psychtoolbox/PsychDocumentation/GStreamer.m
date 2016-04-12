% GStreamer - Installation instructions for the GStreamer media framework.
%
% Psychtoolbox uses the GStreamer multi-media framework for all multi-media
% related operations. On Windows with Matlab, GStreamer is also needed for
% high quality text rendering via Screen('DrawText').
%
% All movie playback, movie creation, video capture and video recording
% operations are based on GStreamer. These functions won't work without a
% working GStreamer installation on your system (with the exception of video
% capture from firewire DCAM/IIDC machine vision cameras on Linux and OSX).
%
% You will need at least version 1.0 of GStreamer, but we recommend to
% use the latest available stable release of the version 1 series.
%
% Installation instructions:
%
%
% GNU/Linux:
%
% Any recent 2013 Linux distribution will include support for GStreamer-1 in its
% package management system, so you can easily install it via the software
% management tools of your system. If you install PTB via NeuroDebian, then most
% GStreamer packages will get automatically installed, with the possible exception
% of some optional packages with potential license or patent restrictions, so read on.
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
% "gst-libav" plugins.
%
%
% MS-Windows and Apple OSX:
%
% You must install GStreamer if you want to use multi-media functions!
% You must also install GStreamer if you want to use the high-quality text
% renderer on Windows with Matlab, instead of the lower quality legacy GDI
% text renderer.
%
% If you don't intend to use such functionality then installation of
% GStreamer is optional. Screen will work normally, but abort with an error
% message if you try to use any multi-media functions. For use with Octave-4 on
% Windows you must install GStreamer regardless if you want to use multi-media
% functionality or not, as the Screen() mex file won't work at all without GStreamer
% installed.
%
% Download and install the latest 64-Bit ("x86_64") GStreamer runtimes from:
%
% <http://gstreamer.freedesktop.org/download/>
%
% You should check for and install the latest runtime packages available for your
% system for best reliability and performance. However, as a convenience, at time
% of this writing (April 2016) the required downloads would be:
%
% For MS-Windows: 64-Bit Intel runtime v1.8.0 for use with 64-Bit Matlab/Octave.
%
% <http://gstreamer.freedesktop.org/data/pkg/windows/1.8.0/gstreamer-1.0-x86_64-1.8.0.msi>
%
% For MS-Windows: 32-Bit Intel runtime v1.8.0 for use with 32-Bit GNU/Octave-4.
%
% <http://gstreamer.freedesktop.org/data/pkg/windows/1.8.0/gstreamer-1.0-x86-1.8.0.msi>
%
% For Apple OSX: Runtime v1.8.0
%
% <http://gstreamer.freedesktop.org/data/pkg/osx/1.8.0/gstreamer-1.0-1.8.0-x86_64.pkg>
%
%
% When the installer asks you to select the components it should install,
% select a "Custom installation" (instead of "Full installation" or "Basic
% installation" or such). Then, in the displayed check list of packages to
% install, select *all* components manually, if you want support for all
% video formats and all functionality. Without this, many popular video
% formats like H264 video will not play at all, or video recording / video
% capture and similar functions may not work. In fact, even our own demos,
% e.g., SimpleMovieDemo *will fail* if you don't have all codecs installed!
% -> If SimpleMovieDemo doesn't work, then the most likely cause is that
% you didn't select all GStreamer packages for installation, so restart the
% installer and repeat installation with the full set of packages.
%
%
% On Apple MacOSX, optionally you can also install GStreamer version 1.4 or later
% via the Homebrew package manager, building the very latest GStreamer from
% source code. This is more effort and takes more time for download and
% installation, also it is so far completely untested for compatibility with
% Psychtoolbox by us. At this point in time there are no known functional
% advantages in using the HomeBrew build instead of the binary packages from
% gstreamer.net, but whatever fancies you...
%
% 1. Get Homebrew at <http://brew.sh>
% 2. Then install GStreamer 1 and its plugins like this:
%
% brew install gstreamer gst-plugins-base gst-plugins-good gst-plugins-bad gst-plugins-ugly gst-libav
%
