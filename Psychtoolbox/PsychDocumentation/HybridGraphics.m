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
% Most modern common PC laptops are muxless though. The iGPU is hard-wired to
% the video outputs, both to the laptop flat panel and the external outputs.
% The iGPU is always active and drives the displays and takes care of drawing
% the GUI and handling 2D applications. The dGPU can be powered up as needed
% to handle GPGPU computations and the rendering for more demanding 2D, Video
% and 3D applications. As the dGPU is not connected to the display outputs, it
% has to copy its rendered images into the RAM of the iGPU and the iGPU then
% displays the images on behalf of the dGPU. This involves some significant
% overhead: Multiple milliseconds of time are needed for each Screen('Flip') to
% copy image data from the dGPU to the iGPU, and converting the data into a format
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
% On Linux, as of August 2017, good progress has been made in implementing methods
% which provide both good performance *and* reliable, trustworthy, accurate visual
% timing and timestamping. Some - but not all! - types of Laptop hardware should
% work well, but for all of them some special configuration or software upgrades
% are needed.
%
% We recommend XServer version 1.19.3 or later, and Mesa version 13 or later, and
% Linux 4.10 or later, as this combination provides best performance and ease of
% setup for all supported types of hybrid graphics laptops. Users of Ubuntu Linux
% can simply install Ubuntu 16.04.6 LTS (or Ubuntu 18.04-LTS and later) from fresh
% installation media, or upgrade to 16.04.6 LTS from earlier Ubuntu releases and then
% install the new hardware enablement stack (HWE) via ...
%
% sudo apt install --install-recommends linux-lowlatency-hwe-16.04 xserver-xorg-hwe-16.04
%
% ... if it isn't already automatically installed after an upgrade to 16.04.4 LTS.
%
% The following sections describe the current level and quality of support for different
% types of hybrid graphics laptops, and required configuration steps, assuming you have
% sufficiently up to date kernel, X-Server and Mesa as explained in the previous paragraph.
% Psychtoolbox would tell you if you need to upgrade your kernel, if you'd run it on a
% muxless hybrid graphics Laptop.
%
%
% * Laptops with an Intel iGPU combined with a NVidia dGPU ("NVidia Optimus" models):
%
%   These should work perfectly if you use the "nouveau" open-source graphics driver,
%   at least as far as testing with three different laptops went. Stimuli are displayed
%   without any artifacts, and timing and timestamping is accurate and trustworthy.
%   Performance is highly dependent on the model of NVidia gpu though, with the latest
%   generations currently providing only relatively low performance, and ongoing work
%   to improve the performance for recent models.
%
%   If you want to use the NVidia proprietary display driver for Linux instead, there
%   now exists a solution which works with correct timing and timestamping, as verified
%   on two Optimus Laptops, a Lenovo Lenovo Ideapad Z50-70 with GeForce 840M and a Razer
%   Blade 2016 with GeForce 1060M. However, the solution is less flexible and power-efficient
%   than use of the "nouveau" open-source driver. It requires some setup work, and it needs
%   a NVidia proprietary driver of at least version 375.66. If you can't select a recent
%   enough driver of at least version 375.66, you need to enable the proprietary graphics
%   driver ppa to get a convenient update to NVidia driver version 375.66 or later versions.
%   Also, you must use a X-Server of the 1.19.x series, the v1.20 servers are not yet supported.
%   Follow the following steps to get Optimus set up:
%
%   1. Install the proprietary graphics drivers ppa by typing in a terminal:
%       sudo add-apt-repository ppa:graphics-drivers/ppa
%       sudo apt-get update
%
%   2. Then launch the 3rd party driver manager GUI to select the NVidia proprietary driver
%      for use on your system. You must select a NVidia proprietary driver of the 375 series,
%      with a version number of 375.66 or later. This will also automatically setup standard
%      Optimus / PRIME support for tear-free stimulus display, but getting proper visual
%      stimulation timing and timestamping for Psychtoolbox requires some more steps.
%
%   3. Edit the file /etc/modprobe.d/nvidia-graphics-drivers.conf. Modify the last line
%      in that file and replace the assignment modeset=0 with modeset=1 to enable drm
%      modesetting support.
%
%   4. Execute "sudo update-initramfs -u -k all" in a terminal.
%      NOTE: You may need to repeat both steps 3 and 4 every time after a significant upgrade
%      of your distributions software. Otherwise Psychtoolbox may complain about timing and
%      synchronization problems after such an upgrade.
%
%   5. Copy the custom Psychtoolbox modesetting driver into the system driver directory.
%      There are two variants, the nolag variant and the highlag variant. In theory, the
%      nolag variant would be preferrable, but it sometimes gives inconsistent performance:
%
%      sudo cp /pathto/Psychtoolbox/PsychHardware/LinuxDrivers/NVidiaOptimus/modesetting_drv.so /usr/lib/xorg/extra-modules/modesetting_drv.so
%
%      For use of the variant with higher lag but consistent performance, use the highlag driver
%      instead:
%
%      sudo cp /pathto/Psychtoolbox/PsychHardware/LinuxDrivers/NVidiaOptimus/modesetting_drv.so_highlag /usr/lib/xorg/extra-modules/modesetting_drv.so
%
%   6. Reboot. Now your system should be ready for research compatible Optimus.
%
%   On Ubuntu, the "nvidia-settings" GUI tool allows you to switch between Optimus (PRIME) and
%   standard Intel graphics. The section "PRIME profiles" allows to click on a toggle button
%   to switch between "NVidia" gpu for power hungry but fast Optimus, and "Intel" for low power
%   consumption lower performance mode.
%
%   If you want to use a different distribution than Ubuntu, "Fedora 25" and later, "Debian unstable",
%   "Arch Linux" and "SuSE Tumbleweed" are known to ship required X-Server, Linux kernel and NVidia
%   driver options for Optimus. However, these are not tested with PTB, and setup may be different
%   from Ubuntu's approach. Alternatively you could also download and compile your own X-Server 1.19
%   if you are not afraid of compilers and Makefiles and willing to spend a workday doing this. The
%   following paragraph assumes you are not using Ubuntu 17.04:
%
%   Once you have a X-Server 1.19 up and running, you will need the NVidia proprietary
%   display drivers of version 375.66 or later for 64-Bit Intel processors. Then you need
%   to copy various configuration files into various places, and adapt some of these files
%   to your specific system. Finally you need to install a custom xf86-video-modesetting
%   display driver onto your system. This modesetting driver is specifically made to
%   interoperate with Psychtoolbox to provide research grade precision timing and
%   timestamping. Then, after a reboot, you may be rewarded with a NVidia Optimus laptop
%   which can efficiently use your discrete high-performance NVidia gpu with research grade
%   timing. However, research grade timing is only provided for pure single-display setups,
%   not for any kind of multi-display operation. That means your Laptop can have exactly
%   one display enabled, either the Laptop internal flat-panel, or one externally connected
%   display. Also timing is only reliable and trustworthy for a Psychtoolbox fullscreen
%   window. You will find all the needed config files and custom made display driver and
%   setup instructions in the Psychtoolbox subdirectory Psychtoolbox/PsychHardware/LinuxDrivers/NVidiaOptimus/
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
%   These should work very well out of the box on Ubuntu 16.04.3 LTS and later, as explained
%   above.
%
%   On other Linux distributions make sure to install Linux 4.8.11 or later versions of the Linux
%   kernel, together with X-Server 1.18 or later, and Mesa version 17.0 or later.
%
%   AMD Enduro hybrid graphics was tested with two PC setups:
%
%   - Intel HD "Haswell desktop" graphics chip + AMD Radeon R9 380 Tonga Pro.
%   - Intel HD "Ivybridge desktop" graphics chip + AMD FireGL "Cedar".
%
%   Stimuli are displayed without any artifacts and timing and timestamping is accurate and
%   trustworthy.
%
%
% * Laptops with dual NVidia gpus NVidia iGPU + NVidia dGPU:
%
%   Muxless would not work with any current official solution [1]. However, i am not aware of
%   any recent muxless laptops - or any such muxless laptops actually - which use dual-NVidia
%   gpus. All known dual-NVidia laptops are rather old and use a hardware mux, so Linux
%   "vgaswitcheroo" mechanism can be used to switch between gpus for perfect results.
%
%
% * Laptops with dual AMD gpus AMD iGPU + AMD dGPU ("AMD Enduro" models):
%
%   Muxless won't work with any current official solution, so you can only use the
%   AMD iGPU [1].
%
%
% For those combinations that should work (Intel iGPU + NVidia/AMD dGPU "Optimus/Enduro"),
% after you've upgraded to all the required software, the following setup steps are
% needed for muxless PRIME mode. Note that these *do not apply* to Optimus with the proprietary
% graphics driver from NVidia:
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
% 3. Optionally verify the handshaking and synchronization actually works. Psychtoolbox must not
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
% [1] There also exist some muxless Laptop models where the iGPU is hard-wired to the internal
%     Laptop flat panel, whereas the dGPU is hard-wired to (some of) the external video outputs.
%     On these models one can configure a dual-x-screen setup for visual stimulation and then
%     assign the iGPU to drive X-Screen 0 on the internal panel and assign the dGPU to drive
%     X-Screen 1 on the external video outputs.
%
%     This would work with high performance and timing precision even on hybrid graphics laptops
%     which otherwise wouldn't work, e.g., dual NVidia or dual AMD laptops. Such a setup
%     wouldn't require any of the setup steps mentioned above. Instead it would require to
%     create a dual-x-screen setup via XOrgConfCreator, but then to manually customize the
%     created config file, as XOrgConfCreator can't automatically handle such dual-gpu setups
%     yet. Ask for assistance on the Psychtoolbox user forum if you happen to have such a
%     laptop. One example xorg.conf file for handling such a setup (Intel iGPU + AMD dGPU)
%     can be found under the name xorg.conf_SeparateScreensDualGPUIntelAndAMD in the
%     Psychtoolbox/PsychHardware/LinuxX11ExampleXorgConfs/ folder. It would need customization
%     though for a given Laptop, specifically adapting the "BusID" parameter for your hardware.
%
%     Another example X-Config file for such a laptop can be found for year 2016 Razer Blade gaming
%     laptop with Intel HD-530 Skylake iGPU + NVidia GeForce 1060M Pascal dGPU, where the Intel
%     iGPU is hardwired to the laptop panel and USB-C output, whereas the NVidia dGPU is hardwired
%     to a HDMI output. The filename in the Psychtoolbox/PsychHardware/LinuxX11ExampleXorgConfs/
%     folder is xorg.conf_RazerBlade-2-XScreens-intel+nouveau
%
%     Another X-Config example file for the Razer Blade 2016 is the file ...
%     xorg.conf_RazerBlade-2-XScreens-NVidiaProprietary_iGPUPrime+dGPUHDMInative.conf
%     ... This file is for use with the NVidia proprietary driver instead of the nouveau open-source
%     driver. It uses the NVidia gpu to drive two separate X-Screens 0 and 1. X-Screen 0 is driven
%     via Optimus PRIME output, displaying via the Intel HD 530 iGPU on either the laptop flatpanel
%     or the USB-C video output, but not both at the same time if visual stimulation timing matters.
%     X-Screen 1 is driven directly via the HDMI output connected to the NVidia dGPU.
%
%     NOTE: If you copy these files into the /etc/X11/xorg.conf.d/ folder you must rename them to
%           end with the suffix .conf otherwise they won't be actually used!
