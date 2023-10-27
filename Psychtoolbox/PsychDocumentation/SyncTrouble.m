% SyncTrouble -- Causes and solutions for synchronization problems.
%
% You most probably arrived at this help page because Psychtoolbox
% aborted with "SYNCHRONIZATION FAILURE" or a similar error message and
% asked you to read this page.
%
% BACKGROUND: Why proper synchronization to retrace is important.
%
% When executing Screen('OpenWindow'), Psychtoolbox executes different
% calibration routines to find out if back- and frontbuffer swaps (what
% Screen('Flip') does) are properly synchronized to the vertical retrace
% signal (also known as VBL) of your display. At the same time, it measures
% the real monitor video refresh interval - the elapsed time between two
% VBL signals. It is crucial for flicker free, tear free, properly timed
% visual stimulus presentation that buffer swaps only happen during the VBL
% period of the display. The VBL (vertical blank) is the small gap in time
% that occurs when the display has updated its last scanline and before it
% starts redrawing its display surface starting at the first scanline
% again. This small gap is a neccessity for CRT displays and it is
% preserved for compatibility reasons or other technical reasons on flat
% panels and video beamers. After issuing the Screen('Flip') command, the
% graphics hardware finalizes all pending drawing- and image processing
% operations in the backbuffer of an onscreen window to make sure that the
% final stimulus image is ready in the backbuffer for presentation. Then it
% waits for onset of the next VBL interval before flipp'ing the back- and
% frontbuffer of the onscreen window: The previous backbuffer with your
% newly drawn stimulus becomes the frontbuffer, so it will get scanned out
% and displayed to the subject, starting with the next refresh cycle of
% your display device and on all consecutive refresh cycles, until you draw
% a new stimulus to the onscreen window and update the display again via
% Screen('Flip').
%
% On a properly working system, this double buffer swap happens in less
% than a microsecond, synchronized to VBL onset with an accuracy of better
% than a microsecond. All change of visual content therefore only happens
% during the VBL period when the display is not updating, thereby avoiding
% any kind of visual flicker or tearing that would be caused by a mixup of
% an old stimulus and a new (incompletely drawn) stimulus when changing
% image content during the scanout cycle of the display. The exact point in
% time when this buffer swap happened, is returned as timestamp by the
% Screen('Flip') command. It is the most well defined timestamp of visual
% stimulus onset, and it allows to define stimulus onset of future stims
% relative to this accurate baseline, using the 'when' argument of
% Screen('Flip').
%
% Without proper synchronization, you would see very strong visual flicker
% and tearing artifacts in animated (movie / moving) stimuli, you would not
% have any well defined stimulus onset for sequences of static stimuli or
% rapid stimulus presentation, and no means of synchronizing visual
% stimulus presentation to any external stimulation- or acquisition devices
% like fMRI, EEG, sound, ... You also would not have any accurate way of
% getting a stimulus onset timestamp.
%
% However, if you have very special needs, you can disable either Matlabs /
% Octaves synchronization of execution to the vertical retrace, or you can
% disable synchronization of stimulus onset to the vertical retrace
% completely by setting the 'dontsync' flag of Screen('Flip') accordingly.
%
% For more infos about tearing, see Wikipedia articles about "Tearing",
% "Double buffering", "Vertical Synchronization" and the info pages on
% www.psychtoolbox.org
%
% TESTS: How Psychtoolbox tests for proper synchronization to retrace.
%
% After opening an onscreen window, Psychtoolbox executes a measurement
% loop, where it issues Screen('Flip') commands and measures the time
% elapsed between execution of two consecutive flip commands, getting one
% refresh sample per loop iteration. Each sample is checked for validity:
% Duration must be longer than 4 milliseconds and shorter than 40
% milliseconds, because we assume that none of the available display
% devices updates slower than 25 Hz or faster than 250 Hz. Each sample is
% also tested against the expected value provided by the operating system,
% e.g., if the operating system reports a nominal refresh rate of 100 Hz,
% then a sample should have a duration of roughly 1000 ms / 100 Hz == 10
% milliseconds. We accept any sample in a range of +/- 20% around this
% expected value as valid, because timing jitter present in any computer
% system can cause some deviation from the expected value. Samples that
% don't pass this basic test are rejected. Valid samples are used to update
% a mean value, standard deviation of the mean is also calculated: The
% measurement loop ends when at least 50 valid samples have been taken and
% the standard deviation from the mean is less than 200 microseconds. If it
% is not possible to satisfy this criteria during a five second measurement
% interval, then the calibration is aborted and repeated for up to three
% times. Failure to get a valid measurement during up to three calibration
% runs is indicating massive timing problems or the inability of the
% gfx-hardware to properly synchronize buffer swaps to the vertical
% retrace, or the inability to reliably detect when a buffer swap finished.
% This would lead to abortion with the "SYNCHRONIZATION FAILURE" error
% message. Assuming that this calibration loop did provide a valid mean
% measurement of monitor refresh, the value is checked against the value
% reported by the operating system and - on Windows, and some Linux and macOS
% systems - against the result of an independent measurement loop that uses
% direct queries of rasterbeam positions to measure the monitor refresh interval.
%
% Only if all available measurements yield similar results will the test finally
% be rated as PASSED, Psychtoolbox continues execution, and the computed monitor
% refresh interval is used internally for all built-in timing checks and for
% properly timed stimulus presentation.
%
% REASONS FOR FAILING THE SYNC TESTS AND HOW TO FIX THEM:
%
% There are multiple classes of possible causes for sync failure. Work down
% this list of causes and solutions until your problem is resolved, or you hit
% the bottom of the list:
%
% 1. Wrong configuration settings: This usually only affects MS-Windows
% systems, where the display settings control panel for your graphics card
% allows to customize a couple of graphics driver parameters. Some of these
% settings can cause sync failure if they are wrong:
%
% -> Make sure the "Synchronize bufferswaps to the vertical retrace" option
% is set to "Application controlled" or "Application controlled, default to
% on". The wording of the option differs between different graphics cards,
% search for something like that. Examples of other names: "Wait for
% vertical sync", "Wait for vertical refresh" ... If this setting is forced
% to off and *not* application controlled, then the sync tests will fail
% because the hardware doesn't synchronize its image onset (bufferswap) to
% the video refresh cycle of your display.
%
% -> Make sure the "Triple buffering" setting is off, or if you can select
% some "Multibuffering" setting, that it is set to "double buffering" or
% "wait for 1 video refresh" or "swap every refresh". This option may not
% exist, but if it does, any other setting will cause the sync tests to
% possibly succeed, but later stimulus onset timestamping to fail with
% errors. On MS-Windows and macOS, triple-buffering is sometimes indicated
% by a "shaking" or "vibrating" of the welcome screen during the startup
% tests. This serves as visual indicator of this troublemaker. Certain
% types of triple-buffering will not cause the visual indication of shaking
% though, ie. this diagnosis can miss many troublesome triple-buffering
% cases! Note that many modern Intel graphics chips on MS-Windows nowadays
% enforce triple-buffering, and at least we don't know how to disable it -
% no settings in a control panel, no known tricks, no tweakable Windows
% registry keys to prevent it. Therefore many Intel graphics chips are
% currently unfixably broken on Windows, timing-wise.
%
% -> If there is an option "Buffer swap mode" or "Bufferswap strategy", it
% should be set to "Auto select" or "Page flipping" or "Exchange buffers".
% The so called "Copy buffers" or "Blitting" option would result in lower
% performance and wrong/inaccurate timing.
%
% -> On dual/multi display setups, MS-Windows allows you to assign one
% monitor the role of the "primary monitor" or "main monitor" or "primary
% display". It is important that the display device which you use for
% stimulus presentation is this "primary display", otherwise random things
% may go wrong wrt. sync tests and timing.
%
% -> On all operating systems in dual display or multi display mode, it is
% important that you configure both displays for exactly the same color
% depths, resolution and refresh rate if you want to present stimuli across
% multiple displays, e.g., for binocular stereoscopic presentation on a
% dual-display setup. If there is some option you can choose for "genlocked
% modes" or "genlocked modes only", choose or enable that one. Failing to
% configure dual display setups like this will cause massive timing
% problems or tearing artifacts on one of the display if you do dual
% display stimulation. It may also cause failures in timetamping.
%
% -> If you use a hybrid graphics laptop, also known as switchable graphics
% laptop, dual-gpu laptop, or by its marketing name "NVidia Optimus" or
% "AMD Enduro", then read "help HybridGraphics" on how to set up your Linux
% system for proper timing and performance and to learn how to work around
% massive timing problems that are usually unavoidable on MS-Windows.
%
% 2. Temporary timing glitches or system malfunction: It may help to
% restart Matlab/Octave, or to reboot your machine. Sometimes this resolves
% intermittent problems on your system, especially after the system was
% running without reboot for a long time, on high load, or if display
% settings or display configuration has been changed frequently.
%
%
% 3. Driver bugs: Many graphics card device drivers have bugs that cause
% synchronization to fail. This is usually only a problem on Apple macOS or
% Microsoft Windows, rarely on Linux with NVidia proprietary display drivers,
% almost never on Linux with open-source drivers. If none of the above steps
% resolve your problems, check the website of your computer vendor or graphics
% card vendor, or use the "Check for driver updates" function of some operating
% systems to find out if new, more recent graphics drivers have been released
% for your graphics card. If so, update to them. A large number of problems can
% be resolved by a simple driver update.
%
% 4. Driver/Hardware limitations:
%
% All current systems can't provide reliable research grade timing if you don't
% display your stimuli in opaque fullscreen windows, but use windowed mode or
% transparent mode instead. This can lead to sync failures, wrong/unreliable
% timestamping and other performance problems. Only use non-fullscreen or
% transparent windows for development, debugging and leisure, not for running
% your actual data collection.
%
% Some systems have serious problems if more than one graphics card is connected
% and enabled on the computer. They only work well in single-display mode or multi
% display mode from a single graphics card.
%
% Microsoft Windows may provide poor performance on multi display setups if you
% present on multiple displays simultaneously, although your mileage may vary
% widely depending on exact setup.
%
% On MS-Windows, you will suffer drastic timing problems if the stimulus
% presentation window loses the "keyboard focus". The window with the
% "keyboard focus" is the one which is in the foreground (in front of all other
% windows), has its titlebar highlighted instead of shaded (assuming it has a
% titlebar) and receives all keyboard input, i.e., key presses. Therefore we
% assign "keyboard focus" to our onscreen windows automatically. However, if the
% user clicks into windows other than our window with the mouse, or onto the
% desktop background, or uses key combos like ALT+TAB or Windows+TAB to switch
% between windows, then our window will "lose" the keyboard focus and severe
% timing and performance problems may occur. If any window on the screen is
% highlighted, this means it *has stolen* the keyboard focus from our window.
% This weird keyboard focus problem is an unfortunate design decision (or rather
% design flaw) of the MS-Windows graphics subsystem. There isn't anything we or
% the graphics cards vendors could do about it, so you'll have to accept it and
% work around it. Of course this becomes mostly a problem on multi-display setups
% where one display shows the desktop and GUI, and others show Psychtoolbox stimuli,
% so avoid such configurations if you can.
%
% Further examples:
%
% On all systems, graphics adapters are only capable of properly timed
% bufferswaps and proper visual stimulus onset timestamps syncing to
% retrace if the onscreen window is a full-screen window. Synchronization
% fails if the onscreen window only covers part of the screen (i.e., when
% providing a 'rect' argument to Screen('OpenWindow') other than the
% default full-screen rect). Solution is to only use fullscreen windows for
% stimulus presentation. On Windows, Linux and macOS, graphics cards are
% only capable of synchronizing to the retrace of one display. On a single
% display setup, this will simply work. On a dual display setup, e.g.,
% Laptop connected to external video beamer or CRT, the driver/hardware can
% sync to the wrong output device. A simple, although inconvenient, solution
% is to disable the internal flat panel of a Laptop while running your
% study, so the hardware is guaranteed to sync to the external display.
% The use of "mirror mode", or "clone mode", where multiple displays show the
% same content, will almost always cause timing and performance problems.
%
% 5. Graphics system overload: If you ask too much from your poor graphics
% hardware, the system may enter a state where the electronics is not
% capable of performing drawing operations in hardware, either because it
% runs out of video memory resources, or because it is lacking the
% necessary features. In that case, some drivers (e.g., on Microsoft
% Windows or MacOS-X) may activate a software rendering fallback-path: The
% graphics engine is switched off, all rendering is performed by slow
% software in system memory on the cpu and the final image is copied to the
% onscreen framebuffer. While this produces visually correct stimuli,
% presentation timing is completely screwed and not synchronized to the
% monitors refresh at all. On Microsoft Windows, Psychtoolbox will detect
% this case and output some warnings to the Matlab window.
%
% Possible causes of such an overload: Running with anti-aliasing enabled
% at a setting that is too high for the given screen resolution (see 'help
% AntiAliasing'), or running at a display resolution that is too high,
% given the amount of video memory installed on your graphics adapter.
% There may be other cases, although we didn't encounter any of them up to
% now.
%
% Troubleshooting: Try lower display resolutions and multisampling levels,
% or buy a graphics adapter with more onboard memory and performance.
%
% 6. General system overload: If you run too many applications on your
% system in parallel to your Psychtoolbox+Matlab/Octave session, then these
% applications may cause significant timing jitter in your system, so the
% execution of Psychtoolbox - and its measurement loops - becomes
% non-deterministic up to the point of being unusable.
%
% Troubleshooting: Quit and disable all applications and services not
% needed for your study, then retry. The usual suspects are: Virus
% scanners, applications accessing the network or the harddiscs,
% applications like iTunes, system software update...
%
% 7. Bad drivers or hardware in your system that interferes with general
% system timing: This is difficult to diagnose. At least on MS-Windows, you
% can download a free tool "dpclat.exe" from the internet. If you run it, it
% will tell you if there are potential problems with your systems timing and
% give hints on how to resolve them.
%
% 8. Running inside a Virtual Machine: This almost always causes extremely
% bad timing. The command PsychTweak('PrepareForVirtualmachine') may help by
% disabling most timing tests. This is only useful for demos, not for real
% data collection, of course!
%
% 9. Other: Search the FAQ pages on the www.psychtoolbox.org Wiki and the
% Psychtoolbox forum for other problems and solutions.
%
% 10. If everything else fails, post on the forum for help, but read our
% instructions on how to ask questions on the forum properly. You can find
% these instructions on the "Forum" and "Bugs" pages of our Wiki. If we
% find that you didn't read the instructions and you're basically wasting
% our time due to your omissions, we will simply ignore your request for
% help. You can also use "help PsychPaidSupportAndServices" to add for paid
% professional support.
%
%
% HOW TO OVERRIDE THE SYNC TESTS:
%
% That all said, there may be occasions where you do not care about
% perfect sync to retrace or millisecond accurate stimulus presentation
% timing, but you do care about running other applications in parallel, or
% getting your stimulus running quickly, e.g., during development and debugging
% of your experiment or when showing a quick online demo of your stimulus
% during a presentation. In these situations you can add the command
% Screen('Preference','SkipSyncTests', 1); at the top of your script,
% before the first call to Screen('OpenWindow'). This will shorten the
% maximum duration of the sync tests to 3 seconds worst case and it will
% force Psychtoolbox to continue with execution of your script, even if the
% sync tests failed completely. Psychtoolbox will still print error
% messages to the Matlab/Octave command window and it will nag about the
% issue by showing the red flashing warning sign for one second. You can
% disable all visual alerts via Screen('Preference','VisualDebugLevel', 0);
% You can disable all output to the command window via Screen('Preference',
% 'SuppressAllWarnings', 1);
%
% If your graphics system basically works, but your computer has just very
% noisy timing you can adjust the threshold settings we use for our tests
% via the setting:
%
% Screen('Preference','SyncTestSettings' [, maxStddev=0.001 secs][, minSamples=50][, maxDeviation=0.1][, maxDuration=5 secs]);
%
% 'maxStddev' selects the amount of tolerable noisyness, the standard
% deviation of measured timing samples from the computed mean. We default
% to 0.001, ie., 1 msec.
%
% 'minSamples' controls the minimum amount of valid measurements to be
% taken for successful tests: We require at least 50 valid samples by
% default.
%
% 'maxDeviation' sets a tolerance threshold for the maximum percentual
% deviation of the measured video refresh interval duration from the
% duration suggested by the operating system (the nominal value). Our
% default setting of 0.1 allows for +/- 10% of tolerance between
% measurement and expectation before we fail our tests.
%
% 'maxDuration' Controls the maximum duration of a single test run in
% seconds. We default to 5 seconds per run, with 3 repetitions if
% necessary. A well working system will complete the tests in less than 1
% second though.
%
% Empirically we've found that especially Microsoft Windows may need some tweaking
% of these parameters, as some of those setups do have rather noisy timing.
%
%
% MORE WAYS TO TEST:
%
% macOS: The script OSXCompositorIdiocyTest() is a "must run" for macOS
% users, to make sure their system doesn't have the macOS compositor bug.
% If that test fails then visual stimulation timing must be considered not
% trustworthy. For macOS running on ARM based Macs with "Apple silicon" and
% Apples own proprietary AGFX graphics chip, e.g., the Apple M1 SoC's (M1,
% M1 Pro, M1 Max, ...) there is currently no known way to prevent the
% desktop compositor from interfering and therefore visual stimulation
% timing must be considered unfixably broken at the moment!
%
% On MS-Windows, you can try Screen('Preference', 'VisualDebugLevel', 6).
% If the onscreen window is subject to desktop composition, which will certainly
% destroy any kind of proper visual stimulation timing, then the window may
% turn invisible if this VisualDebugLevel value is set. Iow. if your window is
% not showing up at all, despite all the status output in your Octave or Matlab
% command window suggesting normal operation, or if the window is disappearing
% in the middle of your data collection session, then this is a clear indication
% that the DWM desktop compositor is active and interfering. It is instructive to
% try this by running a visual stimulation script and then pressing ALT+TAB, or
% clicking into some other application window: Your Psychtoolbox onscreen window
% will lose foreground status, the DWM will kick in and interfere, and this
% diagnostic will cause your window to turn invisible. Update December 2021:
% For unknown reasons, the "window becomes invisible under DWM composition"
% no longer works with all driver + gpu configurations for recent Windows
% 10 / 11 versions for unknown reasons. So an invisible window means DWM
% composition, but a visible window unfortunately does no longer 100%
% guarantee the absence of DWM composition.
%
% Linux: Psychtoolbox has various mechanisms to detect timing problems and will
% almost always provide difficult to ignore warning messages in the Octave or
% Matlab command window. It has the most advanced diagnostic mechanisms to
% protect you against silent failures.
%
% The script VBLSyncTest() allows you to assess the timing of Psychtoolbox
% on your specific setup in a variety of conditions. It provides various tweakable
% parameters, and displays a couple of plots at the end, so there is no way
% around reading the 'help VBLSyncTest' if you want to use it.
%
% The script PerceptualVBLSyncTest() shows some flickering stimulus on the
% screen and allows you to assess visually, if synchronization works
% properly. On Linux there are also PerceptualVBLSyncTestFlipInfo() and
% PerceptualVBLSyncTestFlipInfo2() for further approaches to testing and
% timestamping.
%
% FlipTimingWithRTBoxPhotoDiodeTest() allows very extensive testing, by use of
% external measurement equipment. VRRTest() allows testing of variable refresh
% display setups - mostly only applicable on Linux.
%
% MORE READING: See 'help BeampositionQueries' for more info about timing issues.
% See 'help HybridGraphics' for problems and caveats related to multi-gpu machines.
%
%
% LINUX specific tips:
%
% 1. Just as on all other operating systems, timed visual stimulus onset
% and stimulus onset timestamping is not reliable for regular (non-
% fullscreen) windows, ie. windows which don't cover the complete desktop of
% a Psychtoolbox screen (also known as X-Screen), or for transparent windows,
% e.g., when the PsychDebugWindowConfiguration() command was used. Use of
% rotated display output (90, 180, 270 degrees etc.) or mirror/clone mode
% also prevents proper timing, just as on the other systems. Psychtoolbox
% PsychImaging command and Panelfitter (cfe. PanelFitterDemo) provide means
% to rotate or scale display output with correctly working timing.
%
% Your windows must be non-transparent, decoration/borderless, fullscreen
% and cover a complete X-Window system X-Screen. On a multi-display setup
% that means that either your window must cover all connected displays, or
% you need to setup separate X-Screens in the graphics driver control panel
% GUI, or via a /etc/X11/xorg.conf file for different displays, so that all
% stimulus displays are grouped in one (or multiple) X-Screen which are
% fully covered by your PTB onscreen window, and all other displays, e.g.,
% operator GUI displays, are grouped into a different X-Screen. The most
% easy way to set up such a configuration is to use the XOrgConfCreator()
% script, followed by use of the XOrgConfSelector() script.
%
% One source of spurious warnings about "page flipping" not being used, can
% be if your desktop GUI displays some onscreen notification messages, e.g.,
% little popup boxes with messages like "You have new mail!", "New software
% updates available", "Meeting reminder at XXX clock", "Network connection lost",
% ... etc. Obviously if this happens during an experiment session, it will cause
% the stimulation onscreen window to be be partially occluded for some time
% by the popup, and that would temporarily impair stimulation timing and print
% one such warning message for each Screen('Flip'). Try to disable the notification
% or the source of the notification popup during experiment sessions to avoid this.
%
% Another cause for spurious warnings like these, usually at the startup/beginning
% of your experiment session is low system memory. This prevents the graphics drivers
% from using the optimized page-flipping mode, which is needed for precise timing,
% but requires more memory. The system will try to free up memory and then switch
% to page-flipping, so the warnings go eventually away after a couple of seconds of
% runtime. An indicator that this happened is if the problem goes away on successive
% runs of your experiment, and/or after you've closed unneeded applications, so enough
% free memory is available from the start of your script. This type of low RAM problem
% is especially likely on machines with not much RAM and Intel integrated graphics chips,
% as those depend on enough free system RAM.
%
% 2. A major source of timing trouble can be 3D desktop compositors. Either
% use a user interface that doesn't employ a desktop compositor, e.g., the
% GNOME-2 classic 2D desktop, LXDE or XFCE desktop at default settings, or
% configure your desktop compositor of choice to "unredirect_fullscreen_windows",
% which will disable the compositor for Psychtoolbox fullscreen windows.
%
% How to configure your desktop compositor to do this? On modern Linux
% distributions, usually no manual steps are required for typical use:
%
% - Ubuntu's GUI doesn't require any setup for "single display single x-screen"
%   setups, or for multi display setups if the visual stimulation display(s) are
%   attached to one or multiple secondary X-Screens (screen 1, 2, ...).
%
% - On other very old and ancient GUI's which use the Compiz compositor, the command ...
%   PsychGPUControl('FullScreenWindowDisablesCompositor', 1);
%   ... can do this setup step for you. It is executed automatically during
%   installation of Psychtoolbox, so you usually don't need this command.
%   Multi display stimulation for displays attached to X-Screen 0 may not
%   work properly though, at least as tested long ago on Ubuntu 16.04.0 LTS.
%   In the unlikely case you would want to use such a setup on Compiz, ask on the
%   Psychtoolbox forum for setup help for such legacy desktop environments.
%
% - On GNOME-3, no special setup is required.
%
% - On KDE with a single display setup, usually no special setup is required.
%
% - KDE multi-display setups usually requires some manual configuration:
%   Do this on (K)Ubuntu 14.04 LTS: Open "KDE System Settings" ->
%   "Display and Monitor" -> "Compositor" -> "Allow applications to block compositing"
%   -> Check the checkbox -> "Apply" -> Done.
%
%   If on KDE you still get warnings or errors by PTB related to display timing,
%   or you want maximum graphics performance, you can also try to completely
%   disable desktop composition, either by pressing SHIFT + ALT + F12
%   before the beginning and after the end of your experiment session to
%   completely disable the compositor during the runtime of your experiment
%   script. Or you disable composition completely: In the "KDE System Settings"
%   -> "Compositor" section, uncheck the "Enable compositor on startup" checkbox,
%   so KDE will start up with its non-composited GUI. This GUI is still very nice
%   looking and ergonomic but frees up additional resources for PTB's graphics
%   and timing requirements.
%
% If you use the NVidia proprietary graphics drivers, frequent synchronization
% failures, or other sync related warnings, or unsteady, irregularly timed flicker
% during PerceptualVBLSyncTest indicate that a desktop compositor is in use. On
% Linux with the free and open-source graphics drivers radeon, nouveau or intel,
% PTB will output warnings about non-pageflipped flips in such a case. It will
% also output similar warnings about triple-buffering or one-buffering if the
% proprietary NVidia driver is recent enough to support detecting this.
%
% 3. Another reason for timestamping problems can be the use of
% triple-buffering.
%
% Psychtoolbox would warn you, at least when the free graphics drivers are
% in use, about some error in "PsychOSGetSwapCompletionTimestamp" and some
% system configuration problems.
%
% The versions of Psychtoolbox since April 2015 can deal with the
% triple-buffering of both the nouveau graphics driver for NVidia cards,
% and the Intel graphics driver for Intel graphics chips automatically,
% at least for the drivers shipping with Ubuntu 14.04.2 LTS and later,
% so no special setup is required for regular experiment scripts. Precise
% visual stimulus onset timing and timestamping is fully compatible with
% the triple-buffering of those open source graphics drivers.
%
% If you happen to have a unusual setup whose triple-buffering still proves
% troublesome for Psychtoolbox visual presentation timing, read on for how
% to disable triple-buffering in the graphics drivers.
%
% Triple-buffering can be disabled with driver specific options in xorg.conf.
% Use the XOrgConfCreator and XOrgConfSelector scripts to easily set up
% your system for optimal timing or multi-display configurations in case.
% These scripts will guide you through the setup.
%
% For very special needs - if XOrgConfCreator can not do the job for you:
%
% The Psychtoolbox subfolder "PsychHardware/LinuxX11ExampleXorgConfs/"
% contains a collection of xorg.conf sample files which show how to
% configure your graphics driver for optimal timing, for special
% multi-display configurations, and for high color depth displays. In the
% most simple case you can simply copy a suitable file for your graphics
% card into the /etc/X11/xorg.conf.d/ directory of your system, under the name
% xorg.conf, ie., ie rename the file to xorg.conf, then copy it into
% /etc/X11/xorg.conf.d/
%
% Then logout and login again for the changes to take effect.
%
% For more complex setups you may need to combine snippets of code from
% multiple of our sample files into a single xorg.conf file.
%
% On Linux with the open-source intel, nouveau and radeon graphics drivers,
% the script OMLBasicTest() allows some additional correctness checks.
%

% History:
% 17.06.2006 written (MK).
% 09.10.2012 Add Linux-specific section (MK).
% 29.09.2013 Update to current state (MK).
% 05.10.2014 Update to current state (MK).
% 24.12.2014 Update to state where we provide our own ddx'en (MK).
% 12.03.2015 Update to the state where we only provide intel-ddx for XOrg 1.16 (MK).
% 19.04.2015 Update: Intel ddx 2.99.914 and 2.99.917's triple-buffering is
%            now compatible with PTB, no need for Option TripleBuffer off anymore. (MK).
% 09.10.2021 Revamp, making it closer to Ubuntu 20.04-LTS.
% 28.01.2022 Reformat some. Explain triple-buffer jitter indicator, Intel
%            triple-buffer brokeness on Windows, Apple ARM SoC brokeness
%            and unreliable connection DWM - invisible windows on current
%            Windows 10/11. (MK)
