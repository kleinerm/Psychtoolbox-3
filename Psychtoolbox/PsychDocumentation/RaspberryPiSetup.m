% RaspberryPiSetup -- How to setup your Pi optimally for Psychtoolbox.
%
% There are two major families of the RaspberryPi with features suitable for
% meaningful use for visual and auditory stimulation with Psychtoolbox-3, the
% older variants RaspberryPi 2 and 3, and the new RaspberryPi 4/400 family.
%
% Psychtoolbox is currently only tested with up to date versions of the official
% RaspberryPi Linux operating system distribution known as "RaspberryPi OS", also
% known under its former name "Raspbian". Only 32-Bit Raspbian is supported by
% Psychtoolbox as provided directly by us via DownloadPsychtoolbox et al., even
% on 64-Bit processors. Whatever is the most recent stable Raspbian + GNU/Octave
% is what PTB will support, with some jiggle room, as we do not keep close track
% of development and updates to Raspbian. Raspbian itself also provides versions
% of Psychtoolbox, installable via "sudo apt install octave-psychtoolbox-3", and
% 64-Bit versions of Raspbian would ship such packages as 64-Bit packages. Other
% Linux distributions for RaspberryPi, like Ubuntu for RaspberryPi, will also
% ship Psychtoolbox packages as 32-Bit or 64-Bit variants via the sudo apt install
% octave-psychtoolbox-3 method. While these packages will likely work just fine,
% these are currently not tested by the Psychtoolbox developers, or supported in
% case of questions or problems. These packages are maintained by the NeuroDebian
% maintainers, and they are usually quite a bit out of date wrt. the current PTB
% beta releases, as the Linux distributions usually ship whatever PTB was the most
% recent version at a time sometime before the distributions was released. Those
% packages do not get updates after initial distribution release, not even bug
% fixes. E.g., Raspbian at late 2022 ships some Psychtoolbox 3.0.17 variant,
% whereas PTB betas are now at version 3.0.19 or later.
%
%
% The RaspberryPi 2B is tested for compatibility with Psychtoolbox. The test system
% is a RaspberryPi 2B with 1 GB of RAM. Other models of RaspberryPi 2 and 3 are
% expected to work equally well (or better for the more powerful RaspberryPi 3),
% but not tested. It provides:
%
% - 40 pin digital i/o, e.g., for TTL trigger emission or reception, hardware
%   interfacing etc. See RaspberryPiGPIODemo.m for how to use it.
%
% - Working stereo audio output with ok latency and good timing via the headphone
%   jack. Tested and verified by use of a VPixx Datapixx. Working audio output or
%   input via UAC compliant USB audio cards.
%
% - Working video capture with external USB connected web cams.
%
% - Single display support via HDMI for up to 1920x1080 pixels full HD, with 8 bit
%   of color precision, for standard 24 bit color depth / 16.8 Millions of colors,
%   or 256 levels of red, green, blue, grey. No support for floating point framebuffers
%   or textures, and thereby no support for stimulus drawing or post-processing
%   which would require such buffers.
%
%
% The RaspberryPi 4, and its keyboard form-factor variant RaspberryPi 400, apart
% from providing more RAM and faster processor and graphics, do support:
%
% - 40 pin digital i/o, e.g., for TTL trigger emission or reception, hardware
%   interfacing etc. See RaspberryPiGPIODemo.m for how to use it. Untested so far,
%   but should work the same as on the RaspberryPi 2 and 3.
%
% - Presumably working stereo output like the RPi 2 and 3 on models with headphone
%   jack. This is untested by us so far, but reasonable to assume it is working.
%
% - Audio output via HDMI audio: This is tested to be working for video playback
%   with the Chrome webbrowser, and with Psychtoolbox built-in GStreamer based
%   movie/video/audio playback engine. You can play back movies with sound, or
%   sound files in common formats like .wav or .mp3 or .ogg audio etc., whatever
%   is supported by GStreamer's collection of plugins. It also works with our
%   PsychPortAudio driver.
%
% - Working audio output or input via UAC compliant USB audio cards.
%
% - Working video capture with external USB connected web cams.
%
% - Dual-display support via two micro-HDMI connectors up to 4k UHD resolution,
%   but 4k 3840x2160 resolution is limited to 30 Hz refresh rate by default. A
%   2560x1440 resolution works at 60 Hz, something like 1920x1080 should be able
%   to reach 100 Hz or a bit more.
%
%   So far tested is single-display operation, due to lack of a 2nd micro-HDMI
%   adapter cable.
%
%   The new VideoCore-6 gpu has slightly higher performance than the older VideoCore
%   4 gpu in RPi 2 and 3. More importantly, the gpu supports full 32 bit floating
%   point framebuffers and textures, with the restriction that framebuffer blending/
%   alpha-blending is only supported on 16 bit floating point framebuffers. This
%   opens up many new use cases for more complex visual stimuli, e.g., it makes
%   AdditiveBlendingForLinearSuperpositionTutorial.m demo work, as well as
%   MorphDemo and MorphTextureDemo, FDFDemo, GarboriumDemo and ProceduralGarboriumDemo
%   and various other complex shader-based or alpha-blending based stimuli. It also
%   allows to take full advantage of high-end visual stimulators and display devices
%   from VPixx, if you are inclined enough to drive a multi-thousand dollar stimulator
%   with a 50$ microcomputer. Ofc. while accuracy is excellent, performance is more
%   limited than with a desktop or laptop graphics card from Intel, AMD or NVidia.
%
%   Testing showed that stimuli generated without use of alpha-blending can be output
%   at the full 16 bit or 14 bit precision of VPixx or CRS visual stimulators, while
%   stimuli created via alpha blending with 16 bit floating point framebuffers can
%   achieve up to 11 bit precision, depending on specific stimulus.
%
%   Output precision without special stimulators to regular displays are still
%   limited to 8 bpc maximum, as of Linux 6.2 in February 2023.
%
%   Movie playback of full HD video at 24 fps is possible with suitably encoded
%   movies and proper playback settings. For playback of H264 encoded movies, the
%   builtin H264 hardware decoder of the RaspberryPi 4 is used. You must specify
%   an explicit 'pixelFormat' of 6 in Screen('OpenMovie', ...., pixelFormat); for
%   this to work at good performance for high resolution movies. At default settings
%   movie playback would be very slow, e.g., 1-2 fps for full HD content. Additionally,
%   the H264 hardware decoder is picky wrt. proper content encoding. While footage
%   e.g., from YouTube or other professionally encoded sources works, badly encoded
%   footage may not. Regrettably our own included demo movie DualDiscs.mov, as used
%   by SimpleMovieDemo and PlayMoviesDemo by default, does not work at all, the
%   decoder chokes on it. Many other movie formats play back with software decoding,
%   at potentially lower performance. To summarize: Good performance playback of HD
%   content at good framerates works if you do everything right wrt. playback parameters
%   and encoding. Otherwise your mileage may vary both wrt. performance and reliability.
%
%   It is also possible to enable experimental Vulkan display support, although this
%   is of no meaningful benefit right now (as of March 2023), and just yields you lower
%   performance and broken visual stimulation timing and timestamps. The way to enable
%   this is to follow the following instructions on how to use PiKISS to install an
%   up to date Vulkan driver for the RPi 4+. It is important to install a driver from
%   Mesa 23.1 or later: https://qengineering.eu/install-vulkan-on-raspberry-pi.html
%   After that, you can launch Octave from the command line with the use of the
%   zink OpenGL driver via the following: MESA_LOADER_DRIVER_OVERRIDE=zink octave
%
%
%   If you use Psychtoolbox with Raspbian, these setup steps are strongly recommended:
%
%   1. Update your Raspbian to the latest version "sudo apt update" and "sudo apt upgrade".
%
%   2. If you are on a RaspberryPi 4 or 400, make sure the Linux kernel is at least of
%      version 5.10 or later. If "uname -a" reports a lower version, upgrade the kernel
%      via "sudo rpi-update" and some reboot. This step is not needed for the RPi 2 and 3.
%
%   3. Switch from the firmware kms video driver to the proper fully open-source kms
%      video driver. For this, edit the file /boot/config.txt - the section at the end,
%      then reboot. The section should look like this on a RaspberryPi 1 / 2 / 3 / 4 / 400:
%
%      dtoverlay=vc4-kms-v3d
%      max_framebuffers=2
%
%      It is important that the dtoverlay parameter should be vc4-kms-v3d instead
%      of vc4-fkms-v3d.
%
%   4. On the RaspberryPi 4 / 400 with editions of Raspbian / RaspberryPi OS older than
%      February 2021, you should either update your OS to the latest, and reboot, or run
%      PsychLinuxConfiguration, XOrgConfCreator, and XOrgConfSelector to install a custom
%      xorg.conf file, which will work around a bug in Raspbian. Reboot! If the bug is
%      present and this workaround from step 4 is not applied, Psychtoolbox will complain
%      angrily about "pageflipping not being used for flips", visual stimulation timing
%      will be grossly wrong, and you will observe massive tearing artifacts, flicker and
%      other stimulus anomalies.
%
%      On the RaspberryPi 1/2/3, you must disable use of xcompmgr to avoid tearing and
%      bad visual timing:
%
%      In a terminal type: sudo raspi-config
%      Navigate to Advanced Options > Compositor > xcompmgr composition manager
%      Choose No
%      Reboot the Raspberry Pi.
%
%      Also run XOrgConfCreator, and XOrgConfSelector to install a custom
%      xorg.conf file, which will work around a current limitation in Raspbian,
%      as of November 2022. Logout and login again, or reboot the RaspberryPi.
%
%   With all setup steps performed properly, and a reboot for good measure, visual
%   stimulation should work very well, with robust, trustworthy and sub-millisecond
%   accurate visual stimulus onset timestamps, as verfied by us with external measurement
%   equipment.
%
%
%   In general, the RaspberryPi microcomputers are quite capable devices for neuroscience
%   testing, even for many vision science paradigms. Their builtin gpio support makes them
%   especially nifty if one needs to receive or send TTL triggers or other i/o.
%
%   Further support for improvements to Psychtoolbox wrt. RaspberryPi devices, and
%   user support will be contingent on labs financially supporting our work. Until
%   then, consider this work being done on a low effort basis.
%
