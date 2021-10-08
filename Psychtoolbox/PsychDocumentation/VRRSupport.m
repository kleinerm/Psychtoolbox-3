% VRRSupport - How to set up and use variable refresh rate (VRR) displays.
%
% This text describes how you can take advantage of variable refresh rate
% displays (also known as VRR displays, DisplayPort Adaptive Sync displays,
% HDMI VRR displays, FreeSync displays, or G-Sync displays) to improve the
% timing granularity of visual stimulus presentation to smaller time-scales
% than what the video refresh rate of your display allows, e.g., for a 60
% Hz display with a refresh duration of 1/60 second or 16.666 msecs.
%
% Regular fixed rate refresh displays only allow to start presentation of
% a new stimulus image at frame boundaries between video refresh cycles,
% e.g., for a 60 Hz display only at integral multiples of 16.666 msecs. If
% you specify a requested stimulus onset time 'when' in
% Screen('Flip', window, when); then your stimulus image won't show up
% until the start time 't' of the first video refresh cycle after 'when' ie.
% only at time 't' >= 'when' with t being a multiple of 16.666 msecs. This
% means that interstimulus intervals or stimulus presentation durations are
% limited to multiple of 16.666 msecs. Dynamic animations correspondingly
% can only run at framerates (fps) which are integral divisors of the video
% refresh rate, e.g., for a 60 Hz display at 60 fps, 30 fps, 20 fps, 15 fps,
% 12.5 fps, 10 fps, ...
%
% For some high level overview:
%
% https://en.wikipedia.org/wiki/Variable_refresh_rate
% https://en.wikipedia.org/wiki/FreeSync
% https://en.wikipedia.org/wiki/Nvidia_G-Sync
%
% If you have an operating system, a graphics card, video cables and a display
% device which are VRR capable, you can enable a Psychtoolbox function to enable
% a continuous setting of inter-stimulus-interval (ISI) and the system will try
% its best to conform to your specification. There is still a minimum stimulus
% presentation duration / ISI which is the duration of a video refresh cycle of
% your display, ie. for a 60 Hz display, that would be 16.666 msecs. However,
% above 16.666 msecs you can specify ISI or stimulus duration with millisecond
% precision and the system will try to obey, ie. closely match the onset of a
% requested 'when' time.
%
% This allows for variable ISI's / stimulus durations over a larger range
% of values, and it allows for changing any framerate fps for animations
% that is <= 60 Hz and not locked to the 30, 20, 15, 12.5 ... fps sequence,
% e.g., choose 38.4 fps, 42.2 fps, etc.
%
% So far the theory. In practice things are not always that beautiful.
%
% But first things first:
%
% On a properly setup hardware + software system, you can request this new
% fine-grained timing mode via a new PsychImaging task, before opening a
% opaque, non-transparent, unoccluded, toplevel, decorationless, fullscreen
% onscreen window:
%
% PsychImaging('PrepareConfiguration');
% PsychImaging('AddTask', 'General', 'UseFineGrainedTiming');
% win = PsychImaging('OpenWindow', screenNumber, ...);
%
% If the hardware and software supports fine-grained timing mode (referred
% to from now on as VRR), the opened fullscreen window will be presented
% with VRR mode and vbl = Screen('Flip', win, when); will try to obey 'when' as
% closely as possible, as long as 'when' for flip is more than one video
% refresh duration away from the time of previous Flip, ie. when >= vbl +
% videorefreshduration. If you request a 'when' smaller than 1 video refresh
% duration away, the system will behave as in the past, it will flip after one
% video refresh duration. How well this mechanism works is highly dependent on
% your operating system, graphics card, display driver and display device.
%
%
% Operating system requirements:
%
% Currently only Linux is supported. This has been tested with Ubuntu
% 20.04-LTS and later versions of Ubuntu Linux.
%
% Hardware requirements:
%
% Ideally a modern "FreeSync capable" AMD graphics card (GPU). GPU's of the
% Sea Islands gpu family (Graphics Core Next 2nd Generation, cfe.
% https://en.wikipedia.org/wiki/Graphics_Core_Next#second) should work after
% some manual extra setup, GPU's of the Volcanic Islands or Polaris series
% (Graphics Core Next 3rd Generation, cfe.
% https://en.wikipedia.org/wiki/Graphics_Core_Next#third) should work plug
% and play. GPU's of the AMD Vega series or later (Graphics Core Next 5th
% Generation, cfe. https://en.wikipedia.org/wiki/Graphics_Core_Next#fifth)
% should provide improved stability and precision for VRR mode in many cases
% and are therefore recommended if you intend to make heavy use of this new
% feature. Best results have been obtained during actual testing with AMD
% RavenRidge integrated gpu's - the stuff found in modern AMD Ryzen
% processors. Those Ryzen integrated graphics or AMD Navi discrete gpu's
% are recommended strongly.
%
% Such "FreeSync" gpu's from AMD need to be currently combined with a
% "FreeSync" or "FreeSync-2" or "FreeSync-2 HDR" certified display that is
% connected via DisplayPort cable. FreeSync-2 certified displays are preferable
% for better quality and displays which are both "AMD FreeSync-2" certified, and
% simultaneously certified as "NVidia G-Sync compatible", should provide best
% quality. A display with a large refresh rate range (VRR range) of 30 Hz - 120 Hz
% (or higher) is recommended. One problem of VRR displays is that they may flicker
% if ISI's are changed too rapidly over a too large range. Higher quality displays
% (probably correlated to purchase prize) are supposed to flicker less. Both AMD
% and NVidia have testing labs to test and certify displays if they meet certain
% criteria (like little flicker), with FreeSync2 / FreeSync2 HDR having to conform
% to stricter criteria than FreeSync, and a display that is both FreeSync2
% certified and G-Sync compatible will have passed the strictest tests at both
% AMD and NVidia.
%
% Some interesting technical talk from AMD at XDC 2019 about VRR / FreeSync
% under Linux:
%
% https://www.youtube.com/watch?v=ajBf_b4Aw98
%
% Blurbusters has a list of FreeSync monitors here:
% https://www.blurbusters.com/freesync/list-of-freesync-monitors
%
% Another option that may work, but is *not* expected to work as well as AMD,
% would be a NVidia G-Sync capable graphics card, ie. the Kepler GPU family or
% later cards (GeForce GTX 650 Ti Boost GPU or higher) in combination with a
% NVidia G-Sync capable monitor, or a Pascal GPU family card (GeForce GTX 1000
% series) or later with "G-Sync compatible" monitor, e.g., certain models of
% FreeSync2 monitors.
%
% Blurbusters has a list of G-Sync monitors here:
% https://www.blurbusters.com/gsync/list-of-gsync-monitors
%
% Please note that currently simultaneously "FreeSync and G-Sync compatible"
% monitors should work well with both AMD and NVidia GPU's, but G-Sync monitors
% will *only* work with NVidia GPU's. Therefore a "FreeSync2 and G-Sync compatible"
% monitor would be a much better long-term choice, which doesn't restrict the
% freedom of choice of graphics card.
%
% Intel integrated graphics chips of type Tigerlake or later ("Intel Xe
% graphics" or later), or future Intel discrete graphics cards, should also
% support VRR over Displayport if you use Linux 5.12 or later. However,
% this has not been tested in practice yet, due to lack of suitable
% hardware.
%
% As operating system, Ubuntu Linux 20.04.3 LTS or later, or compatible
% flavors, are currently recommended.
%
% Setup:
% ------
%
% Run our XOrgConfCreator script, answer the question for "advanced
% features" with (y)es, answer the question about VRR support with (y)es,
% and the other questions to your liking, finish the script to save the new
% config file, then run XOrgConfSelector to select that file as new
% configuration for the following work sessions accordingly, followed by a
% logout and login (or a reboot if you want) to activate the new
% configuration.
%
% For NVidia GPU's (Not recommended! May or may not work ok) you must
% install a recent enough NVidia proprietary graphics and display driver,
% use a X-Screen with exactly one G-Sync monitor, and use the
% nvidia-settings GUI app to enable "G-Sync" mode whenever you need it, and
% disable "G-Sync" mode whenever you want to use conventional fixed refresh
% rate presentation again. Psychtoolbox was lightly tested with a NVidia
% GeForce GTX 1070 + "435 series" drivers in December 2019, the situation
% may have changed since then.
%
% Once you have set up your hardware and OS accordingly, you can add the
% above mentioned 'UseFineGrainedTiming' PsychImaging command to script
% which want to utilize VRR for fine grained timing or controllable
% framerate. With AMD or Intel hardware, Psychtoolbox Screen() command will
% help you to diagnose and fix any problems that might prevent VRR from
% working - pointing out missing but required setup steps. With NVidia
% hardware, Screen() will just mostly hope that it works and is properly
% setup and enabled, simply failing or malfunctioning in "interesting"
% ways, if something is not set up correctly.
%
% An example script for testing and exercising VRR mode can be found as
% VRRTest.m
%
% Starting 4th December 2019, Psychtoolbox 3.0.16 shipped very basic support for
% VRR. It works reasonably well, as long as you choose animation framerates that
% are within the supported VRR range of your display, e.g., 30 - 144 Hz, or
% 48 - 144 Hz or such, or inter-stimulus-intervals that are within a range
% corresponding to those framerates, e.g., for a 30 - 144 Hz display, from 6.9444
% msecs to 33.333 msecs. For framerates lower than the displays minimum, e.g.,
% below 30 fps, or corresponding ISI's above 33.333 msecs on a 30 Hz display,
% timing may become very unstable and unpredictable! Smooth changes in framerate
% or small incremental changes in ISI may provide more stable results than quick
% changes.
%
% As of 18th December 2019, Psychtoolbox 3.0.16 ships improved support for VRR.
% A more sophisticated scheduler allows for more stable and precise results over
% a larger range of framerates and ISI's. See "help PsychImaging" for the new
% scheduling method "OwnScheduled", versus the old method "Simple". Testing
% shows pretty good stability and precision on AMD FreeSync gpu's. NVidia G-Sync
% also showed some improvement, but is less stable than AMD, just as expected.
%
% These are operating system and hardware limitations at the moment. Future
% versions of Psychtoolbox and especially in combination with future
% versions of the Linux operating system are expected to provide vastly
% improved support and stability with modern AMD graphics cards - Ryzen
% integrated processor graphics, or Navi discrete gpu's and later models
% recommended for best results, Vega may also work well but is so far
% untested - and potentially Intel graphics chips, much less so with NVidia
% hardware. But only time will tell... Stay tuned...
%
