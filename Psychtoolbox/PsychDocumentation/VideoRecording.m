% VideoRecording - Parameter settings, howtos and tips for Video recording.
%
% This file describes how to use the GStreamer and DC1394 video capture engines
% for video (and audio) recording into movie files. The GStreamer engine is
% available on all operating systems and can record both video and sound.
% The DC1394 engine is available on Linux and OSX and can currently only
% record video, but no simultaneous audio.
%
% - Check VideoRecordingDemo for regular video recording via GStreamer! On
%   both MacOSX and MS-Windows, one often needs to pass special settings or
%   codec types for video recording and especially combined video + audio
%   recording to work. That demo illustrates at least one set of settings
%   which were shown to work on OSX and Windows in December 2013.
%
% - Check VideoDVCamCaptureDemo for video recording from DV cameras. These
%   cameras seem to need special treatment on all systems, but especially
%   on MS-Windows and the demo shows how to do that.
%
% - Check VideoMultiCameraCaptureDemo for video capture and recording from
%   multiple professional class IIDC/DCAM compliant firewire and USB cameras
%   on Linux and OSX via the DC1394 engine.
%
% Codec and parameter selection:
%
%
% If you only want to record a movie with default settings, specify its
% filename as 'targetmoviename' parameter to Screen('OpenVideocapture', ...);
%
% If you want to specify codecs and their settings as well, it is recommended
% that you specify these settings in the 'targetmoviename' and specify the
% moviefile name separately via the Screen('SetVideoCaptureParameter') interface,
% e.g., to set the movie name to 'foo.avi' you'd use this command:
% Screen('SetVideoCaptureParameter', grabber, 'SetNewMoviename=foo.avi');
%
% If you want to use the default codec(s) with their default settings,
% simply omit the parameter string. Psychtoolbox has a list of codecs that
% it tries to use for video and audio encoding, together with reasonable
% default parameters. It works its way down the list, starting with the most
% suitable/efficient codec, until it finds a codec that is supported on
% your system. Not all codecs may be installed by default on your operating
% system. Especially proprietary, non-free, or patent-encumbered codecs may
% not be installed on your system. You may have to select them explicitely
% in the software center of your distribution (see "help GStreamer").
%
% If you only want to specify settings for the automatically chosen default
% codec, start the parameter string with ':CodecSettings=', followed by
% parameters.
%
% If you want to chose a specific codec and (optionally) its settings,
% start the parameter string with ':CodecType=', followed by the settings.
%
% Some video codecs are supported by our automatic setup code. These will
% automatically select matching audio codecs and audio-video multiplexers
% and reasonable default settings, so they are convenient for you to use:
%
% x264enc:         A highly optimized H264 video encoder, automatically combined with the
%                  faacenc or ffenc_aac MPEG-4 AAC audio encoder and an AVI file
%                  multiplexer or Quicktime mov file multiplexer.
%
% xvidenc:         The XVid MPEG-4 video encoder + AAC audio in a AVI file.
%
% ffenc_mpeg4:     Another MPEG-4 video encoder + AAC audio in a AVI file.
%
% theoraenc:       The Ogg Theora video encoder with Ogg Vorbis audio encoder and
%                  Ogg file format multiplexer (.ogv files).
%
% vp8enc_webm:     The VP-8 video codec with Ogg Vorbis audio in a WEBM (.webm)
%                  video container (HTML-5 video).
%
% vp8enc_matroska: As above, but in a matroska file container.
%
% ffenc_h263p:     H.263 video encoder with AAC audio in a Quicktime container.
%
% yuvraw:          Raw, uncompressed YUV video data with AAC audio in a avi container.
%
% huffyuv:         Huffman encoded YUV raw video data + AAC audio in a
%                  matroska container. This is a lossless video codec, but
%                  it creates relatively large files.
%
% ffenc_sgi:       Stores video as a sequence of RLE compressed, lossless
%                  encoded SGI image files. This format usually can only be
%                  read and played back by Psychtoolbox itself and some
%                  specialized tools. It also creates relatively large
%                  files.
%
% The huffyuv and ffenc_sgi encoders are mostly useful if you need
% bit-exact image storage or storage of raw video sensor data (Bayer color
% filter format), or of high bit depths video data, ie., with more than 8
% bpc. For most common use cases you can achieve qualitatively good enough
% results at much smaller file sizes with the standard lossy codecs.
%
% Psychtoolbox supports high-level settings, which are relatively easy to
% use and understand. We describe these first. Psychtoolbox also supports
% low-level tweaking of codec specific settings, which require significant
% knowledge about video and audio encoding and lots of tinkering, but
% provide fine grained control over every tiny aspect of the recording
% process.
%
% High level settings are not specific to a movie file format or choice of
% codec. All settings are accepted for all formats and codecs and mapped to
% corresponding format and codec specific low level settings, or they are
% silently ignored if a specific file/codec combination doesn't support a
% high level setting. These settings are the most frequently choosed
% settings.
%
% High level settings are specified as Keyword=value pairs, without a blank
% between the Keyword= and the value, and the first letter of the Keyword
% being a capital letter.
%
% E.g.: ':CodecSettings= Keyframe=5 Videoquality=0.5' would encode using
% the default video codec with a video encoding quality of 0.5 aka 50% and
% a keyframe distance of maximum 5 frames.
%
% ':CodecType=xvidenc Keyframe=10 Videobitrate=1000' would choose the
% "xvidenc" XVid video codec with a keyframe interval of 10 frames and a
% average video bitrate of 1000 kilobits/sec.
%
% The following high level parameters are supported:
%
% Video encoding settings:
% ------------------------
%
% Interlaced=0/1 - Tell codec if input video material is interlaced. This
%                  allows to optimize encoding further for smaller file size
%                  and better quality.
%
% Keyframe=x     - Set the maximum keyframe interval to at most 'x' frames.
%                  Most video players can only seek/navigate in a video with
%                  keyframe granularity. E.g., a keyframe setting of 10 would
%                  mean that one can only move forward/backward or address a
%                  specific target video frame with a stepsize or accuracy of
%                  10 frames. Lower numbers mean better navigation accuracy,
%                  with 1 providing frame accurate positioning, and possibly
%                  higher quality, but bigger video file sizes and potentially
%                  higher computational load. If omitted, the codec selects
%                  keyframe distances automatically, possibly dynamically,
%                  depending on visual scene content.
%
% Videobitrate=x kb/s  - Select video bit rate in kilobits per second. A direct
%                        control of the tradeoff filesize vs. quality. Bigger
%                        numbers mean higher quality and larger files. Codecs
%                        interpret this number as target average rate or maximum
%                        rate, depending on codec. Many codecs select this
%                        parameter dynamically (variable bitrate coding VBR).
%
% Videoquality=x - A value between 0.0 and 1.0 to select target video quality
%                  between 0% and 100%. This controls different aspects of the
%                  encoding process, but bigger values mean higher quality and
%                  sometimes bigger filesize and sometimes higher cpu processor
%                  load during recording. If you have a slow computer and a
%                  demanding video format (high resolution, high framerate) you
%                  may need to lower video quality, so your poor machine can
%                  cope.
%
% Audio encoding settings:
% ------------------------
%
% Audioquality=x - See Videoquality, this time for the audio encoding.
%
% Audiobitrate=x kb/s  - See Videobitrate, this time for the audio encoding.
%
% Multiplexer / File format settings:
% -----------------------------------
%
% Timeresolution=x - How fine should time be resolved in the recorded footage?
%                    A value of x means to divide 1 second into x units, i.e.,
%                    provide a time granularity of 1/x th of a second. This
%                    influences file size, accuracy of time based navigation in
%                    the video, possibly the accuracy of returned movie presentation
%                    timestamps and of audio-video sync. If omitted, defaults to
%                    1/1000 th second aka 1 msec granularity.
%
% Faststart=0/1    - If set to 1, optimize recorded files for a fast load and start
%                    of playback in players. This is usually what you want, so it is
%                    on by default.
%
% Bigfiles=0/1     - If set to 1, allow recording of movie files with a size greater
%                    than 2 GB. This is usually what you want, but it may cause
%                    compatibility problems with playback software which can't handle
%                    the "big file" file format.
%
%
% Specifying low-level settings:
% ------------------------------
%
% For more control you can also specify the various codec types and their
% low level settings in the syntax of the "gst-launch" GStreamer command
% line utility. This disables use of the high-level settings and provides
% full control:
%
% A video codec type and settings string is prefixed with: 'VideoCodec='
% followed by codec name and settings, postfixed with ':::', e.g.,
% 'VideoCodec=x264enc speed-preset=1 noise-reduction=100000 :::'.
%
% A audio codec type and settings follows the same logic, with the
% 'AudioCodec=' prefix, e.g., 'AudioCodec=faac :::'
%
% A multiplexer is chosen via the 'Muxer=' prefix, but no low-level
% settings can be passed to the multiplexer, only high-level settings as
% described above.
%
% A specific (non-auto-selected) audio source and its settings can be
% chosen via the 'AudioSource=' prefix, e.g., 'AudioSource=pulsesrc :::' to
% select an audio input provided by the PulseAudio sound server explicitely.
%
% A full example string to select codecs and other low level settings would
% look like this:
% ':CodecType=VideoCodec=x264enc speed-preset=1 noise-reduction=100000 ::: AudioCodec=faac ::: AudioSource=pulsesrc ::: Muxer=avimux'
%
% Intermixing of high-level and low-level settings is also possible:
% ':CodecType=x264enc Keyframe=1 Videobitrate=8192 AudioCodec=alawenc ::: AudioSource=pulsesrc ::: Muxer=qtmux'
% This would select the x264enc H264 video codec, with high level settings,
% and additionally choose (low-level) a specific audio codec, audio source
% and multiplexer.
%
% You can find a list of supported codecs, sources and multiplexers by
% typing 'gst-inspect' in a Unix or Windows terminal window. You can
% list the available low-level settings (aka properties) via gst-inspect codecname.
%
% E.g., to list all available low-level properties of the x264enc codec,
% you'd type "gst-inspect x264enc" in a terminal window.
%
% Please note that if you don't choose a Psychtoolbox supported video codec
% from the list provided above, then you will need to specify all audio
% codec and multiplexer settings manually, as Psychtoolbox doesn't know
% which audio codecs or muxers to use with a video codec unknown to it.
%

% History:
% 5.6.2011  mk  Created.
%
