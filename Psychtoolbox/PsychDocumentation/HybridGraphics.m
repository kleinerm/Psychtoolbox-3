% HybridGraphics -- Support for hybrid graphics laptops.
%
% Please check the Psychtoolbox website and Wiki for up to date
% information about the state of HybridGraphics. This document
% may contain obsolete information quite quickly on this fast
% changing topic, especially on Linux.
%
% BACKGROUND:
% -----------
%
% Hybrid graphics laptops are laptops which have two built-in
% graphics cards (GPUs). One card is usually an integrated
% graphics chip (iGPU) which has a low power consumption and
% heat production, allowing for a cool laptop and long battery
% runtime, but it also has relatively low performance. This chip
% is sufficient for typical desktop GUI use, watching videos and
% other light graphics applications. The 2nd card, called discrete
% GPU (dGPU), provides much higher performance but consumes more
% power and produces more heat, so the laptop runs hotter and/or
% needs more cooling, all resulting in shorter battery runtimes.
%
% The idea is to use the dGPU for performance hungry graphics
% intense applications, and the iGPU for day to day tasks,
% selecting either for high performance or long battery runtimes.
%
% These laptops come in two basic flavors, hardware multiplexed
% (muxed) and non-multiplexed (muxless). Muxed laptops have an
% electronic switch that can connect the iGPU or dGPU to the video
% outputs (internal flat panel and external video connectors like
% MiniDisplayPort or HDMI or DVI or VGA). The switch is software
% controlled and allows selection of which gpu is driving the
% displays, the other gpu is powered down for maximum power saving.
%
% macOS, Linux on Apple hardware and on other muxed Laptops:
% ----------------------------------------------------------
%
% As of 2021, all past and current dual-gpu laptops from Apple (Intel
% MacBookPro line with NVidia or AMD gpu) are muxed. When using macOS, the
% operating system controls the mux to select an appropriate gpu. For light
% desktop and 2D use, the iGPU is active. Whenever a 3D application starts
% up, e.g., Psychtoolbox, the iGPU gets disconnected and powered down and
% the dGPU gets powered up and connected. From the perspective of
% Psychtoolbox there essentially is only one GPU, which is the dGPU, either
% a NVidia or AMD graphics card. Apart from a couple of rather horrible
% bugs, e.g., for the 2010 MacBookPro with some recent versions of macOS,
% this means that hybrid graphics under macOS usually "just works".
%
% If one uses an Apple MacBookPro under Linux then the machine will run
% with either the dGPU active and the Laptop behaves like a machine with
% one gpu, or the active gpu can be switched via the Linux "vgaswitcheroo"
% mechanism. The same applies for other PC laptops which are equipped with
% a mux. Iow. one can manually select the performance vs. power consumption
% tradeoff.
%
% Most modern common PC laptops are muxless though. The iGPU is hard-wired
% to the video outputs, both to the laptop flat panel and the external
% outputs. The iGPU is always active and drives the displays and takes care
% of drawing the GUI and handling 2D applications. The dGPU can be powered
% up as needed to handle GPGPU computations and the rendering for more
% demanding 2D, Video and 3D applications. As the dGPU is not connected to
% the display outputs, it has to copy its rendered images into the RAM of
% the iGPU and the iGPU then displays the images on behalf of the dGPU.
% This involves some significant overhead: Multiple milliseconds of time
% are needed for each Screen('Flip') to copy image data from the dGPU to
% the iGPU, and converting the data into a format the iGPU can display. For
% this reason, display latency on a muxless laptop will always be longer
% and absolute graphics performance lower than on a laptop which only has a
% dGPU of the same model, or on a muxless laptop. A big problem is the need
% to properly synchronize the rendering of the dGPU with the display of the
% iGPU. Depending on how this synchronization or handshake is implemented,
% visual stimulus onset timing and timestamping can be highly unreliable
% and inaccurate.
%
% WINDOWS:
% --------
%
% On Microsoft Windows a handshake method is used which maintains good
% framerates for video games and similar applications, but causes visual
% stimulus onset timing and timestamping to be almost always completely
% wrong, with observed errors in the range of +/- 33 msecs on a 60 Hz
% panel. That means that the dGPU is unusable if visual timing matters in
% any way. The best you can do on a muxless laptop under Microsoft Windows
% is to configure the driver to disable the dGPU and only use the iGPU for
% all rendering, and then hope that the iGPU graphics driver isn't too
% buggy, a hope that is often unjustified, especially if the iGPU is an
% Intel graphics chip. Intel graphics has timing bugs itself quite often on
% MS-Windows, even on regular single gpu laptops.
%
% LINUX:
% ------
%
% On Linux, as of December 2021, good progress has been made in
% implementing methods which provide both good performance *and* reliable,
% trustworthy, accurate visual timing and timestamping. Some - but not all!
% - types of Laptop hardware should work well, but for all of them some
% special configuration or software upgrades are needed.
%
% We recommend XServer version 1.20.11 or later, and Mesa version 21.0.3 or
% later, and Linux 5.11 or later, as this combination provides best
% performance and ease of setup for all supported types of hybrid graphics
% laptops. Users of Ubuntu Linux can simply install Ubuntu 20.04.3 LTS from
% fresh installation media, or upgrade to 20.04.3 LTS from earlier Ubuntu
% releases and then install the new hardware enablement stack (HWE) via ...
%
% sudo apt install --install-recommends linux-lowlatency-hwe-20.04
%
% ... if it isn't already automatically installed after an upgrade to 20.04.3 LTS.
%
% The following sections describe the current level and quality of support
% for different types of hybrid graphics laptops, and required
% configuration steps, assuming you have sufficiently up to date kernel,
% X-Server and Mesa as explained in the previous paragraph. Psychtoolbox
% would tell you if you need to upgrade your kernel, if you'd run it on a
% muxless hybrid graphics Laptop.
%
%
% * Laptops with an Intel iGPU combined with a NVidia dGPU ("NVidia Optimus" models):
%
%   These should work perfectly if you use the "nouveau" open-source
%   graphics driver, at least as far as testing with three different
%   laptops went. Stimuli are displayed without any artifacts, and timing
%   and timestamping is accurate and trustworthy. Performance is highly
%   dependent on the model of NVidia gpu though, with the latest
%   generations currently providing only relatively low performance. Using
%   the nouveau open-source driver will probably provide no performance
%   gain over just using the Intel iGPU for any NVidia gpu of the Maxwell
%   gpu family (GeForce 800 series) or later, so you would need the
%   proprietary driver to take advantage of those later models.
%
%   For using such gpu's with the NVidia proprietary driver, choose a
%   driver of version 495.44 or later. It may work with earlier drivers,
%   e.g., maybe even from the 450 series, but was only "tested" with the
%   495.44 driver. Linux 5.11 and X-Server 1.20.11 or later is strongly
%   recommended. Iow. Ubuntu 20.04.3-LTS will HWE stack and all updates
%   applied.
%
%   On Ubuntu, the "nvidia-settings" GUI tool allows you - in the PRIME
%   profiles section - to switch between "high performance" NVidia
%   graphics, standard "power efficient" Intel graphics. and "on demand"
%   Optimus mode. You should select "On demand" and reboot. The command
%   line tool "prime-select ondemand" does the same selection. This may
%   also work with the other selections, but may require custom xorg.conf
%   files in that case.
%
%   Now you should be able to choose if you want to use the Intel iGPU or
%   the NVidia dGPU for rendering:
%
%   Starting "octave" or "matlab" you would get to use the Intel iGPU, low
%   power consumption, full PTB feature set, but lower performance.
%
%   Starting "octave' or "matlab" with the following prefix...
%
%   __NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia octave
%   or
%   __NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia matlab
%
%   should enable use of the NVidia high performance gpu, with most PTB
%   features working fine.
%
%   Stimuli should display without any artifacts and timing and timestamping
%   should be accurate and trustworthy. Performance should be good. Note
%   the "should" qualifier, as due to lack of a modern machine with Intel
%   iGPU + NVidia dGPU this is not actually tested, but deduced from code
%   review and some transfer of testing results from some experimental
%   hardware setup.
%
%   This procedures for use of Optimus / Prime render offload is needed
%   when your display device is only connected to the Intel iGPU on a
%   muxless laptop. For displays directly connected to the NVidia dGPU, you
%   would probably have to use the NVidia in PRIME "Performance" mode, or
%   simply without the special launch command above.
%
%
% * Laptops with an Intel iGPU combined with an AMD dGPU ("AMD Enduro" models):
%
%   These should work very well out of the box on Ubuntu 20.04.3 LTS and
%   later, as explained above.
%
%   On other Linux distributions, make sure to install Linux 4.8.11 or
%   later versions of the Linux kernel, together with X-Server 1.18 or
%   later, and Mesa version 17.0 or later.
%
%   AMD Enduro hybrid graphics was tested with three PC setups:
%
%   - Intel HD "Haswell desktop" graphics chip + AMD Radeon R9 380 Tonga Pro.
%   - Intel HD "Ivybridge desktop" graphics chip + AMD FireGL "Cedar".
%   - Intel HD 630 "Kabylake GT2" graphics chip + AMD Radeon Pro 560 "Polaris11"
%
%   Stimuli are displayed without any artifacts and timing and timestamping
%   is accurate and trustworthy. Performance is good.
%
%
% * Laptops with dual NVidia gpus NVidia iGPU + NVidia dGPU:
%
%   Muxless would not work with any current official solution [1]. However,
%   i am not aware of any recent muxless laptops - or any such muxless
%   laptops actually - which use dual-NVidia gpus. All known dual-NVidia
%   laptops are rather old (around year 2010 or earlier) and use a hardware
%   mux, so Linux "vgaswitcheroo" mechanism can be used to switch between
%   gpus for perfect results.
%
%
% * Laptops with dual AMD gpus AMD iGPU + AMD dGPU ("AMD Enduro" models):
%
%   Muxless is untested due to lack of suitable AMD dual-gpu hardware, but
%   according to code review should work on the recommended Linux 5.11 and
%   later for AMD iGPU's of type "Stoney", "Carrizo", "Raven2", "Renoir",
%   "Picasso", "VanGogh" and later. Essentially most integrated graphics
%   chips (APU's) in laptops or PC's with modern AMD Ryzen processors,
%   except for early models which use RavenRidge 1st generation iGPU's.
%   Check product specs, but in general Ryzen 5 2000/3000 series machines
%   may have the problematic RavenRidge iGPU's, whereas recent Ryzen 7 4000
%   series may have more modern and suitablg iGPU's
%
%   Stimuli should display without any artifacts and timing and timestamping
%   should be accurate and trustworthy. Performance should be good.
%
%
% * Laptops with AMD iGPU + NVidia dGPU ("NVidia Optimus" models):
%
%   Psychtoolbox 3.0.18.2 and later has some support for this. Setup is the
%   same as in the section above for Intel iGPU + NVidia dGPU. However, due
%   to current limitations in the AMD Linux display driver amdgpu-kms as of
%   at least Linux 5.16, proper pageflipping and scanout is not yet
%   supported. Psychtoolbox will need to use (and force-enable) the desktop
%   compositor of your Ubuntu/Gnome/KDE desktop GUI to get proper high
%   quality pictures on the display. It will try some hackery to try to get
%   proper visual stimulus onset timing and timestamping, but the
%   reliability of stimulus onset timestamps may be not quite as good and
%   100% trustworthy as when you use an Intel iGPU + NVidia dGPU. Some
%   tests suggest it mostly works ok, at least on KDE, GNOME and Ubuntu
%   desktop of Ubuntu 20.04.3-LTS, but there could be edge cases where
%   timestamps could be off by one video refresh durations - unknown if
%   this really can happen due to some limitations of my measurement
%   equipment. Also, performance will be limited to a maximum framerate
%   that is half the video refresh rate of the display. E.g., on a standard
%   60 Hz display, you won't be able to achieve more than 30 fps!
%
%   If you use the Gnome or Ubuntu desktop GUI (or any GUI which uses the Gnome
%   "Mutter" X11 compositor and window manager), you can try the following
%   command setenv('PSYCH_EXPERIMENTAL_NETWMTS', '1') to opt-in into some highly
%   experimental mode which makes timing reliable and trustworthy, however while
%   potentially reducing framerate. Additional downsides are that currently only
%   single-window use with Screen('Flip') will work, also no more fancy things like
%   flip events, async flips, frame-sequential stereo or VRR. This is a construction site!
%
%   I also have developed experimental patches to the Linux amdgpu driver to
%   lift all these limitations on AMD to get the same excellent performance and
%   reliability as with Intel iGPU's, but the current patches are hacky and
%   unlikely to get accepted into official Linux kernels in their current
%   state. Time permitting i will try to get those integrated properly.
%   Meanwhile, these only exist on my development machine and i haven't
%   decided on the conditions under which these may be made available to
%   Psychtoolbox users. The only sure thing is that it won't happen free of
%   cost, as this work requires a lot of time and money.
%
%
% For those combinations that should work (Intel iGPU + NVidia/AMD dGPU
% "Optimus/Enduro", and AMD iGPU + AMD dGPU), after you've upgraded to all
% the required software, the following setup steps are needed for muxless
% PRIME mode. Note that these *do not apply* to Optimus with the proprietary
% graphics driver from NVidia, for which setup was explained above:
%
% 1. Run the "XOrgConfCreator" script to create a proper XOrg configuration file,
%    and then "XOrgConfSelector" to switch to that configuration file, logout and
%    login again.
%
% 2. Tell Matlab or Octave to use the dGPU for rendering with Psychtoolbox:
%
%    - Either use the setenv('DRI_PRIME','1'); command before calling the first Screen()
%      command, e.g., by adding it to Matlabs startup.m script or Octaves ~/.octaverc
%      startup script.
%
%    - Or copy the Psychtoolbox/PsychHardware/LinuxX11ExampleXorgConfs/_.drirc to the file
%      ~/.drirc and then customize it for your hardware to always select the dGPU for
%      rendering with Octave or Matlab. The file itself contains customization instructions.
%      The <device> ... <\device> section in that file can also be included into the
%      global /etc/drirc file if it should apply to all users on a machine.
%
% 3. Optionally verify the handshaking and synchronization actually works.
%    Psychtoolbox must not report any timing or timestamping related errors
%    or warnings, or other warnings relating to hybrid graphics problems.
%    Typical tests like PerceptualVBLSyncTest or VBLSyncTest must work
%    correctly. All demos should display without any visual artifacts,
%    tearing artifacts etc.
%
%    Additionally you can use the Linux ftrace script i915_optimus_trace.sh
%    in the Psychtoolbox/PsychHardware/ folder. Instructions on how to use
%    it are inside the script. Running it while a Psychtoolbox stimulation
%    script runs will measure the timing of functions relevant for proper
%    timing. The printout after 20 seconds should show the function
%    "reservation_object_wait_timeout_rcu" using a significant amount of
%    time, e.g., multiple thousand microseconds (usecs), e.g.,
%
%    3)               |  intel_mmio_flip_work_func [i915]() {
%    3) # 3060.318 us |    reservation_object_wait_timeout_rcu();
%    3) # 3070.039 us |  }
%
%    Here the iGPU waited for 3060.318 usecs until the dGPU was done with
%    its part of the job. That's a realistic waiting time for simple visual
%    stimuli, although numbers could easily go up into the > 9 msecs range
%    for more demanding stimuli or slower gpus. Just to give you a
%    perspective on the potential performance loss or added latency
%    compared to a single gpu laptop.
%
% [1] There also exist some muxless Laptop models where the iGPU is
%     hard-wired to the internal Laptop flat panel, whereas the dGPU is
%     hard-wired to (some of) the external video outputs. On these models
%     one can configure a dual-x-screen setup for visual stimulation and
%     then assign the iGPU to drive X-Screen 0 on the internal panel and
%     assign the dGPU to drive X-Screen 1 on the external video outputs.
%
%     This would work with high performance and timing precision even on
%     hybrid graphics laptops which otherwise wouldn't work, e.g., dual
%     NVidia or dual AMD laptops. Such a setup wouldn't require any of the
%     setup steps mentioned above. Instead it would require to create a
%     dual-x-screen setup via XOrgConfCreator, but then to manually
%     customize the created config file, as XOrgConfCreator can't
%     automatically handle such dual-gpu setups yet. Ask for assistance on
%     the Psychtoolbox user forum if you happen to have such a laptop. One
%     example xorg.conf file for handling such a setup (Intel iGPU + AMD
%     dGPU) can be found under the name
%     xorg.conf_SeparateScreensDualGPUIntelAndAMD in the
%     Psychtoolbox/PsychHardware/LinuxX11ExampleXorgConfs/ folder. It would
%     need customization though for a given Laptop, specifically adapting
%     the "BusID" parameter for your hardware.
%
%     Another example X-Config file for such a laptop can be found for year
%     2016 Razer Blade gaming laptop with Intel HD-530 Skylake iGPU +
%     NVidia GeForce 1060M Pascal dGPU, where the Intel iGPU is hardwired
%     to the laptop panel and USB-C output, whereas the NVidia dGPU is
%     hardwired to a HDMI output. The filename in the
%     Psychtoolbox/PsychHardware/LinuxX11ExampleXorgConfs/ folder is
%     xorg.conf_RazerBlade-2-XScreens-intel+nouveau
%
%     Another X-Config example file for the Razer Blade 2016 is the file
%     ...
%     xorg.conf_RazerBlade-2-XScreens-NVidiaProprietary_iGPUPrime+dGPUHDMInative.conf
%     ... This file is for use with the NVidia proprietary driver instead
%     of the nouveau open-source driver. It uses the NVidia gpu to drive
%     two separate X-Screens 0 and 1. X-Screen 0 is driven via Optimus
%     PRIME output, displaying via the Intel HD 530 iGPU on either the
%     laptop flatpanel or the USB-C video output, but not both at the same
%     time if visual stimulation timing matters. X-Screen 1 is driven
%     directly via the HDMI output connected to the NVidia dGPU.
%
%     NOTE: If you copy these files into the /etc/X11/xorg.conf.d/ folder you must rename them to
%           end with the suffix .conf otherwise they won't be actually used!
