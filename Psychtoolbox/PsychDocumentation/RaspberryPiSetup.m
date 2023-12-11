% RaspberryPiSetup -- How to setup your Pi optimally for Psychtoolbox.
%
% There are two major families of the RaspberryPi with features suitable for
% meaningful use for visual and auditory stimulation with Psychtoolbox-3, the
% older variants RaspberryPi 2 and 3, and the new RaspberryPi 4/400/5 family.
%
% Psychtoolbox is currently only tested with up to date versions of the
% official RaspberryPi Linux operating system distribution known as
% "RaspberryPi OS" (RPi OS), also known under its former name "Raspbian".
% Only the old/legacy 32-Bit RPi OS version 11 is supported by Psychtoolbox
% at the moment, as provided directly by us via DownloadPsychtoolbox et
% al., even on 64-Bit processors. Currently the most recent stable RPi OS
% 11 + GNU/Octave is what PTB will support, with some wiggle room, as we do
% not keep close track of development and updates to RPi OS. RPi OS itself
% also provides versions of Psychtoolbox, installable via "sudo apt install
% octave-psychtoolbox-3", and 64-Bit versions of RPi OS would ship such
% packages as 64-Bit packages. Other Linux distributions for RaspberryPi,
% like Ubuntu for RaspberryPi, will also ship Psychtoolbox packages as
% 32-Bit or 64-Bit variants via the sudo apt install octave-psychtoolbox-3
% method. While these packages will likely work just fine, these are
% currently not tested by the Psychtoolbox developers, or supported in case
% of questions or problems. These packages are maintained by the
% NeuroDebian maintainers, and they are usually quite a bit out of date
% wrt. the current PTB beta releases, as the Linux distributions usually
% ship whatever PTB was the most recent version at a time sometime before
% the distributions was released. Those packages do not get updates after
% initial distribution release, not even bug fixes. E.g., RPi OS at late
% 2022 ships some Psychtoolbox 3.0.17 variant, whereas PTB betas are now at
% version 3.0.19 or later.
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
%   to reach 100 Hz or a bit more. The outputs are in principle 10 bit and 12 bit
%   deep color capable, assuming enough video bandwidth, but 12 bit output is not
%   yet that useful, as of December 2023 and Linux 6.1, as gamma tables are not yet
%   supported, only identity passthrough of framebuffer pixels, and the maximum
%   resolution of the framebuffer is 10 bpc. Therefore choosing 12 bit output is
%   not yet of practical measurable benefit, and just a waste of video bandwidth.
%
%   So far only single-display operation is tested, due to lack of a 2nd
%   micro-HDMI adapter cable.
%
%   The new VideoCore-6 gpu has slightly higher performance than the older VideoCore
%   4 gpu in RPi 2 and 3. More importantly, the gpu supports full 32 bit floating
%   point framebuffers and textures, with the restriction that framebuffer blending /
%   alpha-blending is only supported on 16 bit floating point framebuffers. This
%   opens up many new use cases for more complex visual stimuli, e.g., it makes
%   AdditiveBlendingForLinearSuperpositionTutorial.m demo work, as well as
%   MorphDemo and MorphTextureDemo, FDFDemo, GarboriumDemo and ProceduralGarboriumDemo
%   and various other complex shader-based or alpha-blending based stimuli.
%   It also allows to take full advantage of high-end visual stimulators
%   and display devices from VPixx, if you are inclined enough to drive a
%   multi-thousand dollar stimulator with a 50$ microcomputer. Ofc. while
%   accuracy is excellent, performance is more limited than with a desktop
%   or laptop graphics card from Intel, AMD or NVidia.
%
%   Testing showed that stimuli generated without use of alpha-blending can
%   be output at the full 16 bit or 14 bit precision of VPixx or CRS visual
%   stimulators, while stimuli created via alpha blending with 16 bit
%   floating point framebuffers can achieve up to 11 bit precision,
%   depending on specific stimulus.
%
%   Output precision without special stimulators to regular displays is
%   limited to 8 bpc on current RPi OS 11 and RPi OS 12, as of December 2023.
%
%   However, on RaspberryPi 4 and later (tested with RaspberryPi 400) you
%   can achieve 10 bpc output (via the usual XOrgConfCreator setup), if you
%   manually install Mesa version 23.3.1 or later, e.g., by compiling it
%   yourself, or by getting it via PiKISS or similar tools from a 3rd party
%   source.
%
%   Movie playback of full HD video at 24 fps - 30 fps, and up to 50 fps
%   stable and almost 60 fps without sound is possible on the RaspberryPi
%   4/400, and probably at higher stable fps on a RaspberryPi 5, with
%   suitably encoded movies and proper playback settings. For playback of
%   H264 encoded movies, the builtin H264 hardware decoder is used by
%   default, but testing on RPi 400 showed that forcing use of a software
%   decoder may provide higher performance! See specialFlags1 setting 4 for
%   that. You must specify an explicit 'pixelFormat' of 6 in
%   Screen('OpenMovie'); for this to work at good performance for high
%   resolution movies. At default settings movie playback would be very
%   slow, e.g., 1-2 fps for full HD content. Additionally, the H264
%   hardware decoder is picky wrt. proper content encoding. While footage
%   e.g., from YouTube or other professionally encoded sources works, badly
%   encoded footage may not. Many other movie formats play back with
%   software decoding, at potentially lower performance. To summarize: Good
%   performance playback of HD content at good framerates works if you do
%   everything right wrt. playback parameters and encoding. Otherwise your
%   mileage may vary both wrt. performance and reliability.
%
%   It is also possible to enable experimental Vulkan display support,
%   although this is of no meaningful benefit right now (as of December
%   2023), and just yields you lower performance and broken visual
%   stimulation timing and timestamps. The way to enable this is to follow
%   the following instructions on how to use PiKISS to install an up to
%   date Vulkan driver for the RPi 4+. It is important to install a driver
%   from Mesa 23.1 or later:
%   https://qengineering.eu/install-vulkan-on-raspberry-pi.html
%   After that, you can launch Octave from the command line with the use of
%   the zink OpenGL driver via the following:
%   MESA_LOADER_DRIVER_OVERRIDE=zink octave
%
%
%   If you use Psychtoolbox with RPi OS, these setup steps are strongly recommended:
%
%   1. Update your RPi OS to the latest version "sudo apt update" and "sudo apt upgrade".
%
%   2. Make sure the Linux kernel is at least of version 6.1 or later. If "uname -a"
%      reports a lower version, upgrade the kernel via "sudo rpi-update" and some reboot.
%      This step is generally not needed on RPiOS 12 and later, it ships with a recent
%      enough kernel already.
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
%      Note: This step seems to be no longer needed on RPiOS 12, as it is already configured
%      to these settings.
%
%   4. On RPi OS 12, when running on a RaspberryPi 4 / 400 or a later model, a Wayland
%      display server (named "wayfire") is used by default, instead of a XOrg X-Server.
%      This is currently incompatible with Psychtoolbox or any other vision science
%      toolkit. You have to manually switch back to a desktop GUI which uses the native
%      XOrg X-Server via the following steps:
%
%      a) In a terminal type: sudo raspi-config
%      b) Navigate to Advanced Options > "A6 Wayland - Switch between Wayland and X backends"
%      c) Select "W1 X11 - OpenBox window manager with X11 backend"
%      d) Confirm with OK.
%      e) Select Finish to exit the configuration tool.
%      f) Reboot the Raspberry Pi.
%
%      You also need to disable use of xcompmgr to avoid tearing and bad visual timing and
%      Psychtoolbox sync failures with warnings a la "pageflipping not being used for flips":
%
%      In a terminal type the following to remove xcompmgr:
%      killall xcompmgr; sudo apt remove xcompmgr
%
%      This will make visual stimulus timing work for fullscreen windows,
%      and remove tearing. Regular windows on the desktop will tear though,
%      and transparency for Psychtoolbox windows will not work. If you want
%      a tear-free desktop and also working transparency for Psychtoolbox
%      windows, you can install Mutter as a better X11 compositor.
%
%      To switch to Mutter as a much better desktop compositor:
%      sudo apt install mutter
%      Edit the desktop config file via: sudo pico /etc/xdg/lxsession/LXDE-pi/desktop.conf
%      Change the following line
%           window_manager=openbox-lxde-pi
%      into
%           window_manager=mutter
%      Save the file via CTRL+o + Return.
%      logout and login again to make the change to mutter effective.
%
%   5. Legacy RPi OS 11 setup steps: (Not needed on RPi OS 12 and later)
%
%      On the RaspberryPi 1/2/3, you must disable use of xcompmgr to avoid tearing and
%      bad visual timing (as far as we know not needed for RaspberryPi 4/400):
%
%      In a terminal type: sudo raspi-config
%      Navigate to Advanced Options > Compositor > xcompmgr composition manager
%      Choose No
%      Reboot the Raspberry Pi.
%
%   6. If Psychtoolbox complains angrily about "broken timing" and "pageflipping not
%      being used", and/or if you observe flicker/tearing artifacts or other display
%      artifacts, it is a good idea to run XOrgConfCreator and XOrgConfSelector to
%      install a custom xorg.conf file, which will work around a current limitation
%      in RPi OS on some hardware, e.g., RaspberryPi 1/2/3. Logout and login again,
%      or reboot the RaspberryPi, for the changes to take effect.
%
%   With all setup steps performed properly, and a reboot for good measure,
%   visual stimulation should work very well, with robust, trustworthy and
%   sub-millisecond accurate visual stimulus onset timestamps, as verfied
%   by us with external measurement equipment.
%
%   In general, the RaspberryPi microcomputers are quite capable devices
%   for neuroscience testing, even for many vision science paradigms. Their
%   builtin gpio support makes them especially nifty if one needs to
%   receive or send TTL triggers or other i/o.
%
%   Further support for improvements to Psychtoolbox wrt. RaspberryPi
%   devices, and user support will be contingent on labs financially
%   supporting our work. Until then, consider this work being done on a low
%   effort basis.
%
