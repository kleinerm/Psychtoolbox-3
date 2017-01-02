% BeampositionQueries -- What they are used for, and what can go wrong.
%
% GNU/Linux, MacOS-X and MS-Windows provide a mechanism that allows to
% query the scanline which is currently updated by the scanning beam of a
% CRT display or by the equivalent mechanism in a video beamer or flat
% panel display, the so called "beamposition".
%
% We use this mechanism for two purposes:
%
% 1. Independent measurement of the monitor refresh interval: Psychtoolbox
% executes a measurement loop during Screen('OpenWindow') where it determines
% the monitor refresh interval. It takes a timestamp whenever the beamposition
% resets to zero at the start of a new display refresh cycle. The difference
% between consecutive timestamps is a sample of refresh duration. The samples
% of 50 consecutive refresh cycles are averaged. The "length" of the VBL
% is also computed as the difference between screen height (e.g., 1024 at a
% display resolution of 1280 x 1024 pixels) and the highest scanline value
% encountered during the 50 refresh cycles.
%
% This measurement is used to cross-check the results of the synchronization tests
% (see 'help SyncTrouble') and the value provided by the operating system to 
% make the sync tests and display calibration as robust as possible, even
% if the operating system reports bogus values like 0 Hz, which can happen
% on MacOS-X and Windows with some flat panels.
%
% 2. Highly accurate and robust stimulus onset timestamps in Screen('Flip').
%
% In normal operation, the Screen('Flip') command, after issuing a buffer-swap
% request to the gfx-hardware, pauses execution until the operating system
% signals swap-completion in sync with vertical retrace. Then it takes a high-
% precision system timestamp. Due to the scheduling jitter present in any
% operating system, sometimes the execution of Psychtoolbox is resumed only after
% some random multi-millisecond delay has passed after buffer-swap, so the
% stimulus onset timestamp can be possibly off by multiple milliseconds from
% the real stimulus onset time. This is unwanted timing noise, especially if
% the time stamp is to be used for either computing stimulus onset time for
% future stimuli, or for synchronizing Psychtoolbox execution with other
% stimulus generators or acquisition hardware.
%
% On Microsoft Windows 2000 and later, Beamposition timestamping is fully
% supported.
%
% On GNU/Linux, timing precision even in non-realtime scheduling mode is
% far superior to all other operating systems, yielding a timing jitter of
% less than 100 microseconds under normal operating conditions. For special
% needs, there exist multiple methods of improving timing down to
% microsecond level, although some of time require some advanced
% programming skills. If Psychtoolbox runs on any recent AMD/ATI, or NVidia
% graphics card, it will utilize beamposition queries for even better
% timing precision. If your Linux system uses an open-source graphics
% driver for NVidia ("nouveau"), AMD/ATI ("radeon") or Intel GPU's, as well
% as on various embedded GPU's (Smartphones and Tablets, embedded systems
% etc.), Linux itself has a built-in high precision timestamping facility
% which is even more robust and precise than Psychtoolbox beamposition
% timestamping.
%
% On MacOSX we use beamposition queries to improve accuracy of our timestamps
% with the help of the PsychtoolboxKernelDriver (see "help PsychtoolboxKernelDriver")
% on cards from AMD and NVidia. On Intel graphics cards, beamposition time-
% stamping doesn't work by default. An alternative mechanism, based on vblank
% timestamps can be enabled by setting Screen('Preference','VBLTimestampingmode', 1);
% This is implemented at acceptable reliability and precision on OS/X 10.6,
% and will be used if the beamposition mechanism malfunctions or is unavailable.
% On 10.7 and later the rather unreliable and sometimes unstable CoreVideo
% timestamping can be enabled, at the risk of applications crashes and wrong
% results on some setups. We always strongly recommend installing the
% PsychtoolboxKernelDriver for best results. Intel graphics cards on MacOSX
% are problematic: Our PsychtoolboxkernelDriver can't handle them without the
% severe danger of causing a hard system crash, so don't use a Intel graphics
% card if you need high precision visual stimulus onset timestamps or timing.
%
% This is how beamposition queries are used:
%
% When taking the system timestamp, we also query the current rasterbeam
% position. From the known height of the display (in scanlines, including
% height of VBL), and the known refresh interval of our display, we can
% translate the current beam position into "elapsed time since start of VBL
% == elapsed time since double buffer swap". By subtracting this elapsed
% time value from our system timestamp, we get a corrected timestamp - the
% real system time of double buffer swap == start of VBL == aka stimulus
% onset. This allows for very accurate timestamps, despite possible
% non-deterministic multi-millisecond timing jitter. Psychtoolbox goes
% through great pains during startup to double-check that all required
% calibration values and mechanisms are accurate and working properly. You
% can assess the accuracy of all returned timestamps by use of the script
% VBLSyncTest. A visual correctness test is provided by
% PerceptualVBLSyncTest. PTB also performs continuous runtime checking to
% detect possible problems caused by defective graphics card drivers.
%
% In case that beamposition queries should not work properly or are not
% supported, PTB will use different fallback strategies:
%
% On Microsoft Windows, only a normal - possibly noisy - timestamp is taken.
%
% On MacOSX by default noisy timestamps are taken.
% On MacOSX, if you set Screen('Preference','VBLTimestampingmode', 1);
% PTB tries to use CoreVideo CVDisplayLink timestamps, and uses their
% values for timestamping the time of buffer- swap. The robustness,
% precision and correctness of CVDisplayLink timestamps is not that great
% on 10.7 and later, therefore this fallback mechanism may be removed in a
% future PTB release. If these queries should fail as well, PTB falls back
% to pure timestamping without any correction.
%
% To get best precision and reliability on OSX 10.7 and later we strongly
% recommend you install the PsychtoolboxKernelDriver and use a NVidia or
% AMD graphics card, not an Intel graphics card.
%
% On Linux, built-in OpenML timestamping has the highest priority,
% robustness and precision and is available on the open-source graphics
% drivers (intel, radeon, nouveau). Should that functionality be missing,
% because you installed the proprietary graphics drivers, beamposition
% timestamping is used on NVidia and AMD/ATI with the proprietary graphics
% drivers, followed by the equivalent of kernel-level vbl timestamping
% should that fail as well for some reason, followed by uncorrected
% timestamping.
%
% The behaviour of PTB can be controlled by the command:
% Screen('Preference', 'VBLTimestampingMode', mode); where mode can be one of the
% following:
%
% -1 = Disable all cleverness, take noisy timestamps. This is the behaviour
%      you'd get from any other psychophysics toolkit, as far as we know.
%
%  0 = Disable CoreVideo/kernel fallback method (OSX and Linux), use
%      either beamposition stamps or noisy stamps if beamposition is
%      unavailable. This is the effective default setting on OSX and Windows.
%
%  1 = Use beamposition. Should it fail, switch to use of kernel-level/CoreVideo
%      timestamps. If that fails as well or is unavailable, use noisy
%      stamps.
%
%  2 = Use beamposition, but cross-check with kernel-level/CoreVideo timestamps.
%      Use noisy stamps if beamposition mode fails. This is for the paranoid
%      to check proper functioning.
%
%  3 = Always use kernel-level/CoreVideo timestamping, fall back to noisy
%      stamps if it fails.
%
%  4 = Use OpenML OML_sync_control extension for high-precision timestamping
%      on supported system configuration, fall back on beamposition queries
%      if the OpenML mechanism is unavailable. OpenML timestamping is
%      currently a Linux only feature on the free open-source graphics
%      drivers. This is the default on Linux and Windows.
%
% The effective default on OS-X and Windows is "0", and "4" on Linux.
%
% If the beamposition query test fails, you will see some warning message
% about "SYNCHRONIZATION TROUBLE" in the Matlab/Octave command window or
% other error messages, as diagnostics is performed at various stages of
% setup and operation.
%
% There are multiple possible causes for failure:
%
% 1. Running digital displays like flat panels or projectors at non-native
% resolution, ie., anything other than their rated maximum resolution, or
% using display rotation by 90/180/270 degrees, e.g., putting the display
% from landscape into portrait orientation. This will violate various
% assumptions our timestamping code makes and introduce interference of the
% graphics driver with visual stimulus onset timing. If you want to use
% your panel at a non-native resolution or orientation, leave it set at its
% native maximum settings and orientation and then use the panelfitter and
% display rotation functions of PsychImaging(). See the demo
% PanelFitterDemo.m and its "help PanelFitterDemo" for further explanation
% on how to use the panelfitter.
%
% 2. System overload: Too many other applications are running in parallel
% to Psychtoolbox, introducing severe timing noise into the calibration and
% test loop. See 'help SyncTrouble' on what to do. This happens rather
% seldomly.
%
% 3. Driver bug: Not much you can do, except submit a bug report to Apple
% or Microsoft for your specific hardware + software setup. This is by far
% the most common cause of failure. Psychtoolbox tries to enable
% work-arounds for some common problems if possible. Usually you should
% update your graphics card driver to see if that resolves the problems.
%
% Note: As of Spring/Summer 2008, many graphics cards + driver combos from
% ATI and NVidia on WindowsXP have bugs which cause beamposition queries to
% fail in a peculiar way. If PTB detects that failure case, it will enable
% some workaround to keep the mechanism going at slightly reduced accuracy:
% Timestamps will still be mostly jitter-free and consistent, so they are
% fully useable for timestamping, timing checks and as a basis for timed
% stimulus presentation and animation. However, all returned timestamps
% will contain a constant bias wrt. the real stimulus onset time of
% somewhere between 20 microseconds and 1.5 milliseconds, depending on your
% display settings, because Psychtoolbox can't determine the total height
% of your display in scanlines (including the invisible VBL interval)
% anymore. Exact height is important for spot-on timestamps. Psychtoolbox
% uses some safe, conservative value for its internal computations, so
% results will be consistent and useable, but contain a small constant
% offset.
%
% In some rare cases, PTB's automatic test fails to detect the bug and
% doesn't enable the workaround by itself. You can manually enable the
% workaround if you want by adding the setting 4096
% (kPsychUseBeampositionQueryWorkaround) to the value x passed via:
% Screen('Preference', 'ConserveVRAM', x);
%
% Just insert this command at the top of your scripts before any other
% Screen() commands. 'x' must be at least 4096 or the sum of 4096 and any
% other values you may want to pass with that command. See "help
% ConserveVRAMSettings" for other workarounds that you can enable manually
% if needed.
% 
% If you want to get rid of that small offset, e.g., because you need to
% synchronize with other modalities or stimulation/recording equipment at
% sub-millisecond precisison, then you can try to figure out the real
% height of the display yourself and tell Psychtoolbox about the true value
% before calling Screen('OpenWindow').
%
% Once you know the real height, e.g., VTOTAL, you'd call this function:
% Screen('Preference', 'VBLEndlineOverride', VTOTAL);
%
% How to find out about VTOTAL? One way is to search the display control
% panel on Windows for some area with "Advanced Timing" or "Custom Timing"
% settings. The shareware utility "PowerStrip" (http://www.entechtaiwan.com/util/ps.shtm)
% also allows to change and display these parameters in the Display
% Profiles -> Configure -> Advanced Timing -> Vertical Geometry -> "Total"
% field.
%
% Accuracy of beamposition method:
%
% Cross-checking of beamposition timestamps and kernel-level timestamps on
% a single display PowerPC G5 1.6 Ghz under OS-X 10.4.8 with NVidia
% GeforceFX-5200 showed an accuracy of beamposition timestamping of better
% than 100 microseconds, with a maximum deviation between the different
% methods of less than 200 microseconds.
%
% Initial checking on two Window PC's (Dell Inspiron 8000 Laptop, Geforce
% 2Go, Windows 2000, and some 3.2 Ghz Pentium-4 with NVidia Geforce 7800
% GTX) shows a precision of about 30 microseconds. Multiple users performed
% similar testing procedures on their setups and confirmed the high
% accuracy and reliability for various MacOSX and Windows setups.
%
% The results of systematic studies can be found in the PsychDocumentation/
% subfolder in the file ECVP2010Poster_VisualTimingPrecision.pdf. They
% confirm the robustness and high precision of beamposition timestamping
% and especially of Linux's builtin timestamping on a variety of tested
% hardware + operating system combinations in various system
% configurations. The pdf also provides further tips for precise visual
% onset timing.
%
% Also check the FAQ section of http://www.psychtoolbox.org for latest
% infos.
%

% History:
% 17.06.2006 Written (MK).
% 16.11.2006 Updated for Windows exp. beampos support. (MK)
%  7.07.2008 More infos and troubleshooting tips. (MK)
%  3.01.2013 Some updates and cleanups. (MK)
%  6.01.2013 Add info about OSX driver bugs with Retina displays. (MK)
% 29.09.2013 Update info to current state. (MK)
% 08.04.2014 Update info to current state: OSX fallback off by default (MK)
