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
% Hybrid graphics laptops are laptops which have two built in
% graphics cards (GPUs). One card is usually an integrated
% graphics chip (iGPU) which has a low power consumption and
% heat production, allowing for a cool laptop and long battery
% runtime, but it also has relatively low performance. This chip
% is sufficient for typical desktop GUI use, watching videos and
% other light graphics applications. The 2nd card, called discrete
% GPU (dGPU) provides much higher performance but consumes more
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
% OSX, Linux on Apple hardware and on other muxed Laptops:
% --------------------------------------------------------
%
% As of 2016, all past and current laptops from Apple (MacBookPro
% line) are muxed. When using Mac OSX, the operating system controls
% the mux to select an appropriate gpu. For light desktop and 2D use,
% the iGPU is active. Whenever a 3D application starts up, e.g.,
% Psychtoolbox, the iGPU gets disconnected and powered down and the
% dGPU gets powered up and connected. From the perspective of
% Psychtoolbox there essentially is only one GPU, which is the dGPU,
% either a NVidia or AMD graphics card. Apart from a couple of rather
% horrible bugs, e.g., for the 2010 MacBookPro with some recent versions
% of OSX, this means that hybrid graphics under OSX usually "just works".
%
% If one uses an Apple MacBookPro under Linux then the machine will run
% with either the dGPU active and the Laptop behaves like a machine with
% one gpu, or the active gpu can be switched via the Linux "vgaswitcheroo"
% mechanism. The same applies for other PC laptops which are equipped
% with a mux. Iow. one can manually select the performance vs. power
% consumption tradeoff.
%
% Most modern common PC laptops are muxless. The iGPU is hard-wired to
% the video outputs, both to the laptop flat panel and the external outputs.
% The iGPU is always active and drives the displays and takes care of drawing
% the GUI and handling 2D applications. The dGPU can be powered up as needed
% to handle GPGPU computations and the rendering for more demanding 2D, Video
% and 3D applications. As the dGPU is not connected to the display outputs, it
% has to copy its rendered images into the RAM of the iGPU and the iGPU then
% displays the images on behalf of the dGPU. This involves some significant
% overhead: Multiple milliseconds of time are needed for each Screen('Flip') to
% copy image data from the dGPU to the iGPU, converting the data into a format
% the iGPU can display. For this reason, display latency on a muxless laptop
% will always be longer and absolute graphics performance lower than on a laptop
% which only has a dGPU, or on a muxless laptop. A big problem is the need to
% properly synchronize the rendering of the dGPU with the display of the iGPU.
% Depending on how this synchronization or handshake is implemented, visual
% stimulus onset timing and timestamping can be highly unreliable and inaccurate.
%
% WINDOWS:
% --------
%
% On Microsoft Windows a handshake method is used which maintains good framerates
% for video games and similar applications, but causes visual stimulus onset timing
% and timestamping to be almost always completely wrong, with observed errors in
% the range of +/- 33 msecs on a 60 Hz panel. That means that the dGPU is unuseable
% if visual timing matters in any way. The best you can do on a muxless laptop under
% Microsoft Windows is to configure the driver to disable the dGPU and only use the
% iGPU for all rendering, and then hope that the iGPU graphics driver isn't too buggy,
% a hope that is often unjustified, especially if the iGPU is an Intel graphics chip.
% Intel graphics has timing bugs itself quite often on MS-Windows, even on regular
% single gpu laptops.
%
% LINUX:
% ------
%
% On Linux, as of December 2016, good progress has been made in implementing methods
% which provide both good performance *and* reliable, trustworthy, accurate visual
% timing and timestamping. Some - but not all! - types of Laptop hardware should
% work well, but for all of them some special configuration or software upgrades
% are needed:
%
% You always need at least XServer version 1.18 or later, and Mesa 11.2 or later.
% Some earlier software versions might work in some cases, but these are
% completely untested by us and therefore in no way guaranteed to work.
%
% Ubuntu Linux 14.04.5 LTS with its latest hardware enablement stack IV, or even
% better Ubuntu 16.04 LTS, do provide sufficiently modern versions of these software
% components. Additionally you will need one of the most recent Linux kernels:
% At least Linux 4.6 if you have an Intel integrated graphics chip combined with a
% NVidia gpu or with an older AMD gpu. Modern AMD gpus will require the even more
% modern Linux 4.8.11 kernel or later.
%
% As of December 2016, Linux 4.8.11 would be recommended as the most modern Linux kernel
% to cover all bases. See the download and installation instructions under section [2] below.
%
% The autumn 2016 updates of Linux distributions, e.g., Ubuntu 16.10, did ship with
% Linux 4.8 by default, requiring no special software updates anymore, unless you use
% an Intel iGPU with one of the latest generation AMD dGPU's (Linux 4.8.11 required),
% or an Intel iGPU with a NVidia graphics card *and* the proprietary NVidia display
% driver instead of the open-source nouveau driver. Psychtoolbox will tell you if you
% need to upgrade your kernel, if you run it on a hybrid graphics Laptop.
%
%
% * Laptops with an Intel iGPU combined with a NVidia dGPU ("NVidia Optimus" models):
%
%   These should work perfectly if you use the "nouveau" open-source graphics driver,
%   at least as far as testing with two different laptops went. Stimuli are displayed
%   without any artifacts and timing and timestamping is accurate and trustworthy.
%   Performance is highly dependent on the model of NVidia gpu though, with the latest
%   generations currently providing only relatively low performance, and ongoing work
%   to improve the performance for recent models.
%
%   If you want to use the NVidia proprietary display driver for Linux instead, there
%   now exists a solution which works with correct timing and timestamping. However, the
%   solution is less flexible and power-efficient than use of the "nouveau" open-source
%   driver. It also requires substantial manual setup work, and it needs XOrg X-Server 1.19.0
%   or later. In practice this means you either need to use a Linux distribution which
%   uses X-Server 1.19, which in December 2016 is essentially "Fedora 25" or "Debian unstable",
%   or you will have to wait for the Ubuntu 17.04 release in April 2017, or you will have
%   to download and compile your own X-Server 1.19 if you are not afraid of compilers and
%   Makefiles and willing to spend a workday doing this.
%
%   Once you have a X-Server 1.19 up and running, you will need the NVidia proprietary
%   display drivers of version 375.20 or later for 64-Bit Intel processors. Then you need
%   to copy various configuration files into various places, and adapt some of these files
%   to your specific system. Finally you need to install a custom xf86-video-modesetting
%   display driver onto your system. This modesetting driver is specifically made to
%   interoperate with Psychtoolbox to provide research grade precision timing and
%   timestamping. Then, after a reboot, you may be rewarded with a NVidia Optimus laptop
%   which can efficiently use your discrete high-performance NVidia gpu with research grade
%   timing. However, research grade timing is only provided for pure single-display setups,
%   not for any kind of multi-display operation. That means your Laptop can have exactly
%   one display enabled, either the Laptops internal flat-panel, or one externally connected
%   display. Also timing is only reliable and trustworthy for a Psychtoolbox fullscreen
%   window. As you can see, the setup is somewhat limited and inflexible, and setting it up
%   requires quite a bit of effort as of December 2016. You will find all the needed config
%   files and custom made display driver and setup instructions in the Psychtoolbox
%   subdirectory Psychtoolbox/PsychHardware/LinuxDrivers/NVidiaOptimus/
%
%   See the following thread for the current state of the NVidia proprietary implementation
%   and for some more nice background info on the challenges of proper handshaking and
%   synchronization on muxless laptops:
%
%   https://devtalk.nvidia.com/default/topic/957814/linux/prime-and-prime-synchronization
%
%
% * Laptops with an Intel iGPU combined with an AMD dGPU ("AMD Enduro" models):
%
%   These should work very well on Ubuntu 16.10 or with Linux 4.6 and later if you use
%   the open source graphics drivers with slightly older models of AMD GPUs, ie. not the
%   very latest models of the "Volcanic Islands" / GCN 1.2 GPU family. Specifically, GPUs
%   using the classic open-source display driver "radeon-kms" will work with perfectly
%   trustworthy timing and quality.
%
%   GPUs of the "Volcanic Islands" GCN 1.2 generation, which use the new open-source
%   "amdgpu-kms" driver, will currently need you to manually upgrade your Linux kernel to
%   at least Linux 4.8.11. See section [2] below on how to do that.
%
%   These results are based on testing with two PC setups:
%
%   - Intel HD "Haswell desktop" graphics chip + AMD Radeon R9 380 Tonga Pro. Perfect when
%     used with Linux 4.8.11 or later kernels. (amdgpu-kms)
%
%   - Intel HD "Ivybridge desktop" graphics chip + AMD FireGL "Cedar". Perfect under all
%     conditions with Linux 4.6 and later. (radeon-kms).
%
%   Stimuli are displayed without any artifacts and timing and timestamping is accurate and
%   trustworthy.
%
%   You will get acceptable performance out of the box. For good performance you will either
%   need Mesa version 13 or later, or you need to set the R600_DEBUG environment variable to
%   'forcedma', ie., execute setenv('R600_DEBUG','forcedma'); at the very start of your Octave
%   or Matlab session, e.g., from the ~/.octaverc script or Matlabs startup.m script. Better yet,
%   you can install the current Mesa 13.1 development prototype for Ubuntu 16.04 LTS flavors and
%   derivatives from this ppa:
%
%   https://launchpad.net/~paulo-miguel-dias/+archive/ubuntu/mesa/+packages
%
%
% * Laptops with dual NVidia gpus NVidia iGPU + NVidia dGPU:
%
%   Muxless won't work with any current official solution [1]. However, i am not aware of
%   any recent muxless laptops - or any such muxless laptops actually - which use dual-NVidia
%   gpus. All known dual-NVidia laptops are rather old and use a hardware mux, so Linux
%   "vgaswitcheroo" mechanism can be used to switch between gpus for perfect results.
%
%
% * Laptops with dual AMD gpus AMD iGPU + AMD dGPU ("AMD Enduro" models):
%
%   Muxless won't work with any current official solution, so you can only use the
%   AMD iGPU [1]. A "dirty hack" solution exists from Mario Kleiner for old AMD laptops,
%   but it is not released yet, and not clear if releasing this hack would be a good idea.
%   Check the PTB website for updates or contact the user forum if you have questions wrt.
%   this solution.
%
%
% For those combinations that should work (Intel iGPU + NVidia/AMD dGPU "Optimus/Enduro"),
% after you've upgraded to all the required software, the following setup steps are
% needed:
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
% 3. Verify the handshaking and synchronization actually works. Psychtoolbox must not
%    report any timing or timestamping related errors or warnings, or other warnings
%    relating to hybrid graphics problems. Typical tests like PerceptualVBLSyncTest or
%    VBLSyncTest must work correctly. All demos should display without any visual artifacts,
%    tearing artifacts etc.
%
%    Additionally you can use the Linux ftrace script i915_optimus_trace.sh in the
%    Psychtoolbox/PsychHardware/ folder. Instructions on how to use it are inside the
%    script. Running it while a Psychtoolbox stimulation script runs will measure the
%    timing of functions relevant for proper timing. The printout after 20 seconds should
%    show the function "reservation_object_wait_timeout_rcu" using a significant amount of
%    time, e.g., multiple thousand microseconds (usecs), e.g.,
%
%    3)               |  intel_mmio_flip_work_func [i915]() {
%    3) # 3060.318 us |    reservation_object_wait_timeout_rcu();
%    3) # 3070.039 us |  }
%
%    Here the iGPU waited for 3060.318 usecs until the dGPU was done with its part
%    of the job. That's a realistic waiting time for simple visual stimuli, although
%    numbers could easily go up into the > 9 msecs range for more demanding stimuli or
%    slower gpus. Just to give you a perspective on the potential performance loss or
%    added latency compared to a single gpu laptop.
%
% [1] There also exist some muxless Laptop models where the iGPU is hard-wired to
%     the internal Laptop flat panel, whereas the dGPU is hard-wired to the external
%     video outputs. On these models one can configure a dual-x-screen setup for visual
%     stimulation and then assign the iGPU to drive X-Screen 0 on the internal panel and
%     assign the dGPU to drive X-Screen 1 on the external video outputs. This would
%     work with high performance and timing precision even on hybrid graphics laptops
%     which otherwise wouldn't work, e.g., dual NVidia or dual AMD laptops. Such a setup
%     wouldn't require any of the setup steps mentioned above. Instead it would require to
%     create a dual-x-screen setup via XOrgConfCreator, but then to manually customize the
%     created config file, as XOrgConfCreator can't automatically handle such dual-gpu setups
%     yet. Ask for assistance on the Psychtoolbox user forum if you happen to have such a
%     laptop.
%
% [2] You can get Linux 4.8.11 for Ubuntu 16.04-LTS (and later) for manual installation from here:
%     http://kernel.ubuntu.com/~kernel-ppa/mainline/v4.8.11/
%
%     On a 64-Bit system you'd download and install the files in the amd64 section, i.e.
%     the files (download links) named:
%
%     linux-headers-4.8.11-040811_4.8.11-040811.201611260431_all.deb
%     linux-headers-4.8.11-040811-lowlatency_4.8.11-040811.201611260431_amd64.deb
%     linux-image-4.8.11-040811-lowlatency_4.8.11-040811.201611260431_amd64.deb
%
%     You could install them by clicking on them to start the GUI installer. Or you
%     could type "sudo dpkg -i linux-*4.8*" in a terminal window. After a reboot the
%     kernel should become available for use.
%
