% GStreamer - Installation instructions for the GStreamer media framework.
%
% Psychtoolbox uses the GStreamer multi-media framework for all multi-media
% related operations.
%
% All movie playback, movie creation, video capture and video recording
% operations are based on GStreamer. These functions won't work without a
% working GStreamer installation on your system (with the exception of video
% capture from firewire DCAM/IIDC machine vision cameras on Linux and OSX).
%
% You will need at least version 0.10.24 of GStreamer, but we recommend to
% use the latest available versions of the version 0.10.x series, as not
% all Psychtoolbox features are fully supported by version 0.10.24.
% Especially video capture, video recording and movie creation may be
% limited in functionality or performance if you don't use a recent version
% of GStreamer. Do not install GStreamer version 1.0 or later! We need 0.10
% and won't work with 1.0 yet.
%
% GStreamer is a free software / open-source multimedia framework. The
% summary on its webpage tells us:
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
% ABI stable."
%
% In summary, GStreamer is for the free software world what Quicktime is
% for Apple's "walled garden". It is used a the standard media framework
% for many (most?) Linux desktop systems. It is also used on many Linux
% based embedded devices.
%
%
% Installation instructions:
%
%
% GNU/Linux: Any half-way recent Linux distribution will include support
% for GStreamer in its package management system, so you can easily install
% it via the software management tool of your system. If you install PTB
% via NeuroDebian then most GStreamer packages will get automatically
% installed, with exception of some optional packages, so read on.
%
% You will need at least the core GStreamer runtime and the gstreamer-base
% set of plugins, but we recommend to install all available plugins for
% full functionality and performance. Modern desktop distros usually
% already ship with these.
%
% You may need to install some restricted plugin packages to play back all
% common audio and video file formats like MP3 and MP4. Video or movie
% recording with high quality (DivX, H.264) will also require recent
% versions of additional plugin packages which contain support for these
% formats. These are usually not installed by default due to licensing and
% patent clauses in place for some territories. You may want to
% specifically add them to your system depending on your format needs.
%
% E.g., on Debian or Ubuntu:
%
%     sudo apt-get install gstreamer0.10-plugins-bad-multiverse
%     gstreamer0.10-plugins-ugly-multiverse
%
% Ubuntu 14.04-LTS removed the GStreamer FFMPEG plugins from the standard
% distribution, so you will need to install those plugins from a 3rd
% party multi-media PPA. Do so by performing these additional setup steps:
%
% sudo apt-add-repository ppa:mc3man/trusty-media
% sudo apt-get update
% sudo apt-get install gstreamer0.10-ffmpeg
%
%
% MS-Windows and Apple MacOSX:
%
% You must install GStreamer if you want to use multi-media functions! If
% you don't intend to use multi-media functions then installation of
% GStreamer is optional on MS-Windows, but possibly not on OSX.
%
% Download and install the latest GStreamer runtimes from:
%
% <http://www.gstreamer.com>
%
% When the installer asks you to select the components it should install,
% select a "Custom installation" (instead of "Full installation" or "Basic
% installation" or such). Then, in the displayed check list of packages to
% install, select *all* components manually, if you want support for all
% video formats and all functionality. Without this, many popular video
% formats like H264 video will not play at all, or video recording / video
% capture and similar functions will not work. In fact, even our own demos,
% e.g., SimpleMovieDemo *will fail* if you don't have all codecs installed!
% -> If SimpleMovieDemo doesn't work, then the most likely cause is that
% you didn't select all GStreamer packages for installation, so restart the
% installer and repeat installation with the full set of packages.
%
%
% On Apple MacOSX, optionally you can also install GStreamer version 0.10
% via the Homebrew package manager, building the very latest GStreamer from
% source code. This is more effort and takes more time for download and
% installation, but may provide some extra features for video capture and
% possibly enhanced performance for video playback of high-resolution,
% high-framerate movies:
%
% Get Homebrew at ...
%
% http://mxcl.github.com/homebrew
%
% then add the Homebrew-Versions repository via typing (in your terminal window):
%
% brew tap homebrew/versions
%
% ... then install all components of the GStreamer 0.10 series via:
%
% brew install gstreamer010 gst-plugins-base010 gst-plugins-good010
% gst-plugins-bad010 gst-plugins-ugly010
%
% Then install our own FFMPEG GStreamer formula via:
%
% brew install gst-ffmpeg010
%
% ... or if that doesn't work, try the following alternative:
%
% brew install https://raw.github.com/Psychtoolbox-3/homebrew-versions/gst-ffmpeg010/gst-ffmpeg010.rb
%
% After a couple minutes of download and compile time, you'll have a fully
% functional GStreamer compiled from source - assuming everything goes
% well, that is.
%
