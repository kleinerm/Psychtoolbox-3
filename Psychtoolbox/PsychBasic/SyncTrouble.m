% SyncTrouble -- Causes and solutions for synchronization problems.
%
% You most probably arrived at this help page because Psychtoolbox
% aborted with "SYNCHRONIZATION FAILURE" and asked you to read this
% page.
%
% BACKGROUND: Why proper synchronization to retrace is important.
%
% When executing Screen('OpenWindow'), Psychtoolbox executes different
% calibration routines to find out if back- and frontbuffer swaps
% (what Screen('Flip') does) are properly synchronized to the vertical
% retrace signal (also known as VBL) of your display. At the same time,
% it measures the real monitor video refresh interval - the elapsed time
% between two VBL signals. It is crucial for flicker free, tear free,
% properly timed visual stimulus presentation that buffer swaps only
% happen during the VBL period of the display. The VBL (vertical blank) is
% the small gap in time that occurs when the display has updated its last scanline
% and before it starts redrawing its display surface starting at the first
% scanline again. This small gap is a neccessity for CRT displays and it
% is preserved for compatibility reasons or other technical reasons on flat
% panels and video beamers. After issuing the Screen('Flip') command, the
% graphics hardware finalizes all pending drawing- and imageprocessing
% operations in the backbuffer of an onscreen window to make sure that the
% final stimulus image is ready in the backbuffer for presentation. Then
% it waits for onset of the next VBL interval before flipp'ing the back-
% and frontbuffer of the onscreen window: The previous backbuffer with your
% newly drawn stimulus becomes the frontbuffer, so it will get scanned out
% and displayed to the subject, starting with the next refresh cycle of your
% display device and on all consecutive refresh cycles, until you draw a new
% stimulus to the onscreen window and update the display again via Screen('Flip').
%
% On a properly working system, this double buffer swap happens in less than
% a microsecond, synchronized to VBL onset with an accuracy of better than a
% microsecond. All change of visual content therefore only happens during
% the VBL period when the display is not updating, thereby avoiding any kind of
% visual flicker or tearing that would be caused by a mixup of an old stimulus and
% a new (incompletely drawn) stimulus when changing image content during the
% scanout cycle of the display. The exact point in time when this
% buffer swap happened, is returned as timestamp by the Screen('Flip') command.
% It is the most well defined timestamp of visual stimulus onset, and it allows to
% define stimulus onset of future stims relative to this accurate baseline,
% using the 'when' argument of Screen('Flip').
%
% Without proper synchronization, you would see very strong visual flicker and
% tearing artifacts in animated (movie / moving) stimuli, you would not have any
% well defined stimulus onset for sequences of static stimuli or rapid stimulus
% presentation, and no means of synchronizing visual stimulus presentation to any
% external stimulation- or acquisition devices like fMRI, EEG, sound, ... You also
% would not have any accurate way of getting a stimulus onset timestamp.
%
% However, if you have very special needs, you can disable either Matlabs / Octaves
% synchronization of execution to the vertical retrace or you can disable synchronization
% of stimulus onset to the vertical retrace completely by setting the 'dontsync' flag
% of Screen('Flip') accordingly.
%
% For more infos about tearing, see Wikipedia articles about "Tearing", "Double buffering",
% "Vertical Synchronization" and the info pages on www.psychtoolbox.org
%
% TESTS: How Psychtoolbox tests for proper synchronization to retrace.
%
% After opening an onscreen window, Psychtoolbox executes a measurement loop,
% where it issues Screen('Flip') commands and measures the time elapsed between
% execution of two consecutive flip commands, getting one refresh sample per
% loop iteration. Each sample is checked for validity: Duration must be longer than
% 4 milliseconds and shorter than 40 milliseconds, because we assume that none of
% the available display devices updates slower than 25 Hz or faster than 250 Hz. Each
% sample is also tested against the expected value provided by the operating system, e.g.,
% if the operating system reports a nominal refresh rate of 100 Hz, then a sample should have
% a duration of roughly 1000 ms / 100 Hz == 10 milliseconds. We accept any sample in a
% range of +/- 20% around this expected value as valid, because timing jitter present in
% any computer system can cause some deviation from the expected value. Samples that don't
% pass this basic test are rejected. Valid samples are used to update a mean value, standard
% deviation of the mean is also calculated: The measurement loop ends when at least 50 valid
% samples have been taken and the standard deviation from the mean is less than 100 microseconds.
% If it is not possible to satisfy this criteria during a five second measurement interval, then the
% calibration is aborted and repeated for up to three times. Failure to get a valid measurement
% during up to three calibration runs is indicating massive timing problems or the inability
% of the gfx-hardware to properly synchronize buffer swaps to the vertical retrace. This leads
% to abortion with the "SYNCHRONIZATION FAILURE" error message.
% Assuming that this calibration loop did provide a valid mean measurement of monitor refresh,
% the value is checked against the value reported by the operating system and - on MacOS-X - against
% the result of an independent measurement loop that uses direct queries of rasterbeam positions
% to measure the monitor refresh interval. Only if all available measurements yield similar results,
% the test is finally rated as PASSED, Psychtoolbox continues execution and the computed monitor
% refresh interval is used internally for all built-in timing checks and for properly timed
% stimulus presentation.
%
% REASONS FOR FAILING THE SYNC TESTS AND HOW TO FIX THEM:
%
% There are multiple classes of possible causes for sync failure:
%
% 1. Driver bugs: The graphics card device drivers can have bugs that cause synchronization
% to fail: The MacOS-X drivers for ATI graphics adapters had such a bug up to and including
% MacOS-X 10.4.2 and earlier. While synchronization worked properly on single display setups,
% when connected to a dual display setup, the driver/hardware did not synchronize bufferswaps
% to the vertical retrace of the display device on which the onscreen window was presented, but
% always to the device connected to the first display connector. This could be diagnosed by
% running both displays at different refresh rates, so Psychtoolbox could find out, if it was
% syncing to the proper display by comparing the measurements taken with the nominal framerate
% reported by the operating system. It could be solved by physically switching the display
% connectors on the computer. This bug is resolved in the latest releases of MacOS-X, i.e.,
% 10.4.5 and later.
%
% 2. Driver/Hardware limitations: On Microsoft Windows, some ATI graphics adapters are only
% capable of syncing to retrace, if the onscreen window is a full-screen window. Synchronization
% fails if the onscreen window only covers part of the screen (i.e., when providing a 'rect'
% argument to Screen('OpenWindow') other than the default full-screen rect). Solution is to
% only use fullscreen windows for stimulus presentation. On Windows, Linux and MacOS-X, some
% graphics cards (e.g., many - if not all - mobile graphics cards built into Laptops) are only
% capable of synchronizing to the retrace of one display. On a single display setup, this will
% simply work. On a dual display setup, e.g., Laptop connected to external video beamer or CRT,
% the driver/hardware can sync to the wrong output device. A simple, although
% inconvenient solution is to disable the internal flat panel of a Laptop while running your
% study, so the hardware is guaranteed to sync to the external display. Depending on the hardware
% it may also help to try dual display output with either: Non-mirror mode, running both displays
% at different refresh rates, mirror mode running both displays at different rates, mirror mode
% running both displays at exactly the same resolution, color depth and refresh rate. You'll
% have to try, as it has been found to be highly dependent on hardware, driver and operating system,
% which combinations work and which don't.
%
% 3. Graphics system overload: If you ask too much from your poor graphics hardware, the system
% may enter a state where the electronics is not capable of performing drawing operations in
% hardware, either because it runs out of video memory ressources, or because it is lacking the
% neccessary features. In that case, some drivers (e.g., on Microsoft Windows or MacOS-X) may
% activate a software rendering fallback-path: The graphics engine is switched off, all rendering
% is performed by slow software in system memory on the cpu and the final image is copied to
% the onscreen framebuffer. While this produces visually correct stimuli, presentation timing
% is completely screwed and not synchronized to the monitors refresh at all. On Microsoft Windows,
% Psychtoolbox will detect this case and output some warnings to the Matlab window.
%
% Possible causes of such an overload: Running with Anti-Aliasing enabled at a setting that is
% too high for the given screen resolution (see 'help AntiAliasing'), or running at a display
% resolution that is too high, given the amount of video memory installed on your graphics
% adapter. There may be other cases, although we didn't encounter any of them up to now. The same
% could happen if you run a dual display setup that is not switched to mirror-mode (or clone mode),
% so you take up twice the amount of video memory for two separate framebuffers.
%
% Troubleshooting: Try lower display resolutions and multisampling levels, switch dual display
% setups into mirror-mode if possible, or buy a graphics adapter with more onboard memory.
%
% 4. General system overload: If you run too many applications on your system in parallel to
% your Psychtoolbox+Matlab/Octave session, then these applications may cause significant timing
% jitter in your system, so the execution of Psychtoolbox - and its measurement loops - becomes
% non-deterministic up to the point of being unuseable.
%
% Troubleshooting: Quit and disable all applications and services not needed for your study,
% then retry. The usual suspects are: Virus scanners, applications accessing the network or
% the harddiscs, applications like iTunes, system software update...
%
% 5. Triple buffering: Some graphics drivers - especially on Microsoft Windows - allow you to
% enable a mechanism that is called "triple buffering". This is off by default and can be enabled
% and disabled via the display settings panel. If enabled, it will completely screw any kind of
% tests and prevent any reasonable timing. This special mode is only useful for a few computer
% games. Troubleshooting: Make sure this is disabled.
%
% HOW TO OVERRIDE THE SYNC TESTS:
%
% That all said, there may be occassions where you do not care about perfect sync to retrace or
% millisecond accurate stimulus presentation timing, but you do care about listening to iTunes
% or getting your stimulus running quickly, e.g., during development and debugging of your
% experiment or when showing a quick & dirty online demo of your stimulus during a presentation.
% In these situations you can add the command Screen('Preference','SkipSyncTests', 1); at the
% top of your script, before the first call to Screen('OpenWindow'). This will shorten the
% maximum duration of the sync tests to 3 seconds worst case and it will force Psychtoolbox
% to continue with execution of your script, even if the sync tests failed completely.
% Psychtoolbox will still print error messages to the Matlab/Octave command window and it will
% nag about the issue by showing the red flashing warning sign for one second.
% You can disable all visual alerts via Screen('Preference','VisualDebugLevel', 0);
% You can disable all output to the command window via Screen('Preference', 'SuppressAllWarnings', 1);
%
% MORE WAYS TO TEST:
%
% The script VBLSyncTest() allows you to assess the timing of Psychtoolbox on your specific setup
% in a variety of conditions. It expects many parameters and displays a couple of plots at the
% end, so there is no way around reading the 'help VBLSyncTest' if you want to use it.
%
% The script PerceptualVBLSyncTest() shows some flickering stimulus on the screen and allows you
% to assess visually, if synchronization works properly.
%
% Both tests are for the really cautious: The built-in test of Screen('OpenWindow') should be
% able to catch about 99% of all conceivable synchronization problems.
%
% MORE READING:
% See the help for 'help MirrorMode' and 'help BeampositionQueries' for more info about display issues.
%

% History:
% 17.06.2006 written (MK).

