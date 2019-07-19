% LinuxGameMode - How to setup Linux performance optimizations via "gamemode"
%
% You may have arrived here because Priority() instructed you to go here if
% you want to enable a couple of extra performance optimizations for your
% Linux system, to increase graphics and compute performance and further improve
% realtime behaviour for demanding experimental paradigms.
%
% The "gamemode" package is an optional Linux package, developed by the gaming
% company "Feral Interactive". It is meant to automatically optimize a Linux
% system for high performance in demanding video games. Luckily for us, the same
% optimizations that help high-end video games can also help the performance of
% demanding psychophysics paradigms, especially for low-latency sound and hardware
% input/output and high-performance graphics and visual stimulation.
%
% When your script executes the Priority(n) command with a priority level n > 0,
% ie. realtime priority, Psychtoolbox will check if the gamemode package is
% installed and properly set up on your Linux system. If so, it will ask the
% gamemode package to switch your Linux system into "Game mode", until Priority(0)
% is executed, clear Screen, clear mex, or clear all is executed, or Octave/Matlab
% is quit.
%
% Switching to gamemode should disable screen savers and screen lockers and various
% power saving features which can impair performance. It will switch the cpu into
% high-performance, high power consumption mode (by switching the cpu governor
% to "performance" mode) for minimal processing latency and maximum speed. It will
% give Psychtoolbox extra priority for any input/output, e.g., disc access or other
% hardware access. Optionally, if set in a configuration file, it will enable
% additional realtime scheduling optimizations on top of Psychtoolbox optimizations,
% and optionally it will tune supported graphics cards for high performance mode,
% e.g., all recent NVidia gpu's with the proprietary driver, and all modern AMD
% gpu's with the amdgpu or amdgpu-pro open-source or hybrid drivers.
%
% The "gamemode" package is not installed by default, but you can get recent
% versions for some Ubuntu Linux distributions from the following sources:
%
% For Ubuntu 18.04 LTS, 18.10 and 19.04 you can get up to date packages from the
% following privately contributed, inofficial and *untrusted* PPA:
%
% https://launchpad.net/~samoilov-lex/+archive/ubuntu/gamemode
%
% Please note that the above ppa is not reviewed by Ubuntu for correctness
% and could in theory contain malicious code that would execute with admin
% permissions on your system. So you have to trust the producer of that ppa
% if you decide to install it.
%
% Alternatively, you can also download the latest version of the gamemode package
% source code, and compile and install it from its upstream source yourself:
%
% https://github.com/FeralInteractive/gamemode
%
% There you will also find documentation for extended setup of the package, e.g.,
% an example .ini configuration file for additional performance tuning of gpu's:
%
% https://github.com/FeralInteractive/gamemode/blob/master/example/gamemode.ini
%
% If you run PsychLinuxConfiguration it will offer you to install a gamemode.ini
% file into your system which is already optimally set up for high-performance
% use with Psychtoolbox.
%
% Note that users of a multi-gpu system, e.g., a hybrid-graphics laptop, may need
% to edit the /etc/gamemode.ini file manually after installation, as the config
% file encodes the card number of the graphics card to optimize. On a dual-gpu
% system, the default card number 0 may be wrong and would need to be changed
% manually to 1 by you.
%
