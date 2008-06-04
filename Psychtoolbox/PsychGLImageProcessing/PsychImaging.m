function [rc, winRect] = PsychImaging(cmd, varargin)
% rc = PsychImaging(subcommand [,arg1][,arg2][....,argn]) - Control common
% functions of the Psychtoolbox GPU image processing pipeline.
%
% This function allows you to setup and control various aspects and common
% functions of the Psychtoolbox image processing pipeline in a simple way.
% Various standard scenarious can be conveniently set up with this routine,
% e.g., geometric transformations of your stimulus image, various types of
% display correction, ...
%
% If you want to perform less common, unusual or simply not yet supported tasks
% with the pipeline, use the low-level Screen('HookFunction', ...)
% interface instead and have a peek in the M-File code for the
% PsychImaging.m file to learn about the low-level interface.
% See "help PsychGLImageprocessing" for more info.
%
%
% Subcommands and their meaning:
%
% PsychImaging('PrepareConfiguration');
% - Prepare setup of imaging pipeline for onscreen window.
% This is the first step in the sequence of configuration steps.
%
%
% PsychImaging('AddTask', whichChannel, whichTask [,param1]...);
% - Add a specific task or processing requirement to the list of actions
% to be performed by the pipeline for the currently selected onscreen
% window. 'whichChannel' is a string with the name of the channel to
% configure:
%
% 'LeftView' applies the action to the processing channel
% for the left-eye view of a stereo configuration. 'RightView' applies the
% action to the right-eye view of a stereo configuration. 'AllViews' applies
% the action to both, left- and right eye view channels of a stereo
% configuration or to the single monoscopic channel of a mono display
% configuration. Other options are 'Compositor', 'FinalFormatting' and
% 'Finalizer' for special purpose channels. Set this to 'General' if the
% command doesn't apply to a specific view, but is a general requirement.
%
% 'whichTask' contains the name string of one of the supported
% actions:
%
% * 'InterleavedLineStereo' Ask for stereo display in interleaved mode.
%   The output image is composed from the lefteye and righteye stereo
%   buffers by interleaving their content: Even lines are filled with
%   content from the left buffer, odd lines are filled with content from
%   the right buffer, i.e., Row 0 = Left row 0, Row 1 = Right row 0, Row 2
%   = Left row 1, Row 3 = Right row1, ....
%
%   This mode is useful for driving some types of stereo devices and
%   goggles, e.g., the iGlasses 3D Video goggles in interleaved stereo
%   mode.
%
%   Usage: PsychImaging('AddTask', 'General', 'InterleavedLineStereo', startright);
%
%   If 'startright' is zero, then even lines are taken from left buffer. If
%   'startright' is one, then even lines are taken from the right buffer.
%
%
% * 'UseVirtualFramebuffer' Ask for support of virtual framebuffer, even if
%   it isn't strictly needed, given the remaining set of requirements. Most
%   of the tasks require such a framebuffer - it gets enabled anyway. In a
%   few cases, e.g., to simplify your code (no need for special cases), it
%   may be useful to activate such a framebuffer even if it isn't strictly
%   needed. This option activates a minimal buffer with 8 bits per color
%   cmponent fixed point precision.
%
%   Usage: PsychImaging('AddTask', 'General', 'UseVirtualFramebuffer');
%
%
% * 'UseFastOffscreenWindows' Ask for support of fast Offscreen windows.
%   These use a more efficient storage, backed by OpenGL framebuffer
%   objects (FBO's). Drawing into them isn't faster, but *switching*
%   between drawing into onscreen- and offscreen windows, or switching
%   between drawing into different offscreen windows is faster. They also
%   support a couple of other advanced features and performance
%   improvements in conjunction with the imaging pipeline.
%   If you only specify this task, then you'll get the benefit of fast
%   windows, without the cost of other features of the pipeline you might
%   not need.
%
%   Usage: PsychImaging('AddTask', 'General', 'UseFastOffscreenWindows');
%
%
% * 'FloatingPoint16Bit' Ask for a 16 bit floating point precision
%   framebuffer. This allows more than 8 bit precision for complex drawing,
%   compositing and image processing operations. It also allows
%   alpha-blending with signed color values and intermediate results that
%   are outside the displayable range, e.g., negative. Precision is about 3
%   digits behind the decimal point or 1024 discriminable displayable
%   levels. If you need higher precision, choose 'FloatingPoint32Bit'.
%
%   Usage: PsychImaging('AddTask', 'General', 'FloatingPoint16Bit');
%
%
% * 'FixedPoint16Bit' Ask for a 16 bit integer precision framebuffer.
%   This allows for 16 bits of precision for complex drawing. This allows
%   to use a similar precision as the 'FloatingPoint32Bit' mode for
%   high-precision display devices, but at a higher speed and lower memory
%   requirements. However, alpha-blending is not supported, intermediate
%   out-of-range values (smaller than zero or bigger than one) aren't
%   supported either. Additionally this mode is only supported on ATI
%   hardware. It is a special purpose intermediate solution - more accurate
%   than 16 bit floating point, but less capable and less accurate than 32
%   bit floating point. If you need higher precision, choose 'FloatingPoint32Bit'.
%
%   Usage: PsychImaging('AddTask', 'General', 'FixedPoint16Bit');
%
%
% * 'FloatingPoint32Bit' Ask for a 32 bit floating point precision
%   framebuffer. This allows more than 8 bit precision for complex drawing,
%   compositing and image processing operations. It also allows
%   alpha-blending with signed color values and intermediate results that
%   are outside the displayable range, e.g., negative. Precision is about
%   6.5 digits behind the dezimal point or 8 million discriminable displayable
%   levels. Be aware that only the most recent hardware (NVidia Geforce
%   8000 series, ATI Radeon HD 2000 series) is able to perform
%   alpha-blending at full speed in this mode. Enabling alpha-blending on
%   older hardware may cause a significant decrease in drawing performance,
%   or alpha blending may not work at all at this precision! If you'd like
%   to have both, the highest precision and support for alpha-blending,
%   specify 'FloatingPoint32BitIfPossible' instead. PTB will then try to
%   use 32 bit precision if this is possible in combination with alpha
%   blending. Otherwise, it will choose 16 bit precision for drawing &
%   blending, but 32 bit precision at least for the post-processing.
%
%   Usage: PsychImaging('AddTask', 'General', 'FloatingPoint32Bit');
%
%
% * 'FloatingPoint32BitIfPossible' Ask PTB to choose the highest precision
%   that is possible on your hardware without sacrificing functionality like,
%   e.g., alpha-blending. PTB will choose the best compromise possible for
%   your hardware setup.
%
%   Usage: PsychImaging('AddTask', 'General', 'FloatingPoint32BitIfPossible');
%
%
% * 'NormalizedHighresColorRange' Ask PTB to use a normalized range of
%   color and luminance intensity levels in the interval [0; 1], ie. values
%   between zero and one for minimum and maximum intensity. Also ask for
%   unclamped colors -- intermediate results are allowed to take on
%   arbitrary values, e.g., also negative values. All Screen() 2D drawing
%   commands should operate at maximum color/luminance precision.
%
%   This is just a convenience shortcut for Screen('ColorRange', win, 1, 0);
%   with the added benefit of allowing to specify the background clear
%   color in normalized 0-1 range as well. This command is implied by use
%   of any of the high precision display device drivers (for attenuators,
%   Bits+ box etc.). It is only needed if you want to create the same
%   visual results on a 8 bit standard framebuffer without needing to
%   change your code.
%
%   Usage: PsychImaging('AddTask', 'General', 'NormalizedHighresColorRange');
%
%
% * 'DisplayColorCorrection' Select a method for color correction to apply to
%   stimuli before output conversion and display. You have to specify a
%   color correction method 'methodname' to apply as parameter, see "help
%   PsychColorCorrection" for an overview of supported color correction
%   methods and their adjustable parameters. The imaging pipeline will be
%   set up to support the chosen color correction method. After you've
%   opened the onscreen window, you can use the different subcommands of
%   PsychColorCorrection() to change parameters of the color correction
%   algorithm at runtime.
%
%   Usage: PsychImaging('AddTask', whichView, 'DisplayColorCorrection', methodname);
%
%   Example: PsychImaging('AddTask', 'FinalFormatting', 'DisplayColorCorrection', 'SimpleGamma');
%   This would apply a simple power-law gamma correction to all view
%   channels of a stereo setup, or the single view of a monoscopic setup.
%   Later on you could use the methods of PsychColorCorrection() to
%   actually set the wanted gamma correction factors.
%
%   Please note that we use the channel 'FinalFormatting' instead of
%   'AllViews' as we'd usually do. Both specs will work, but a selection
%   of 'FinalFormatting' will lead to faster processing in many cases, so
%   this is preferred here if you want to apply the same setting to all
%   view channels - or a single monoscopic display.
%
%
% * 'EnablePseudoGrayOutput' Enable the high-performance driver for the
%   rendering of up to 1786 different levels of gray on a standard - but
%   well calibrated - color monitor and 8 bit graphics card. This is done
%   by applying an algorithn known as "Pseudo-Gray" or "Bit stealing".
%   Selecting this mode implies use of 32 bit floating point
%   framebuffers, unless you specify use of a 16 bit floating point
%   framebuffer via 'FloatingPoint16Bit' explicitely. If you do that, you
%   will not quite be able to use the full 10.8 bit output precision, but
%   only approximately 10 bits. The expected range of luminance values is
%   between 0 and 1. See "help CreatePseudoGrayLUT" for further
%   explanation.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnablePseudoGrayOutput');
%
%
% * 'EnableGenericHighPrecisionLuminanceOutput'
%   Setup Psychtoolbox for conversion of high precision luminance images
%   into a format suitable for special high precision luminance display
%   devices. This is a generic support routine that uses LUT based
%   conversion.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableGenericHighPrecisionLuminanceOutput', lut);
%
%
% * 'EnableVideoSwitcherSimpleLuminanceOutput'
%   Setup Psychtoolbox for conversion of high precision luminance images
%   into a format suitable for driving the "VideoSwitcher" high precision
%   luminance display device which was developed by Xiangrui Li et al.
%
%   This implements the simple converter, which only needs the
%   Blue-To-Red-Ratio of the device as input parameter and performs
%   conversion via a closed-form formula without any need for lookup
%   tables. This is supposed to be fast.
%
%   See "help VideoSwitcher" for more info about the device and its
%   options.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableVideoSwitcherSimpleLuminanceOutput' [, btrr] [, trigger]);
%
%   - The optional 'btrr' parameter is the Blue-To-Red-Ratio to use. If the
%   parameter is left out, the btrr value will be read from a global
%   configuration file.
%
%   - The optional 'trigger' parameter can be zero for "No trigger", or 1
%   for "Use trigger as configured". By default, trigger is off (==0).
%   Enabled, one can use the VideoSwitcher('SetTrigger', ...); function to
%   configure when and how a trigger signal should be emitted. Trigger
%   signals are simply specific pixel patterns in the green output channel.
%   That channel is recognized by the VideoSwitcher as a trigger signal
%   control channel.
%
%
% * 'EnableVideoSwitcherCalibratedLuminanceOutput'
%   Setup Psychtoolbox for conversion of high precision luminance images
%   into a format suitable for driving the "VideoSwitcher" high precision
%   luminance display device which was developed by Xiangrui Li et al.
%
%   This implements the simple converter, which only needs the
%   Blue-To-Red-Ratio of the device as input parameter and performs
%   conversion via a closed-form formula without any need for lookup
%   tables. This is supposed to be fast.
%
%   See "help VideoSwitcher" for more info about the device and its
%   options.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableVideoSwitcherCalibratedLuminanceOutput' [, btrr] [, lut] [, trigger]);
%
%   - The optional 'btrr' parameter is the Blue-To-Red-Ratio to use. If the
%   parameter is left out, the btrr value will be read from a global
%   configuration file.
%
%   - The optional 'lut' paramter is a 257 elements vector of luminance
%   values, which maps blue channel drive indices to luminance values. This
%   lut needs to be acquired via a calibration procedure by use of a
%   photometer. If 'lut' is left out, the table will be read from a global
%   configuration file.
%
%   - The optional 'trigger' parameter can be zero for "No trigger", or 1
%   for "Use trigger as configured". By default, trigger is off (==0).
%   Enabled, one can use the VideoSwitcher('SetTrigger', ...); function to
%   configure when and how a trigger signal should be emitted. Trigger
%   signals are simply specific pixel patterns in the green output channel.
%   That channel is recognized by the VideoSwitcher as a trigger signal
%   control channel.
%
%
% * 'EnableNative10BitFramebuffer' Enable the high-performance driver and
%   support for output of stimuli with 10 bit precision per color channel
%   (10 bpc) on graphics hardware that supports native 10 bpc framebuffers.
%   Currently, ATI/AMD Radeon hardware of the X1000/HD2000/HD3000 series
%   and later models should support a native ARGB2101010 framebuffer, ie.,
%   a system framebuffer with 2 bits for the alpha channel, and 10 bits per
%   color channel.
%
%   As this is supported by the hardware, but not by the standard ATI
%   graphics drivers, we follow a hybrid approach: We use a special kernel
%   level driver to reconfigure the hardware for 10bpc framebuffer support.
%   Then we use a special imaging pipeline formatting plugin to convert
%   16bpc or 32bpc stimuli into the special data format required by this
%   framebuffer configuration.
%
%   You'll need to install and load the special Psychtoolbox kernel driver
%   and you'll need to have a supported gfx-card for this to work! This
%   feature is highly experimental and not guaranteed to work reliable on
%   any system configuration. Read 'help PsychtoolboxKernelDriver' for info
%   about the driver and installation instructions.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableNative10BitFramebuffer');
%
% * 'EnableBrightSideHDROutput' Enable the high-performance driver for
%   BrightSide Technologies High dynamic range display device for 16 bit
%   per color channel output precision. See "help BrightSideHDR" for
%   detailed explanation. Please note that you'll need to install the 3rd
%   party driver libraries for that display as described in the help file.
%   PTB doesn't come bundled with that libraries for copyright reasons.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableBrightSideHDROutput');
%
%
% * 'EnableBits++Bits++Output' Setup Psychtoolbox for Bits++ mode of the
%   Cambridge Research Systems Bits++ box. This loads the graphics
%   hardwares gamma table with an identity mapping so it can't interfere
%   with Bits++ T-Lock system. It also sets up automatic generation of
%   Bits++ T-Lock codes: You will be able to upload new CLUT's into the
%   Bits++ by use of the Screen('LoadNormalizedGammaTable', window, clut, 2);
%   command. CLUT updates will be synchronized with Screen('Flip')
%   commands, because PTB will generate and draw the proper T-Lock code
%   into the top line of your onscreen window. Please note that while
%   Bits++ CLUT mode works even with very old graphics hardware, this is a
%   pretty cumbersome way of driving the Bits++. On recent hardware, you
%   will want to use Mono++ or Color++ mode (see below). That allows to
%   draw arbitrarily complex stimuli with as many colors as you want and
%   PTB will take care of conversion into the Color++ or Mono++ format for
%   Bits++.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableBits++Bits++Output');
%
%
% * 'EnableBits++Mono++Output' Enable the high-performance driver for the
%   Mono++ mode of Cambridge Research Systems Bits++ box. This is the
%   fastest and most elegant way of driving the Bits++ box with 14 bit
%   luminance output precision. See "help BitsPlusPlus" for more
%   information. Selecting this mode implies use of 32 bit floating point
%   framebuffers, unless you specify use of a 16 bit floating point
%   framebuffer via 'FloatingPoint16Bit' explicitely. If you do that, you
%   will not be able to use the full 14 bit output precision of Bits++, but
%   only approximately 10 bits.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableBits++Mono++Output');
%
%   If you want to make use of the color overlay plane in Mono++ mode, then
%   call the function like this:
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableBits++Mono++OutputWithOverlay');
%
%   Then you can query the window handle of the overlay window via:
%
%   overlayWin = BitsPlusPlus('GetOverlayWindow', window);
%
%   'overlayWin' is the handle to the overlay window associated with the
%   overlay of onscreen window 'window'. The overlay window is a standard
%   offscreen window, so you can do anything with it that you would want to
%   do with offscreen windows. The only difference is that the window is a
%   pure index window: It only has one "color channel", which can be written
%   with color values between 0 and 255. Values 1 to 255 get mapped to the
%   corresponding color indices of the Bits++ overlay plane: A zero value is
%   transparent -- Content of the onscreen window is visible. Positive
%   non-zero color values map to the 255 indices available in overlay mode,
%   these get mapped by the Bits++ CLUT to colors. You can define the
%   mapping of indices to CLUT colors via the
%   Screen('LoadNormalizedGammaTable', window, clut, 2); command.
%
%   Updates of the overlay image are synchronized to Screen('Flip')
%   updates. If you draw into the overlay window, the changed overlay image
%   will become visible at Screen('Flip') time -- in sync with the changed
%   onscreen window content. The overlay plane is not automatically cleared
%   to background (or transparent) color after a flip, but its content
%   persists across flips. You need to clear it out manually via a
%   Screen('FillRect') command.
%
% * 'EnableBits++Color++Output' Enable the high-performance driver for the
%   Color++ mode of Cambridge Research Systems Bits++ box. This is the
%   fastest and most elegant way of driving the Bits++ box with 14 bit
%   per color channel output precision. See "help BitsPlusPlus" for more
%   information. Selecting this mode implies use of 32 bit floating point
%   framebuffers, unless you specify use of a 16 bit floating point
%   framebuffer via 'FloatingPoint16Bit' explicitely. If you do that, you
%   will not be able to use the full 14 bit output precision of Bits++, but
%   only approximately 10 bits.
%
%   Usage: PsychImaging('AddTask', 'General', 'EnableBits++Color++Output');
%
%
% * 'AddOffsetToImage' Add a constant color- or intensity offset to the
%   drawn image, prior to all following image processing and post
%   processing operations:
%   Outimage(x,y) = Inimage(x,y) + Offset. If the framebuffer is in a color
%   display mode, the same offset will be added to all three color
%   channels.
%
%   Usage: PsychImaging('AddTask', whichView, 'AddOffsetToImage', Offset);
%   Example: PsychImaging('AddTask', 'AllViews', 'AddOffsetToImage', 0.5);
%
%
% * 'MirrorDisplayTo2ndOutputHead' Mirror the content of the onscreen
%   window to given 2nd screen, ie., to a 2nd output connector (head)
%   of a dualhead graphics card. This should give the same result as if one
%   switches the graphics card into "Mirror mode" or "Clone mode" via the
%   display settings panel of your operating system. Use of the "Mirror
%   Mode" or "Clone Mode" of your operating system and graphics card is
%   preferable to use of this command, if that works for you. The OS
%   builtin facilities are usually faster, more efficient and thereby
%   more reliable wrt. timing and synchronization!
%
%   This function only works for monoscopic displays, ie., it can not be
%   used simultaneously with any stereo display mode. The reason is that it
%   internally uses stereomode 10 with a few modifications to get its job
%   done, so obviously neither mode 10 nor any other mode can be used
%   without interference.
%
%   Only use this function for mirroring onto the 2nd head of a dual-head
%   graphics card under MacOS/X, or if you need to mirror onto a 2nd head
%   on MS-Windows and can't use "desktop spanning" mode on Windows to
%   achieve dual display output. If possible on your setup and OS, rather use
%   'MirrorDisplayToSingleSplitWindow' (see below). That mode should work
%   well on dual-head graphics cards on MS-Windows or GNU/Linux, as well as
%   in conjunction with a hardware display splitter attached to a single
%   head on any operating system. It has the advantage of consuming less
%   memory and compute ressources, so it is potentially faster or provides
%   a more reliable overall timing.
%
%   Usage: PsychImaging('AddTask', 'General', 'MirrorDisplayTo2ndOutputHead', mirrorscreen);
%
%   The content of the onscreen window shall be shown not only on the
%   display associated with the screen given to PsychImaging('OpenWindow',
%   ...); but also (as a copy) on the screen with the index 'mirrorscreen'.
%
%
% * 'MirrorDisplayToSingleSplitWindow' Mirror the content of the onscreen
%   window to the right half of the desktop (if desktop spanning on a
%   dual-display setup is enabled) or the right-half of the virtual screen
%   if a display splitter (e.g., Matrox Dualhead2Go (TM)) is attached to a
%   single head of a graphics card. This should give the same result as if one
%   switches the graphics card into "Mirror mode" or "Clone mode" via the
%   display settings panel of your operating system. Use of the "Mirror
%   Mode" or "Clone Mode" of your operating system and graphics card is
%   preferable to use of this command, if that works for you. The OS
%   builtin facilities are usually faster, more efficient and thereby
%   more reliable wrt. timing and synchronization!
%
%   Usage: PsychImaging('AddTask', 'General', 'MirrorDisplayToSingleSplitWindow');
%
%   Optionally, you can add the command...
%   PsychImaging('AddTask', 'General', 'DontUsePipelineIfPossible');
%   ... if you don't intend to use the imaging pipeline for anything else
%   than display mirroring. This will allow further optimizations.
%
%
% * 'RestrictProcessing' Restrict stimulus processing to a specific subarea
%   of the screen. If your visual stimulus only covers a subarea of the
%   display screen you can restrict PTB's output processing to that
%   subarea. This may save some computation time to allow for higher
%   display redraw rates.
%
%   Usage: PsychImaging('AddTask', whichChannel, 'RestrictProcessing', ROI);
%
%   ROI is a rectangle defining the area to process ROI = [left top right bottom];
%   E.g., ROI = [400 400 800 800] would only create output pixels in the
%   screen area with top-left corner (400,400) and bottom-right corner
%   (800, 800).
%
%
% * 'FlipHorizontal' and 'FlipVertical' flip your output images
%   horizontally (left- and right interchanged) or vertically (upside down).
%
%   Usage: PsychImaging('AddTask', whichChannel, 'FlipHorizontal');
%   Usage: PsychImaging('AddTask', whichChannel, 'FlipVertical');
%
%
% * 'GeometryCorrection' Apply some geometric warping operation during
%   rendering of the final stimulus image to correct for geometric
%   distortion of your physical display device. You need to measure the
%   geometric distortion of your display with a suitable calibration
%   procedure, then compute an inverse warp transformation to undo this
%   distortion, then provide that transformation to this function.
%
%   Usage: PsychImaging('AddTask', whichChannel, 'GeometryCorrection', calibfilename [, debugoutput] [, arg1], [arg2], ...);
%
%   'calibfilename' is the filename of a calibration file which specified
%   the type of undistortion to apply. Calibration files can be created by
%   interactive calibration procedures. See 'help CreateDisplayWarp' for a
%   list of calibration methods. One of the supported procedures is, e.g.,
%   "DisplayUndistortionBezier", read "help DisplayUndistortionBezier". The
%   recommended method for most cases is 'DisplayUndistortionBVL', read
%   "help DisplayUndistortionBVL" for help.
%
%   The optional flag 'debugoutput' if set to non-zero value will trigger
%   some debug output about the calibration with some calibration methods.
%
%   The optional 'arg1', 'arg2', ..., are optional parameters whose
%   meaning depends on the calibration method in use.
%
%
% * More actions will be supported in the future. If you can think of an
%   action of common interest not yet supported by this framework, please
%   file a feature request on our Wiki (Mainpage -> Feature Requests).
%
%
% After adding all wanted task specifications and other requirements,
% call...
%
% [windowPtr, windowRect] = PsychImaging('OpenWindow', screenid, [backgroundcolor], ....);
%
% - Finishes the setup phase for imaging pipeline, creates a suitable onscreen
% window and performs all remaining configuration steps. After this
% command, your onscreen window will be ready for drawing and display of
% stimuli. All specified imaging operations will get automatically applied
% to your stimulus before stimulus onset.
%
%
% After the window has been opened you can call the following commands any
% time at runtime:
%
% PsychImaging('RestrictProcessingToROI', window, whichChannel, ROI);
% - Restrict the processing area of viewChannel 'whichChannel' of onscreen
% window 'window' to the rectangular subarea defined by 'ROI'. See the
% explanation above for subtask 'RestrictProcessing'. This does exactly the
% same but allows a dynamic change of the restricted area at any point
% during your experiment script.
%
%
% PsychImaging('UnrestrictProcessing', window, whichChannel);
% - Remove a restriction of the processing area of viewChannel
% 'whichChannel' of onscreen window 'window' to a previously defined
% subarea. Can be called anytime during your scripts execution.
%
%
% The following commands are only for specialists:
%
% imagingMode = PsychImaging('FinalizeConfiguration');
% - Finish the configuration phase for this window. This will compute an
% optimal configuration for all stages of the pipeline, but won't apply it
% yet. You'll have to call Screen('OpenWindow', windowPtr, ......,
% imagingMode, ...); with the returned 'imagingMode' + any other options
% you'd like to have for your window. After that, you'll have to call
% PsychImaging('PostConfiguration') to really apply and setup all your
% configuration settings. If you don't have unusual needs, you can simplify
% these steps by simply calling PsychImaging('OpenWindow', ....);
% with the same parameters that you'd pass to Screen('OpenWindow', ....);
% PsychImaging will perform all necessary steps to upon return, you'll have
% your window properly configured.
%
%
% PsychImaging('PostConfiguration', windowPtr [, clearcolor]);
% - To be called after opening the onscreen window 'windowPtr'.
% Performs all the setup work to be done after the window was created.
%
%

% History:
% 3.6.2007 Written. (MK)
%
% 19.7.2007 Added our own implementation mystrcmp() of Matlabs strcmp().
% The strcmp() of Octave is not as powerful as Matlab 7's strcmp(). We need
% a powerful implementation that works for both runtime environments, so we
% reimplemented in in a portable way.
%
% 19.7.2007 Added initial support for display geometry correction. (MK).
%
% 27.9.2007 Added support for floating point framebuffer, Bits++ and
%           Brightside-HDR. Documentation cleanup. (MK).
%
% 13.1.2008 Support for 10 bpc native framebuffer of ATI Radeons. (MK).
% 17.4.2008 Support for a few new subcommands, and description of overlay
%           planes setup with Bits++ in Mono++ mode. (MK).
% 18.5.2008 A few bug fixes and support for 'DisplayColorCorrection' setup
%           code: Now a central solution that will work for all current and
%           future output devices (hopefully). (MK).

persistent configphase_active;
persistent reqs;

% This flag is global - needed in subfunctions as well (ugly ugly coding):
global ptb_outputformatter_icmAware;

if isempty(configphase_active)
    configphase_active = 0;
    ptb_outputformatter_icmAware = 0;
end

if nargin < 1 || isempty(cmd)
    error('You did not provide any subcommand to execute!');
end

rc = [];
winRect = [];

if strcmp(cmd, 'PrepareConfiguration')
    % Prepare new configuration:
    if configphase_active
        error('Tried to prepare a new configuration phase, but you did not finalize the previous phase yet!');
    end
    
    configphase_active = 1;
    clear reqs;
    reqs = [];
    ptb_outputformatter_icmAware = 0;
    
    rc = 0;
    return;
end

if strcmp(cmd, 'AddTask')
    if nargin < 3 || isempty(varargin{1}) || isempty(varargin{2})
        error('Parameters missing: Need at least "whichChannel" and "whichTask"!');
    end
    
    if configphase_active ~= 1
        error('Call PsychImaging(''PrepareConfiguration''); first to prepare the configuration phase!');
    end
    
    % Store requirement in our cell array of requirements. We need to
    % extend each requirement vector to some number of max elements, so all
    % rows in the cell array have the same length:
    x = varargin;
    maxreqarg = 10;
    if length(x) < maxreqarg
        for i=length(x)+1:maxreqarg
            x{i}='';
        end
    end

    % First use of 'reqs' array?
    if isempty(reqs)
        % Yes: Initialize the array with content of 'x':
        reqs = x;
    else
        % No: Just concatenate new line with requirements 'x' to existing
        % array 'reqs':
        reqs = [reqs ; x];
    end
    
    rc = 0;
    return;
end

if strcmp(cmd, 'FinalizeConfiguration')
    if configphase_active ~= 1
        error('You tried to finalize configuration, but no configuration in progress!');
    end

    if isempty(reqs)
        error('You tried to FinalizeConfiguration, but you did not specify any requirements or tasks!');
    end

    configphase_active = 2;
    
    % Compute correct imagingMode - Settings for current configuration and
    % return it:
    rc = FinalizeConfiguration(reqs);
    return;
end

if strcmp(cmd, 'PostConfiguration')
    if configphase_active ~= 2
        error('Tried to call PostConfiguration without calling FinalizeConfiguration before!');
    end
    
    if nargin < 2 || isempty(varargin{1}) || Screen('WindowKind', varargin{1})~=1
        error('No "windowPtr" or invalid "windowPtr" or non-onscreen window handle provided!');
    end

    if nargin < 3
        clearcolor = 0;
    else
        clearcolor = varargin{2};
    end
    
    rc = PostConfiguration(reqs, varargin{1}, clearcolor);

    configphase_active = 0;
    return;
end
    
if strcmp(cmd, 'OpenWindow')
    if configphase_active ~= 1
        error('You tried to OpenImagingWindow, but didn''t specify any imaging configuration!');
    end

    if nargin < 2
        error('You must supply at least a "screenId" for the screen on which the window should be opened');
    end
    
    % Final config phase:
    configphase_active = 2;
    
    screenid = varargin{1};
    
    if nargin < 3 || isempty(varargin{2})
        clearcolor = [];
    else
        clearcolor = varargin{2};
    end
    
    if nargin < 4 || isempty(varargin{3})
        winRect = [];
    else
        winRect = varargin{3};
    end
    
    if ~isempty(find(mystrcmp(reqs, 'EnableNative10BitFramebuffer')))
        % Request a pixelsize of 30 bpp to enable native 2101010
        % framebuffer support:
        pixelSize = 30;
    else
        % Ignore pixelSize:
        pixelSize = [];
    end
    
    % Override numbuffers -- always 2:
    numbuffers = 2;
        
    if nargin < 7 || isempty(varargin{6})
        stereomode = 0;
    else
        stereomode = varargin{6};
    end
        
    % Compute correct imagingMode - Settings for current configuration and
    % return it:
    [imagingMode needStereoMode] = FinalizeConfiguration(reqs, stereomode);

    % Override stereomode derived from requirements?
    if needStereoMode ~= -1
        if needStereoMode == -2 && stereomode == 0
            % Stereo operation needed, but not set up by usercode:
            error('Your requirements demand a stereo presentation mode, but you didn''t specify one!');
        else
            if (needStereoMode > -1) && (stereomode ~= needStereoMode)
                % Need a specific mode: Override current setting by our needs:
                stereomode = needStereoMode;

                % Give feedback about stereomode override. If the user
                % didn't provide a stereomode, we just output an info.
                % Otherweise we output a warning about the conflict and our
                % override...
                if nargin < 7 || isempty(varargin{6})
                    fprintf('PsychImaging-Info: Stereomode %i required - Enabling it.\n', stereomode);
                else
                    warning('Your provided "stereomode" conflicts with required stereomode for imaging pipeline. Overriden...');
                end
            end
        end
    end
    
    if nargin < 8 || isempty(varargin{7})
        multiSample = 0;
    else
        multiSample = varargin{7};
    end
    
    if nargin < 9 || isempty(varargin{8})
        imagingovm = 0;
    else
        imagingovm = varargin{8};
    end
    
    imagingMode = mor(imagingMode, imagingovm);
    
    % Custom color correction for display wanted on a Bits+ display in
    % Mono++ or Color++ mode?
    if ~isempty(find(mystrcmp(reqs, 'DisplayColorCorrection')))
        if ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++Output'))) || ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++OutputWithOverlay'))) || ~isempty(find(mystrcmp(reqs, 'EnableBits++Color++Output')))
            % Yes. The BitsPlusPlus() setup routine implements its own
            % setup code for display color correction, which is very
            % efficient on a monoscopic setup, but potentially problematic
            % on a multi-display / stereoscopic setup. Need to handle both
            % cases specially:
            if stereomode == 0
                % Purely monoscopic. Use BitsPlusPlus internal setup code,
                % just provide proper method setting for it now:
                floc = find(mystrcmp(reqs, 'DisplayColorCorrection'));
                if length(floc)>1
                    error('In monoscopic display mode there must be only one single spec of "DisplayColorCorrection" not multiple!');
                end
                
                % Which channel?
                x=floc;
                [rows cols]= ind2sub(size(reqs), x);
                for row=rows'
                    % Extract first parameter - This should be the method of correction:
                    colorcorrectionmethod = reqs{row, 3};

                    if isempty(colorcorrectionmethod) | ~ischar(colorcorrectionmethod)
                        Screen('CloseAll');
                        error('PsychImaging: Name of color correction method for ''DisplayColorCorrection'' missing or not of string type!');
                    end

                    % Select method:
                    PsychColorCorrection('ChooseColorCorrection', colorcorrectionmethod);
                end
            else
                % Stereoscopic: Select special method which won't be
                % harmful, a simple clamping to valid range, labeled with a
                % special name that can't clash with our own definition of
                % ICM shaders:
                PsychColorCorrection('ChooseColorCorrection', 'ClampedNoName');
            end
        end        
    end

    % Open onscreen window with proper imagingMode and stereomode set up.
    % We have a couple of special cases here for BrightSide HDR display and
    % the CRS Bits++...
    win = [];

    if ~isempty(find(mystrcmp(reqs, 'EnableBrightSideHDROutput')))
        % Special case: Need to open BrightSide HDR driver. We delegate the
        % openwindow procedure to the BrightSideHDR.m file:
        if ~isempty(win)
            error('You specified multiple conflicting output display device drivers! This will not work.');
        end

        if nargin > 10
            [win, winRect] = BrightSideHDR('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode, varargin{9:end});
        else
            [win, winRect] = BrightSideHDR('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode);
        end        
    end

    if ~isempty(find(mystrcmp(reqs, 'EnableBits++Bits++Output')))
        % Special case: Need to open Bits++ Bits++ driver. We delegate the
        % openwindow procedure to the BitsPlusPlus.m file:
        if ~isempty(win)
            error('You specified multiple conflicting output display device drivers! This will not work.');
        end

        if nargin > 10
            [win, winRect] = BitsPlusPlus('OpenWindowBits++', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode, varargin{9:end});
        else
            [win, winRect] = BitsPlusPlus('OpenWindowBits++', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode);
        end        
    end

    if ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++Output'))) || ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++OutputWithOverlay')))
        % Special case: Need to open Bits++ Mono++ driver. We delegate the
        % openwindow procedure to the BitsPlusPlus.m file:
        if ~isempty(win)
            error('You specified multiple conflicting output display device drivers! This will not work.');
        end

        if ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++OutputWithOverlay')))
            bpcom = 'OpenWindowMono++WithOverlay';
        else
            bpcom = 'OpenWindowMono++';
        end
        
        if nargin > 10
            [win, winRect] = BitsPlusPlus(bpcom, screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode, varargin{9:end});
        else
            [win, winRect] = BitsPlusPlus(bpcom, screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode);
        end        
    end

    if ~isempty(find(mystrcmp(reqs, 'EnableBits++Color++Output')))
        % Special case: Need to open Bits++ Color++ driver. We delegate the
        % openwindow procedure to the BitsPlusPlus.m file:
        if ~isempty(win)
            error('You specified multiple conflicting output display device drivers! This will not work.');
        end

        if nargin > 10
            [win, winRect] = BitsPlusPlus('OpenWindowColor++', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode, varargin{9:end});
        else
            [win, winRect] = BitsPlusPlus('OpenWindowColor++', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode);
        end        
    end

    if isempty(win)
        % Standard openwindow path:
        if ~isempty(win)
            error('You specified multiple conflicting output display device drivers! This will not work.');
        end

        if nargin > 10
            [win, winRect] = Screen('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode, varargin{9:end});
        else
            [win, winRect] = Screen('OpenWindow', screenid, clearcolor, winRect, pixelSize, numbuffers, stereomode, multiSample, imagingMode);
        end
    end
    
    % Display mirroring in stereomode 10 requested?
    if ~isempty(find(mystrcmp(reqs, 'MirrorDisplayTo2ndOutputHead')))
        % Yes. Need to open secondary slave window:
        floc = find(mystrcmp(reqs, 'MirrorDisplayTo2ndOutputHead'));
        [rows cols]= ind2sub(size(reqs), floc);
        % Extract first parameter - This should be the name of a
        % calibration file:
        slavescreenid = reqs{rows, 3};

        if isempty(slavescreenid)
            Screen('CloseAll');
            error('In PsychImaging MirrorDisplayTo2ndOutputHead: You must provide the index of the secondary screen "slavescreen"!');
        end
        
        if ~any(ismember(Screen('Screens'), slavescreenid))
            Screen('CloseAll');
            error('In PsychImaging MirrorDisplayTo2ndOutputHead: You must provide the index of a valid secondary screen "slavescreen"!');
        end
        
        Screen('OpenWindow', slavescreenid, [255 0 0], [], [], [], stereomode);
    end

    
    % Perform double-flip, so both back- and frontbuffer get initialized to
    % background color:
    Screen('Flip', win);
    Screen('Flip', win);
    
    % Window open. Perform imaging pipe postconfiguration:
    PostConfiguration(reqs, win, clearcolor);

    rc = win;
    
    % Done.
    configphase_active = 0;

    return;
end

if strcmp(cmd, 'RestrictProcessingToROI')
    % Define a ROI in a processing chain/channel to which processing should
    % be restricted by internal use of glScissor() command. This is a
    % runtime function. Each invocation will search the given channel if
    % such a command already exists, then delete it if so. It will prepend
    % the new command with the new spec in any case, so that at any point
    % in time exactly one such ROI can be active for a chain:

    if nargin < 4
        Screen('CloseAll');
        error('You must provide all parameters for subfunction "RestrictProcessingToROI!"');
    end
    
    % Extract window handle:
    win = varargin{1};
    
    if ~isscalar(win) || ~isnumeric(win) || Screen('WindowKind', win) ~= 1
        Screen('CloseAll');
        error('Provided window parameter for subfunction "RestrictProcessingToROI!" is not the handle of a valid onscreen window!');
    end
    
    % Extract window information:
    winfo = Screen('GetWindowInfo', win);
    
    % Extract view channel:
    whichView = varargin{2};
    
    % Extract scissor rectangle:
    scissorrect = varargin{3};
    
    if size(scissorrect,1)~=1 || size(scissorrect,2)~=4
        Screen('CloseAll');
        error('Command "RestrictProcessingToROI" in channel %s expects a 1-by-4 ROI rectangle to define the ROI, e.g, [left top right bottom]!', whichView);
    end

    ox = scissorrect(RectLeft);
    oy = scissorrect(RectTop);
    w  = RectWidth(scissorrect);
    h  = RectHeight(scissorrect);

    if mystrcmp(whichView, 'LeftView') || mystrcmp(whichView, 'AllViews')
        % Need to restrict left view processing:
        DoRemoveScissorRestriction(win, 'StereoLeftCompositingBlit');
        Screen('HookFunction', win, 'PrependBuiltin', 'StereoLeftCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
    end

    if mystrcmp(whichView, 'RightView') || mystrcmp(whichView, 'AllViews')
        % Need to restrict right view processing:
        DoRemoveScissorRestriction(win, 'StereoRightCompositingBlit');
        Screen('HookFunction', win, 'PrependBuiltin', 'StereoRightCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
    end

    if (mystrcmp(whichView, 'AllViews') || mystrcmp(whichView, 'Compositor')) && winfo.StereoMode > 5
        % Needed to restrict both views processing and a
        % compositing mode is active. If both views are restricted
        % in their output area then it makes sense to restrict the
        % compositor to the same area. We also restrict the
        % compositor if that was requested.
        DoRemoveScissorRestriction(win, 'StereoCompositingBlit');
        Screen('HookFunction', win, 'PrependBuiltin', 'StereoCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
    end

    if mystrcmp(whichView, 'FinalFormatting')
        % Need to restrict final formatting blit processing:
        DoRemoveScissorRestriction(win, 'FinalOutputFormattingBlit');
        Screen('HookFunction', win, 'PrependBuiltin', 'FinalOutputFormattingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
    end

    return;
end

if strcmp(cmd, 'UnrestrictProcessing')
    % Remove a ROI in a processing chain/channel to which processing should
    % be restricted by internal use of glScissor() command. This is a
    % runtime function. Each invocation will search the given channel if
    % such a command exists, then delete it if so.
    if nargin < 3
        Screen('CloseAll');
        error('You must provide all parameters for subfunction "UnrestrictProcessing!"');
    end
    
    % Extract window handle:
    win = varargin{1};
    
    if ~isscalar(win) || ~isnumeric(win) || Screen('WindowKind', win) ~= 1
        Screen('CloseAll');
        error('Provided window parameter for subfunction "UnrestrictProcessing!" is not the handle of a valid onscreen window!');
    end
    
    % Extract window information:
    winfo = Screen('GetWindowInfo', win);
    
    % Extract view channel:
    whichView = varargin{2};
    
    if mystrcmp(whichView, 'LeftView') || mystrcmp(whichView, 'AllViews')
        % Need to restrict left view processing:
        DoRemoveScissorRestriction(win, 'StereoLeftCompositingBlit');
    end

    if mystrcmp(whichView, 'RightView') || mystrcmp(whichView, 'AllViews')
        % Need to restrict right view processing:
        DoRemoveScissorRestriction(win, 'StereoRightCompositingBlit');
    end

    if (mystrcmp(whichView, 'AllViews') || mystrcmp(whichView, 'Compositor')) && winfo.StereoMode > 5
        % Needed to restrict both views processing and a
        % compositing mode is active. If both views are restricted
        % in their output area then it makes sense to restrict the
        % compositor to the same area. We also restrict the
        % compositor if that was requested.
        DoRemoveScissorRestriction(win, 'StereoCompositingBlit');
    end

    if mystrcmp(whichView, 'FinalFormatting')
        % Need to restrict final formatting blit processing:
        DoRemoveScissorRestriction(win, 'FinalOutputFormattingBlit');
    end

    return;
end

% Catch all for unknown commands:
error('Unknown subcommand specified! Read "help PsychImaging" for usage info.');
return;

% Internal helper routines:

% FinalizeConfiguration consolidates the current set of requirements and
% derives the needed stereoMode settings and imagingMode setting to pass to
% Screen('OpenWindow') for pipeline preconfiguration.
function [imagingMode, stereoMode] = FinalizeConfiguration(reqs, userstereomode)
global ptb_outputformatter_icmAware;

if nargin < 2
    userstereomode = [];
end

if isempty(userstereomode)
    userstereomode = 0;
end

% Set imagingMode to minimum: Pipeline disabled. All latter task
% requirements will setup imagingMode to fullfill their needs. A few
% tasks/requirements don't need the full pipeline at all. E.g, Support for
% fast offscreen windows only needs that, but not the full pipeline. Some
% of the "software based mirror modes" herein only need the finalizer blit
% chains, but not the imaging pipeline. Bits++ setup for pure CLUT imaging
% (Bits++ mode) doesn't need imaging pipe either...
imagingMode = 0;

% Set stereoMode to don't care:
stereoMode = -1;

% FBO backed framebuffer needed?
if ~isempty(find(mystrcmp(reqs, 'UseVirtualFramebuffer')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
end

% 16 bit integer precision framebuffer needed? This is only supported on
% ATI hardware...
if ~isempty(find(mystrcmp(reqs, 'FixedPoint16Bit')))
    imagingMode = mor(imagingMode, kPsychNeed16BPCFixed);
end

% Does usercode request a stereomode?
if userstereomode > 0
    % Enable imaging pipeline based stereo,ie., kPsychNeedFastBackingStore:
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
end

% Stereomode 6 for interleaved line stereo needed?
if ~isempty(find(mystrcmp(reqs, 'InterleavedLineStereo')))
    % Yes: Must use stereomode 6.
    stereoMode = 6;
    % We also request an effective window height that is only half the real
    % height. This affects all drawing and query commands of Screen:
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore, kPsychNeedHalfHeightWindow);    
end

% Stereomode 10 for display replication needed?
if ~isempty(find(mystrcmp(reqs, 'MirrorDisplayTo2ndOutputHead')))
    % Yes: Must use stereomode 10. This implies kPsychNeedFastBackingStore,
    % automatically set by Screen('OpenWindow') itself, so no need t do it
    % here:
    stereoMode = 10;    
end

% Custom color correction for display wanted?
if ~isempty(find(mystrcmp(reqs, 'DisplayColorCorrection')))
    % Yes. Need full pipeline in any case, ie fast backing store:
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
end

% Replication of left half of window into right half needed?
% This is used for a software implementation of mirror mode displays,
% e.g., in conjunction with desktop-spanning display mode on MS-Windows or
% in conjunction with a display splitter on a single output head:
if ~isempty(find(mystrcmp(reqs, 'MirrorDisplayToSingleSplitWindow')))
    % We simply request that window size is reported and handled as if the
    % window would be only half the width --> right half remains empty and
    % can be used as target for the cloning op of the left half.
    % This works even without imaging pipe enabled, only uses finalizer
    % blit chains:
    imagingMode = mor(imagingMode, kPsychNeedHalfWidthWindow);
end

% 16 bpc float framebuffers needed?
if ~isempty(find(mystrcmp(reqs, 'FloatingPoint16Bit')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeed16BPCFloat);    
end

% 32 bpc float framebuffers needed?
if ~isempty(find(mystrcmp(reqs, 'FloatingPoint32Bit')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeed32BPCFloat);
end

if ~isempty(find(mystrcmp(reqs, 'FloatingPoint32BitIfPossible')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychUse32BPCFloatAsap);
end

if ~isempty(find(mystrcmp(reqs, 'EnableBrightSideHDROutput')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeedOutputConversion);
    % The BrightSide formatter is not icm aware - Incapable of internal color correction!
    ptb_outputformatter_icmAware = 0;
end

if ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++Output'))) || ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++OutputWithOverlay')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeedOutputConversion);
    % The Mono++ formatter is icm aware - Capable of internal color
    % correction, but not setup here -- special case: Set flag to zero:
    ptb_outputformatter_icmAware = 0;
end

if ~isempty(find(mystrcmp(reqs, 'EnableGenericHighPrecisionLuminanceOutput'))) || ~isempty(find(mystrcmp(reqs, 'EnablePseudoGrayOutput')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeedOutputConversion);

    % The Luminance LUT based formatter is icm aware - Capable of internal color correction:
    ptb_outputformatter_icmAware = 1;

    % Request 32bpc float FBO unless already a 16 bpc FBO or similar has
    % been explicitely requested:
    if ~bitand(imagingMode, kPsychNeed16BPCFloat) && ~bitand(imagingMode, kPsychUse32BPCFloatAsap)
        imagingMode = mor(imagingMode, kPsychNeed32BPCFloat);
    end
end

if ~isempty(find(mystrcmp(reqs, 'EnableVideoSwitcherSimpleLuminanceOutput'))) || ~isempty(find(mystrcmp(reqs, 'EnableVideoSwitcherCalibratedLuminanceOutput')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeedOutputConversion);

    % The VideoSwitcher formatter is icm aware - Capable of internal color correction:
    ptb_outputformatter_icmAware = 1;

    % Request 32bpc float FBO unless already a 16 bpc FBO or similar has
    % been explicitely requested:
    if ~bitand(imagingMode, kPsychNeed16BPCFloat) && ~bitand(imagingMode, kPsychUse32BPCFloatAsap)
        imagingMode = mor(imagingMode, kPsychNeed32BPCFloat);
    end
end

if ~isempty(find(mystrcmp(reqs, 'EnableBits++Color++Output')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeedOutputConversion, kPsychNeedHalfWidthWindow);

    % The Color++ formatter is icm aware - Capable of internal color
    % correction, but not setup here -- special case: Set flag to zero:
    ptb_outputformatter_icmAware = 0;
end

% Request for native 10 bit per color component ARGB2101010 framebuffer?
if ~isempty(find(mystrcmp(reqs, 'EnableNative10BitFramebuffer')))
    % Enable output formatter chain:
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeedOutputConversion);

    % Request 32bpc float FBO unless already a 16 bpc FBO or similar has
    % been explicitely requested: In principle, a 16 bpc FBO would be
    % sufficient for a native 10bpc framebuffer...
    if ~bitand(imagingMode, kPsychNeed16BPCFloat) && ~bitand(imagingMode, kPsychUse32BPCFloatAsap)
        imagingMode = mor(imagingMode, kPsychNeed32BPCFloat);
    end

    % The ATI 10bpc formatter is not yet icm aware - Incapable of internal color correction!
    ptb_outputformatter_icmAware = 0;    
end

if ~isempty(find(mystrcmp(reqs, 'LeftView'))) || ~isempty(find(mystrcmp(reqs, 'RightView')))
    % Specific eye channel requested: Need a stereo display mode.
    stereoMode = -2;

    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);

    % Also need image processing stage, because only it can provide
    % separate processing for both eyes:
    imagingMode = mor(imagingMode, kPsychNeedImageProcessing);
else
    % Not a single eye specific command requested: Check if there's any
    % other spec that would require the image processing stage:
    % Any command that applies to 'AllViews' naturally needs the image
    % processing:
    if ~isempty(find(mystrcmp(reqs, 'AllViews')))
        imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
        imagingMode = mor(imagingMode, kPsychNeedImageProcessing);
    end
end

% Image processing stage needed?
if imagingMode & kPsychNeedImageProcessing
    % Yes. How many commands per chain?
    nrslots = max(length(find(mystrcmp(reqs, 'LeftView'))), length(find(mystrcmp(reqs, 'RightView'))));
    nrslots = nrslots + length(find(mystrcmp(reqs, 'AllViews')));
    
    % More than one slot per chain? Otherwise we use the default
    % single-pass chain:
    if nrslots > 1
        % More than two slots per chain?
        if nrslots > 2
            % Need full blown multistage chain:
            imagingMode = mor(imagingMode, kPsychNeedMultiPass);
        else
            % Only two slots. More efficient dual-pass chain is sufficient:
            imagingMode = mor(imagingMode, kPsychNeedDualPass);
        end
    end
end

% Final output formatting stage needed?
if ~isempty(find(mystrcmp(reqs, 'FinalFormatting')))
    imagingMode = mor(imagingMode, kPsychNeedFastBackingStore);
    imagingMode = mor(imagingMode, kPsychNeedOutputConversion);
end

% Support for fast offscreen windows (aka FBO backed offscreen windows)
% needed?
if ~isempty(find(mystrcmp(reqs, 'UseFastOffscreenWindows')))
    % Need fast offscreen windows. They are included if any non-zero imagingMode
    % is set, so we only request'em if imagingMode is still zero:
    if imagingMode == 0
        imagingMode = kPsychNeedFastOffscreenWindows;
    end
end

return;

% End of FinalizeConfiguration subroutine.

% PostConfiguration is called after the onscreen window is open: Performs
% actual pipeline setup of the hook chains:
function rc = PostConfiguration(reqs, win, clearcolor)
global ptb_outputformatter_icmAware;
global GL;

if isempty(GL)
    % Perform minimal OpenGL init, so we can call OpenGL commands and use
    % GL constants. We do not activate a full 3D rendering context:
    InitializeMatlabOpenGL([], [], 1);
end

% Identity CLUT in graphics hardware required?
needsIdentityCLUT = 0;

% 0.0 - 1.0 colorrange without color clamping required?
needsUnitUnclampedColorRange = 0;

% Number of used slots in left- and right processing chain:
leftcount = 0;
rightcount = 0;
outputcount = 0;

% Flags for horizontal/vertical flip operations:
leftUDFlip = 0;
rightUDFlip = 0;
leftLRFlip = 0;
rightLRFlip = 0;

% Stereomode?
winfo = Screen('GetWindowInfo', win);

% --- First action in pipe is a horizontal- or vertical flip, if any ---

% Any flip horizontal requested?
floc = find(mystrcmp(reqs, 'FlipHorizontal'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            if mystrcmp(reqs{row, 1}, 'LeftView') || mystrcmp(reqs{row, 1}, 'AllViews')
                % LeftView horizontal flip.
                leftLRFlip = 1;
            end

            if mystrcmp(reqs{row, 1}, 'RightView') || mystrcmp(reqs{row, 1}, 'AllViews')
                % LeftView horizontal flip.
                rightLRFlip = 1;
            end
        end
    end
end

% Any flip vertical requested?
floc = find(mystrcmp(reqs, 'FlipVertical'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            if mystrcmp(reqs{row, 1}, 'LeftView') || mystrcmp(reqs{row, 1}, 'AllViews')
                % LeftView vertical flip.
                leftUDFlip = 1;
            end

            if mystrcmp(reqs{row, 1}, 'RightView') || mystrcmp(reqs{row, 1}, 'AllViews')
                % LeftView vertical flip.
                rightUDFlip = 1;
            end
        end
    end
end

% Left channel flipping needed?
if leftLRFlip || leftUDFlip
    % Yes.
    sx = 1;
    ox = 0;
    sy = 1;
    oy = 0;

    if leftLRFlip
        sx = -1;
        ox = RectWidth(Screen('Rect', win));
    end

    if leftUDFlip
        sy = -1;
        oy = RectHeight(Screen('Rect', win));
    end

    % Enable left imaging chain:
    Screen('HookFunction', win, 'Enable', 'StereoLeftCompositingBlit');
    % Append blitter for LR/UD flip:
    Screen('HookFunction', win, 'AppendBuiltin', 'StereoLeftCompositingBlit', 'Builtin:IdentityBlit', sprintf('Offset:%i:%i:Scaling:%f:%f', ox, oy, sx, sy));
    leftcount = leftcount + 1;
end

if winfo.StereoMode > 0
    % Stereomode enabled: Need to possibly handle right channel as
    % well. In mono-mode there would be only a left channel...

    % Right channel flipping needed?
    if rightLRFlip || rightUDFlip
        % Yes.
        sx = 1;
        ox = 0;
        sy = 1;
        oy = 0;

        if rightLRFlip
            sx = -1;
            ox = RectWidth(Screen('Rect', win));
        end

        if rightUDFlip
            sy = -1;
            oy = RectHeight(Screen('Rect', win));
        end

        % Enable right imaging chain:
        Screen('HookFunction', win, 'Enable', 'StereoRightCompositingBlit');
        % Append blitter for LR/UD flip:
        Screen('HookFunction', win, 'AppendBuiltin', 'StereoRightCompositingBlit', 'Builtin:IdentityBlit', sprintf('Offset:%i:%i:Scaling:%f:%f', ox, oy, sx, sy));
        rightcount = rightcount + 1;
    end
end

% --- End of the flipping stuff ---

% --- Addition of offsets / scales etc. to input image ---
floc = find(mystrcmp(reqs, 'AddOffsetToImage'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            % Extract first parameter - This should be the offset:
            PixelOffset = reqs{row, 3};
            
            if isempty(PixelOffset) | ~isnumeric(PixelOffset)
                Screen('CloseAll');
                error('PsychImaging: Parameter for ''AddOffsetToImage'' missing or not of numeric type!');
            end
            
            % Further (optional) parameters passed?
            % 2nd parameter, if any, would be a gain value to apply before
            % applying the PixelOffset:
            PixelGain = reqs{row, 4};
            if isempty(PixelGain)
                % No such flag: Default to 1:
                PixelGain = 1;
            else
                if ~isnumeric(PixelGain)
                    Screen('CloseAll');
                    error('PsychImaging: Optional Gain-Parameter for ''AddOffsetToImage'' not of numeric type!');
                end
            end

            % 3rd parameter, if any, would be an Offset value to apply before
            % applying the gain:
            PixelPreOffset = reqs{row, 5};
            if isempty(PixelPreOffset)
                % No such flag: Default to 0:
                PixelPreOffset = 0;
            else
                if ~isnumeric(PixelPreOffset)
                    Screen('CloseAll');
                    error('PsychImaging: Optional "Offset before Gain"- PrescaleParameter for ''AddOffsetToImage'' not of numeric type!');
                end
            end

            % Load and build shader:
            shader = LoadGLSLProgramFromFiles('ScaleAndBiasShader', 1);

            % Init the shader: Assign mapping of input image and offsets, gains:
            glUseProgram(shader);
            
            glUniform1i(glGetUniformLocation(shader, 'Image'), 0);
            glUniform1f(glGetUniformLocation(shader, 'postscaleoffset'), PixelOffset);
            glUniform1f(glGetUniformLocation(shader, 'prescaleoffset'), PixelPreOffset);
            glUniform1f(glGetUniformLocation(shader, 'scalefactor'), PixelGain);
            
            glUseProgram(0);
            
            % Ok, 'gld' should contain a valid OpenGL display list for
            % geometry correction. Attach proper shader to proper chain:
            if mystrcmp(reqs{row, 1}, 'LeftView') || mystrcmp(reqs{row, 1}, 'AllViews')
                % Need to attach to left view:
                if leftcount > 0
                    % Need a bufferflip command:
                    Screen('HookFunction', win, 'AppendBuiltin', 'StereoLeftCompositingBlit', 'Builtin:FlipFBOs', '');
                end
                Screen('HookFunction', win, 'AppendShader', 'StereoLeftCompositingBlit', 'ScaleAndOffsetShader', shader);
                Screen('HookFunction', win, 'Enable', 'StereoLeftCompositingBlit');
                leftcount = leftcount + 1;
            end

            if mystrcmp(reqs{row, 1}, 'RightView') || (mystrcmp(reqs{row, 1}, 'AllViews') && winfo.StereoMode > 0)
                % Need to attach to right view:
                if rightcount > 0
                    % Need a bufferflip command:
                    Screen('HookFunction', win, 'AppendBuiltin', 'StereoRightCompositingBlit', 'Builtin:FlipFBOs', '');
                end
                Screen('HookFunction', win, 'AppendShader', 'StereoRightCompositingBlit', 'ScaleAndOffsetShader', shader);
                Screen('HookFunction', win, 'Enable', 'StereoRightCompositingBlit');
                rightcount = rightcount + 1;
            end

            if mystrcmp(reqs{row, 1}, 'FinalFormatting')
                % Need to attach to final formatting:
                if outputcount > 0
                    % Need a bufferflip command:
                    Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');
                end
                Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', 'ScaleAndOffsetShader', shader);
                Screen('HookFunction', win, 'Enable', 'FinalOutputFormattingBlit');
                outputcount = outputcount + 1;
            end
        end
    end
end
% --- End of addition of offsets / scales etc. to input image ---


% --- Geometry correction via warped blit ---
floc = find(mystrcmp(reqs, 'GeometryCorrection'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            % Extract first parameter - This should be the name of a
            % calibration file:
            calibfilename = reqs{row, 3};
            
            if isempty(calibfilename)
                Screen('CloseAll');
                error('PsychImaging: Parameter for ''GeometryCorrection'' missing!');
            end
            
            % We accept names of calibration files or calibration structs:
            if isstruct(calibfilename)
                % Warpstruct passed: Use it.
                warpstruct = calibfilename;
            else
                if ~ischar(calibfilename)
                    Screen('CloseAll');
                    error('PsychImaging: Passed an argument to ''GeometryCorrection'' which is not a valid name of a calibration file!');
                end
            
                % Filename found. Further (optional) parameters passed?
                % 2nd parameter, if any, would be a 'visualize' flag that
                % asks for plotting of some calibration info and additional
                % output to the console:
                showCalibOutput = reqs{row, 4};
                if isempty(showCalibOutput)
                    % No such flag: Default to "silence":
                    showCalibOutput = 0;
                end
                
                % Additional parameters provided? Pass 'em along. Currently
                % defined are up to additional 6 parameters 5 to 10. These
                % default to empty if not provided by user-code.
                
                % Use helper function to read the calibration file and build a
                % proper warp-function:
                warpstruct = CreateDisplayWarp(win, calibfilename, showCalibOutput, reqs{row, 5:10});
            end
            
            % Is it a display list handle?
            if ~isempty(warpstruct.gld)
                % This must be a display list handle for pure display list
                % blitting:
                gld = warpstruct.gld;
                if ~glIsList(gld)
                    % Game over:
                    Screen('CloseAll');
                    error('PsychImaging: Passed a handle to ''GeometryCorrection'' which is not a valid OpenGL display list!');
                end

                if ~isempty(warpstruct.glsl)
                    glsl = warpstruct.glsl;
                else
                    glsl = 0;
                end
                
                % Ok, 'gld' should contain a valid OpenGL display list for
                % geometry correction. Attach proper blitter to proper chain:
                if mystrcmp(reqs{row, 1}, 'LeftView') || mystrcmp(reqs{row, 1}, 'AllViews')
                    % Need to setup left view warp:
                    if leftcount > 0
                        % Need a bufferflip command:
                        Screen('HookFunction', win, 'AppendBuiltin', 'StereoLeftCompositingBlit', 'Builtin:FlipFBOs', '');
                    end
                    
                    if glsl
                        Screen('HookFunction', win, 'AppendShader', 'StereoLeftCompositingBlit', 'GeometricWarpShader', glsl, sprintf('Blitter:DisplayListBlit:Handle:%i', gld));                        
                    else
                        Screen('HookFunction', win, 'AppendBuiltin', 'StereoLeftCompositingBlit', 'Builtin:IdentityBlit', sprintf('Blitter:DisplayListBlit:Handle:%i:Bilinear', gld));
                    end
                    Screen('HookFunction', win, 'Enable', 'StereoLeftCompositingBlit');
                    leftcount = leftcount + 1;
                end

                if mystrcmp(reqs{row, 1}, 'RightView') || (mystrcmp(reqs{row, 1}, 'AllViews') && winfo.StereoMode > 0)
                    % Need to setup right view warp:
                    if rightcount > 0
                        % Need a bufferflip command:
                        Screen('HookFunction', win, 'AppendBuiltin', 'StereoRightCompositingBlit', 'Builtin:FlipFBOs', '');
                    end

                    if glsl
                        Screen('HookFunction', win, 'AppendShader', 'StereoRightCompositingBlit', 'GeometricWarpShader', glsl, sprintf('Blitter:DisplayListBlit:Handle:%i', gld));
                    else
                        Screen('HookFunction', win, 'AppendBuiltin', 'StereoRightCompositingBlit', 'Builtin:IdentityBlit', sprintf('Blitter:DisplayListBlit:Handle:%i:Bilinear', gld));
                    end
                    Screen('HookFunction', win, 'Enable', 'StereoRightCompositingBlit');
                    rightcount = rightcount + 1;
                end

                if mystrcmp(reqs{row, 1}, 'FinalFormatting')
                    % Need to setup final formatting warp:
                    if outputcount > 0
                        % Need a bufferflip command:
                        Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');
                    end

                    if glsl
                        Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', 'GeometricWarpShader', glsl, sprintf('Blitter:DisplayListBlit:Handle:%i', gld));
                    else
                        Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:IdentityBlit', sprintf('Blitter:DisplayListBlit:Handle:%i:Bilinear', gld));
                    end
                    Screen('HookFunction', win, 'Enable', 'FinalOutputFormattingBlit');
                    outputcount = outputcount + 1;
                end
            else
                % Game over:
                Screen('CloseAll');
                error('PsychImaging: Passed a handle for a not yet implemented display undistortion method!');
            end
        end
    end
end
% --- End of geometry correction via warped blit ---

% --- Interleaved line stereo wanted? ---
if ~isempty(find(mystrcmp(reqs, 'InterleavedLineStereo')))
    % Yes: Load and setup compositing shader.
    shader = LoadGLSLProgramFromFiles('InterleavedLineStereoShader', 1);

    floc = find(mystrcmp(reqs, 'InterleavedLineStereo'));
    [rows cols]= ind2sub(size(reqs), floc);
    % Extract first parameter - This should be the mapping of odd- and even
    % lines: 0 = even lines == left image, 1 = even lines == right image.
    startright = reqs{rows, 3};

    if startright~=0 && startright~=1
        Screen('CloseAll');
        error('PsychImaging: The "startright" parameter must be zero or one!');
    end
    
    % Init the shader: Assign mapping of left- and right image:
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, 'Image1'), 1-startright);
    glUniform1i(glGetUniformLocation(shader, 'Image2'), startright);

    % Add a readout offset of window height in vertical direction:
    [winwidth winheight] = Screen('WindowSize', win);
    glUniform2f(glGetUniformLocation(shader, 'Offset'), 0, winheight);
    glUseProgram(0);
    
    % Reset compositor chain: It got initialized inside Screen() with an
    % unsuitable shader for our purpose:
    Screen('HookFunction', win, 'Reset', 'StereoCompositingBlit');

    % Append our new shader and enable chain:
    Screen('HookFunction', win, 'AppendShader', 'StereoCompositingBlit', 'StereoCompositingShaderInterleavedLineStereo', shader);
    Screen('HookFunction', win, 'Enable', 'StereoCompositingBlit');
end
% --- End of interleaved line stereo setup code ---


% --- Custom color correction for display wanted? ---
%
% This *MUST* be immediately before the final output formatters for
% special display devices. If this is done in the output conversion chain
% it must be the last corrective operation before data is fed into the
% formatter plugins. If it is applied to the image processing chains for
% stereo display setups, it must be the absolutely last operation in that
% processing chains before data is fed into output conversion or into the
% stereo compositor.
%
% If we need per view correction for any stereo output mode except
% anaglyph stereo, it needs to happen at end of per view pipeline, so
% things like gamma-correction are applied to final stims, not
% intermediate results. In any other case, there will be only one physical
% output device, so correction is handled best at the end of output
% conversion.
icmshader = [];
icmstring = [];
icmformatting_downstream = 0;

floc = find(mystrcmp(reqs, 'DisplayColorCorrection'));
if ~isempty(floc)
    handlebrightside  = 0;
    handlebitspluplus = 0;
    
    % Bits+ Mono++ or Color++ mode active?
    if ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++Output'))) || ~isempty(find(mystrcmp(reqs, 'EnableBits++Mono++OutputWithOverlay'))) || ~isempty(find(mystrcmp(reqs, 'EnableBits++Color++Output')))
        if winfo.StereoMode == 0
            % Nothing to do. Full setup has been done inside OpenWindow
            % routine.
            floc = [];
            handlebitspluplus=0;
        else
            % Need to do our setup work -- The Bitsplus output formatter
            % just contains a simple neutral clamping shader. However, we
            % need to be careful where to insert our shader if the target
            % is the output conversion chain.
            handlebitspluplus=1;
        end
    end
    
    if ~isempty(find(mystrcmp(reqs, 'EnableBrightSideHDROutput')))
        handlebrightside = 1;
    end
        
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            % Extract first parameter - This should be the method of correction:
            colorcorrectionmethod = reqs{row, 3};
            
            if isempty(colorcorrectionmethod) | ~ischar(colorcorrectionmethod)
                Screen('CloseAll');
                error('PsychImaging: Name of color correction method for ''DisplayColorCorrection'' missing or not of string type!');
            end

            % Select method:
            PsychColorCorrection('ChooseColorCorrection', colorcorrectionmethod);
            
            % Load and build shader objects: icmshader is the compiled
            % color correction shader:
            [icmshader icmstring] = PsychColorCorrection('GetCompiledShaders', win, 1);

            % Output formatter with built-in ICM capabilities selected? And
            % color correction for final formatting chain insted of
            % per-viewchannel chains?
            if (ptb_outputformatter_icmAware > 0) & (mystrcmp(reqs{row, 1}, 'FinalFormatting') || mystrcmp(reqs{row, 1}, 'AllViews'))
                % Yes. These formatters can use the icm shader internally for
                % higher efficiency if wanted. We can only do that if color
                % correction shall happen in 'AllViews' or 'FinalFormatting', ie.,
                % if this is a monoscopic window or a stereo window where all views
                % display to the same physical output device and therefore the same
                % color correction can be applied to both views.
                
                % Good. We create the icmshader here according to specs,
                % but then pass it along downstream to the output formatter
                % setup code which will attach it.
                icmformatting_downstream = 1;
                
            else
                % Downstream color correction not possible due to use of
                % either a per viewchannel correction, or due to use of
                % either no output formatter at all, or not of an icm aware
                % one:
                icmformatting_downstream = 0;
                
                % Need to build full standalone shader, including main()
                % stub routine and full link and post-link:

                % shMain is the main() routine which needs to get compiled into
                % a valid shader object:
                shBody = ['uniform sampler2DRect Image; vec4 icmTransformColor(vec4 incolor); void main(void){gl_FragColor = icmTransformColor(texture2DRect(Image, gl_FragCoord.xy));}'];
                shMain = sprintf('\n#extension GL_ARB_texture_rectangle : enable \n\n%s', shBody);
                mainShader = glCreateShader(GL.FRAGMENT_SHADER);
                glShaderSource(mainShader, shMain);
                glCompileShader(mainShader);

                % Link together mainShader and icmshader into a GLSL program
                % object:
                shader = glCreateProgram;
                glAttachShader(shader, icmshader);
                glAttachShader(shader, mainShader);

                % Link the program:
                glLinkProgram(shader);

                % Init the shader: Assign mapping of input image and offsets, gains:
                glUseProgram(shader);
                glUniform1i(glGetUniformLocation(shader, 'Image'), 0);
                glUseProgram(0);
            end

            % Ok, shader is our final color correction shader, properly
            % setup. Attach it to proper chain:            
            if mystrcmp(reqs{row, 1}, 'LeftView')
                % Need to attach to left view:
                if leftcount > 0
                    % Need a bufferflip command:
                    Screen('HookFunction', win, 'AppendBuiltin', 'StereoLeftCompositingBlit', 'Builtin:FlipFBOs', '');
                end
                Screen('HookFunction', win, 'AppendShader', 'StereoLeftCompositingBlit', icmstring, shader);
                Screen('HookFunction', win, 'Enable', 'StereoLeftCompositingBlit');
                leftcount = leftcount + 1;
            end

            if mystrcmp(reqs{row, 1}, 'RightView')
                % Need to attach to right view:
                if rightcount > 0
                    % Need a bufferflip command:
                    Screen('HookFunction', win, 'AppendBuiltin', 'StereoRightCompositingBlit', 'Builtin:FlipFBOs', '');
                end
                Screen('HookFunction', win, 'AppendShader', 'StereoRightCompositingBlit', icmstring, shader);
                Screen('HookFunction', win, 'Enable', 'StereoRightCompositingBlit');
                rightcount = rightcount + 1;
            end

            if ~icmformatting_downstream
                if mystrcmp(reqs{row, 1}, 'FinalFormatting') || mystrcmp(reqs{row, 1}, 'AllViews')
                    % Need to attach to final formatting:
                    if ~handlebitspluplus & ~handlebrightside
                        % Standard case:
                        if outputcount > 0
                            % Need a bufferflip command:
                            Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');
                        end
                        Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', icmstring, shader);
                    else
                        % Special case: A BitsPlusPlus or BrightSideHDR output formatter has
                        % been attached at the end of queue already. We need
                        % to insert our new slot + some FlipFBO commands just
                        % before the last occupied slot - which is the output formatter slot.
                        % Let's simply count the number of occupied slots and
                        % then insert at that location:
                        insertPos = 0;
                        while(1)
                            if Screen('Hookfunction', win, 'Query', 'FinalOutputFormattingBlit', insertPos)~=-1
                                insertPos = insertPos + 1;
                            else
                                break;
                            end
                        end
                        % insertPos points to first slot after the end of the
                        % chain, ie., where one could append new slots. We want
                        % to insert just at the location of the last slot, so
                        % the last slot gets pushed back one element:
                        insertPos = insertPos - 1;

                        % Need to prepend a bufferflip command in front of
                        % bitsplusplus:
                        insertSlot = sprintf('InsertAt%iBuiltin', insertPos);
                        Screen('HookFunction', win, insertSlot, 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');

                        % Then need to prepend our shader in front of that
                        % FlipFBO's:
                        insertSlot = sprintf('InsertAt%iShader', insertPos);
                        Screen('HookFunction', win, insertSlot, 'FinalOutputFormattingBlit', icmstring, shader);

                        % If we're not the first, we need to prepend a
                        % FlipFBO's for ourselves:
                        if outputcount > 0
                            % Need a bufferflip command:
                            insertSlot = sprintf('InsertAt%iBuiltin', insertPos);
                            Screen('HookFunction', win, insertSlot, 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');
                        end
                    end

                    % One more slot occupied by us, so increment outputcount:
                    outputcount = outputcount + 1;

                    % And enable the chain if it ain't enabled already:
                    Screen('HookFunction', win, 'Enable', 'FinalOutputFormattingBlit');
                end

                % Perform post-link setup of color correction method after
                % shader attached to pipe:
                PsychColorCorrection('ApplyPostGLSLLinkSetup', win, reqs{row, 1});
            end
        end
    end
end

% Any output formatter to follow which is icmAware, ie., needs to have an
% icmshader as input, either a real one, or a dummy pass-through one?
if ptb_outputformatter_icmAware
    % Yes. To be created output formatter needs an icmshader.
    % Downstream attachment of (already created) icmshader?
    % If so, nothing to do, icmshader and icmstring already setup:
    if ~icmformatting_downstream
        % No. The output formatter is icm aware and needs an icmshader, but
        % none yet created because downstream correction not possible. We
        % need to create a dummy icmshader which just passes through all
        % values uncorrected - This way we make sure that the link
        % operation of the output formatter doesn't fail:
        icmshader = LoadShaderFromFile('ICMPassThroughShader.frag.txt', [], 1);
        icmstring = '';
    else
        % Nothing to do. Just perform some sanity check here to catch
        % possible future implementation bugs:
        if isempty(icmshader) || isempty(icmstring)
            error('In DisplayColorCorrection setup: Downstream formatting for icmAware output formatter requested, but icmshader and/or icmstring undefined! This is an implementation bug!!!');
        end
    end

end

% --- End of Custom color correction for display wanted ---

% --- User code wants to use unclamped, high precision 0-1 range colors? ---
if ~isempty(find(mystrcmp(reqs, 'NormalizedHighresColorRange')))
    % Use unit color range, without clamping, but in high-precision mode:
    needsUnitUnclampedColorRange = 1;
end
% --- End of setup for unclamped, high precision 0-1 range colors ---



% --- FROM HERE ON ONLY OUTPUT FORMATTERS, NOTHING ELSE!!! --- %


% --- Final output formatter for generic LUT based luminance framebuffer requested? ---
% --- OR Final output formatter for Pseudo-Gray processing requested? ---
floc = find(mystrcmp(reqs, 'EnableGenericHighPrecisionLuminanceOutput'));
if isempty(floc)
    floc = find(mystrcmp(reqs, 'EnablePseudoGrayOutput'));
end
if ~isempty(floc)
    [row col]= ind2sub(size(reqs), floc);
    
    if mystrcmp(reqs{row, 2}, 'EnablePseudoGrayOutput')
        % PseudoGray mode: We create the lut ourselves via helper function:
        lut = CreatePseudoGrayLUT;

        % For proper pseudo-gray output the gfx gamma-tables must not be
        % touched by us!
        needsIdentityCLUT = 0;
    else
        % Generic "attenuator" driver mode:

        % Attenuator-style devices need an identity clut:
        needsIdentityCLUT = 1;

        % Extract first parameter - This should be the lookup table 'lut' to use:
        lut = reqs{row, 3};
    end
    
    if isempty(lut) | ~isnumeric(lut)
        Screen('CloseAll');
        error('PsychImaging: Mandatory lookup table parameter lut for ''EnableGenericHighPrecisionLuminanceOutput'' missing or not of numeric type!');
    end

    % Load output formatting shader for GenericHighPrecisionLuminanceOutput:
    % 'icmshader' is a handle to a compiled fragment shader, provided by
    % upstream, that implements the display color correction function:
    pgshader = LoadGLSLProgramFromFiles('GenericLuminanceToRGBA8_FormattingShader', 1, icmshader);

    % Init the shader: Assign mapping texture units etc.:
    glUseProgram(pgshader);
    glUniform1i(glGetUniformLocation(pgshader, 'Image'), 0);
    glUniform1i(glGetUniformLocation(pgshader, 'LUT'),  1);
    glUniform1f(glGetUniformLocation(pgshader, 'MaxIndex'),  size(lut, 2)-1);
    glUseProgram(0);

    % Use helper routine to build a proper RGBA Lookup texture for
    % conversion of HDR luminance pixels to RGBA8 pixels:
    pglutid = PsychHelperCreateGenericLuminanceToRGBA8LUT(lut);
    
    if outputcount > 0
        % Need a bufferflip command:
        Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');
    end
    pgconfig = sprintf('TEXTURERECT2D(1)=%i', pglutid);
    pgidstring = sprintf('Generic high precision luminance output formatting shader: %s', icmstring);
    Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', pgidstring, pgshader, pgconfig);
    Screen('HookFunction', win, 'Enable', 'FinalOutputFormattingBlit');
    outputcount = outputcount + 1;
        
    % Use unit color range, without clamping, but in high-precision mode:
    needsUnitUnclampedColorRange = 1;
end
% --- End of output formatter for generic LUT based luminance framebuffer ---

% --- Final output formatter for VideoSwitcher attenuator device requested? ---
VideoSwitcherTriggerflag = 0;

floc = find(mystrcmp(reqs, 'EnableVideoSwitcherSimpleLuminanceOutput'));
if isempty(floc)
    floc = find(mystrcmp(reqs, 'EnableVideoSwitcherCalibratedLuminanceOutput'));
end
if ~isempty(floc)
    [row col]= ind2sub(size(reqs), floc);

    if mystrcmp(reqs{row, 2}, 'EnableVideoSwitcherSimpleLuminanceOutput')
        simpleVideoSwitcher = 1;
    else
        simpleVideoSwitcher = 0;
    end
    
    % Extract optional first parameter - This should be the 'btrr' ratio to use:
    btrr = reqs{row, 3};
    
    if isempty(btrr)
        % btrr empty: Get it from config file:
        btrr = PsychVideoSwitcher('GetDefaultConfig', win);
    else
        if ~isnumeric(btrr) | ~isscalar(btrr)
            Screen('CloseAll');
            error('PsychImaging: Optional "btrr" parameter for VideoSwitcher output not of numeric scalar type!');
        end
        
        if btrr < 0
            Screen('CloseAll');
            error('PsychImaging: Optional "btrr" parameter for VideoSwitcher output is negative -- Impossible!');
        end
    end
    
    if simpleVideoSwitcher
        % Extract optional 2nd parameter - This should be the 'trigger' flag:
        VideoSwitcherTriggerflag = reqs{row, 4};
    else
        % Extract optional 3rd parameter - This should be the 'trigger' flag:
        VideoSwitcherTriggerflag = reqs{row, 5};
    end
    
    if isempty(VideoSwitcherTriggerflag)
        % triggerflag empty: Default to off:
        VideoSwitcherTriggerflag = 0;
    else
        if ~isnumeric(VideoSwitcherTriggerflag) | ~isscalar(VideoSwitcherTriggerflag)
            Screen('CloseAll');
            error('PsychImaging: Optional "trigger" parameter for VideoSwitcher output not of numeric scalar type!');
        end
        
        if VideoSwitcherTriggerflag > 0
            VideoSwitcherTriggerflag = 1;
        else
            VideoSwitcherTriggerflag = 0;
        end
    end
    
    if simpleVideoSwitcher
        % Load output formatting shader for simple VideoSwitcher output:
        % 'icmshader' is a handle to a compiled fragment shader, provided by
        % upstream, that implements the display color correction function:
        pgshader = LoadGLSLProgramFromFiles('VideoSwitcherSimpleLuminanceToRB8_FormattingShader', 1, icmshader);

        % Init the shader: Assign mapping texture units etc.:
        glUseProgram(pgshader);
        glUniform1i(glGetUniformLocation(pgshader, 'Image'), 0);
        glUniform1f(glGetUniformLocation(pgshader, 'btrr'),  btrr);
        glUniform1f(glGetUniformLocation(pgshader, 'btrrPlusOne'),  btrr + 1);
        glUniform1f(glGetUniformLocation(pgshader, 'btrrFractionTerm'), ((btrr + 1) / btrr));
        glUseProgram(0);

        pgidstring = sprintf('VideoSwitcher simple high precision luminance output formatting shader: %s', icmstring);
        pgconfig = '';
    else
        % LUT calibrated VideoSwitcher setup:

        % Extract optional 2nd parameter - This should be the 'lut':
        lut = reqs{row, 4};

        if isempty(lut)
            % lut empty: Get it from config file:
            [dummy, lut] = PsychVideoSwitcher('GetDefaultConfig', win);
        else
            if ~isa(lut, 'double') | ~isvector(lut) | length(lut)~=257
                Screen('CloseAll');
                error('PsychImaging: Lookup table parameter lut for VideoSwitcher output invalid: Must be a vector of double values with 257 elements!');
            end
        end
                
        % Load output formatting shader for lut calibrated VideoSwitcher output:
        % 'icmshader' is a handle to a compiled fragment shader, provided by
        % upstream, that implements the display color correction function:
        pgshader = LoadGLSLProgramFromFiles('VideoSwitcherCalibratedLuminanceToRB8_FormattingShader', 1, icmshader);

        % Init the shader: Assign mapping texture units etc.:
        glUseProgram(pgshader);
        glUniform1i(glGetUniformLocation(pgshader, 'Image'), 0);
        glUniform1i(glGetUniformLocation(pgshader, 'LUT'), 1);
        glUniform1f(glGetUniformLocation(pgshader, 'btrr'),  btrr);

        % Disable luminance hint color by default by setting it to the
        % luminance key -1, which shouldn't ever match in a regular
        % stimulus:
        glUniform3f(glGetUniformLocation(pgshader, 'BackgroundPixel'), 0, -1, 0);
        glUseProgram(0);    

        % Convert 'lut' into lookup table texture:
        pglutid = PsychVideoSwitcher('GetLUTTexture', win, lut, btrr, pgshader);
                
        pgidstring = sprintf('VideoSwitcher calibrated high precision luminance output formatting shader: %s', icmstring);
        pgconfig = sprintf('TEXTURERECT2D(1)=%i', pglutid);
    end
        
    if outputcount > 0
        % Need a bufferflip command:
        Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');
    end
    Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', pgidstring, pgshader, pgconfig);
    Screen('HookFunction', win, 'Enable', 'FinalOutputFormattingBlit');
    outputcount = outputcount + 1;

    % VideoSwitcher devices need an identity clut:
    needsIdentityCLUT = 1;
    
    % Use unit color range, without clamping, but in high-precision mode:
    needsUnitUnclampedColorRange = 1;    
end

% Setup of trigger for VideoSwitcher device needed?
if VideoSwitcherTriggerflag > 0
    % Yes. Attach a proper slot to the chain: The slot calls back into the
    % VideoSwitcher.m M-File, with the window handle as argument.
    pgconfig = sprintf('PsychVideoSwitcher(%i);', win);
    Screen('HookFunction', win, 'AppendMFunction', 'FinalOutputFormattingBlit', 'VideoSwitcher trigger control callback.', pgconfig);    
end

% --- End of output formatters for VideoSwitcher attenuator device ---

    
% --- Final output formatter for native 10 bpc ARGB2101010 framebuffer requested? ---
if ~isempty(find(mystrcmp(reqs, 'EnableNative10BitFramebuffer')))
    % Load output formatting shader:
    pgshader = LoadGLSLProgramFromFiles('RGBMultiLUTLookupCombine_FormattingShader', 1);

    % Init the shader: Assign mapping of left- and right image:
    glUseProgram(pgshader);
    glUniform1i(glGetUniformLocation(pgshader, 'Image'), 0);
    glUniform1i(glGetUniformLocation(pgshader, 'CLUT'),  1);
    glUniform1f(glGetUniformLocation(pgshader, 'Prescale'),  1024);
    glUseProgram(0);

    % Use helper routine to build a proper RGBA Lookup texture for
    % conversion of HDR RGBA pixels to ARGB2101010 pixels:
    pglutid = PsychHelperCreateARGB2101010RemapCLUT;
    
    if outputcount > 0
        % Need a bufferflip command:
        Screen('HookFunction', win, 'AppendBuiltin', 'FinalOutputFormattingBlit', 'Builtin:FlipFBOs', '');
    end
    pgconfig = sprintf('TEXTURERECT2D(1)=%i', pglutid);
    Screen('HookFunction', win, 'AppendShader', 'FinalOutputFormattingBlit', 'Native ARGB2101010 framebuffer output formatting shader', pgshader, pgconfig);
    Screen('HookFunction', win, 'Enable', 'FinalOutputFormattingBlit');
    outputcount = outputcount + 1;

    % ATI framebuffer devices - Does not matter, as internal clut is bypassed anyway:
    needsIdentityCLUT = 0;
    
    % Use unit color range, without clamping, but in high-precision mode:
    needsUnitUnclampedColorRange = 1;
end
% --- End of output formatter for native 10 bpc ARGB2101010 framebuffer ---


% --- END OF ALL OUTPUT FORMATTERS ---

% --- This must be after setup of all output formatter shaders! ---
% Downstream icm color correction shader linked into an icmAware output
% formatter. We must perform post-link setup for it:
if ptb_outputformatter_icmAware & icmformatting_downstream
    % Perform post-link setup of color correction method after
    % shader attached to pipe. We know it is the
    % 'FinalOutputFormattingBlit' chain, as only in that case, downstream
    % formatting is performed at all.
    PsychColorCorrection('ApplyPostGLSLLinkSetup', win, 'FinalFormatting');
end

% --- GPU based mirroring of left half of onscreen window to right half requested? ---
if ~isempty(find(mystrcmp(reqs, 'MirrorDisplayToSingleSplitWindow')))
    
    % Simply set up the left finalizer chain with a glCopyPixels command
    % that copies the left half of the system backbuffer to the right half
    % of the system backbuffer. As kPsychHalfWidthWindow was requested at
    % window creation time, the reported 'WindowSize' will be already the
    % virtual size of the window, ie., half the real size...
    
    [w, h] = Screen('WindowSize', win);
    myblitstring = sprintf('glRasterPos2f(%f, %f); glCopyPixels(0, 0, %f, %f, 6144);', w, h, w, h);
    Screen('Hookfunction', win, 'AppendMFunction', 'LeftFinalizerBlitChain', 'MirrorSplitWindowToSplitWindow', myblitstring);
    Screen('HookFunction', win, 'Enable', 'LeftFinalizerBlitChain');
end
% --- End of GPU based mirroring of left half of onscreen window to right half requested? ---

% --- Restriction of processing area ROI requested? ---

% This should be at the end of setup, so we can reliably prepend the
% command to each chain to guarantee that restriction applies to all
% processing:
floc = find(mystrcmp(reqs, 'RestrictProcessing'));
if ~isempty(floc)
    % Which channel?
    for x=floc
        [rows cols]= ind2sub(size(reqs), x);
        for row=rows'
            % Extract scissor rectangle:
            scissorrect = reqs{row, 3};
            if size(scissorrect,1)~=1 || size(scissorrect,2)~=4
                error('Task "RestrictProcessing" in channel %s expects a 1-by-4 ROI rectangle to define the ROI, e.g, [left top right bottom]!', reqs{row,1});
            end

            ox = scissorrect(RectLeft);
            oy = scissorrect(RectTop);
            w  = RectWidth(scissorrect);
            h  = RectHeight(scissorrect);
            
            if mystrcmp(reqs{row, 1}, 'LeftView') || mystrcmp(reqs{row, 1}, 'AllViews')
                % Need to restrict left view processing:
                Screen('HookFunction', win, 'PrependBuiltin', 'StereoLeftCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
            end

            if mystrcmp(reqs{row, 1}, 'RightView') || mystrcmp(reqs{row, 1}, 'AllViews')
                % Need to restrict right view processing:
                Screen('HookFunction', win, 'PrependBuiltin', 'StereoRightCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
            end
            
            if (mystrcmp(reqs{row, 1}, 'AllViews') || mystrcmp(reqs{row, 1}, 'Compositor')) && winfo.StereoMode > 5
                % Needed to restrict both views processing and a
                % compositing mode is active. If both views are restricted
                % in their output area then it makes sense to restrict the
                % compositor to the same area. We also restrict the
                % compositor if that was requested.
                Screen('HookFunction', win, 'PrependBuiltin', 'StereoCompositingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
            end

            if mystrcmp(reqs{row, 1}, 'FinalFormatting')
                % Need to restrict final formatting blit processing:
                Screen('HookFunction', win, 'PrependBuiltin', 'FinalOutputFormattingBlit', 'Builtin:RestrictToScissorROI', sprintf('%i:%i:%i:%i', ox, oy, w, h));
            end
            
        end
    end
end

% --- End of Restriction of processing area ROI ---

% --- GPU based mirroring of onscreen window to secondary display head requested? ---
if ~isempty(find(mystrcmp(reqs, 'MirrorDisplayTo2ndOutputHead')))
    % Yes: This means that stereomode 10 is active and that we (ab)use it
    % for the purpose of replicating the framebuffer of the master onscreen
    % window to the slave windows framebuffer.
    
    % What we do: We use the right finalizer blit chain to copy the contents
    % of the master window's system backbuffer (which is bound during
    % execution of the right finalizer blit chain) to the colorbuffer
    % texture of the special finalizedFBO[1] - the shadow framebuffer FBO
    % of the slave window. Once we did this, the processing code of
    % stereomode 10 in Screens PsychPreFlipOperations() routine will take
    % care of the rest --> Blitting that FBO's and its texture to the system
    % backbuffer of the slave window, thereby cloning the master windows
    % framebuffer to the slave windows framebuffer:
    
    [w, h] = Screen('WindowSize', win);
    myblitstring = sprintf('glBindTexture(34037, 1); glCopyTexSubImage2D(34037, 0, 0, 0, 0, 0, %i, %i); glBindTexture(34037, 0);', w, h);
    Screen('Hookfunction', win, 'AppendMFunction', 'RightFinalizerBlitChain', 'MirrorMasterToSlaveWindow', myblitstring);
    Screen('HookFunction', win, 'Enable', 'RightFinalizerBlitChain');    
end
% --- End of GPU based mirroring of onscreen window to secondary display head requested? ---


% Do we need identity gamma tables / CLUT's loaded into the graphics card?
if needsIdentityCLUT
    % Yes. Use our generic routine which is adaptive to the quirks of
    % specific gfx-cards:
    LoadIdentityClut(win);
end

% Do we need a normalized [0.0 ; 1.0] color range mapping with unclamped
% high precision colors?
if needsUnitUnclampedColorRange
    % Set color range to 0.0 - 1.0: This makes more sense than the normal
    % 0-255 values. Try to disable color clamping. This may fail and
    % produce a PTB warning, but if it succeeds then we're better off for
    % the 2D drawing commands...
    Screen('ColorRange', win, 1, 0);

    % Set Screen background clear color, in normalized 0.0 - 1.0 range:
    if (max(clearcolor) > 1) && (all(round(clearcolor) == clearcolor))
        % Looks like someone's feeding old style 0-255 integer values as
        % clearcolor. Output a warning to tell about the expected 0.0 - 1.0
        % range of values:
        fprintf('\n\nPsychImaging-Warning: You specified a ''clearcolor'' argument for the OpenWindow command that looks \nlike an old 0-255 value instead of the wanted value in the 0.0-1.0 range. Please update your code for correct behaviour.');
    end

    % Set the background clear color via old fullscreen 'FillRect' trick,
    % followed by a flip:
    Screen('FillRect', win, clearcolor);

    % Double-flip to be on the safe side:
    Screen('Flip', win);
    Screen('Flip', win);
end

% Return reqs array, for whatever reason...
rc = reqs;
return;

% End of PostConfiguration subroutine.

function rcmatch = mystrcmp(myhaystack, myneedle)

if isempty(myhaystack) || isempty(myneedle)
    rcmatch = logical(0);
    return;
end

if ~iscell(myhaystack) && ~ischar(myhaystack)
    error('First argument to mystrcmp must be a cell-array or a character array (string)!');
end

if iscell(myhaystack)
    % Cell array of strings: Check each element, return result matrix:
    rcmatch=logical(zeros(size(myhaystack)));
    rows = size(myhaystack, 1);
    cols = size(myhaystack, 2);
    for r=1:rows
        for c=1:cols
            if iscellstr(myhaystack(r,c))
                rcmatch(r,c) = logical(strcmpi(char(myhaystack(r,c)), myneedle));
            else
                rcmatch(r,c) = logical(0);
            end
        end
    end
else
    % Single character string: Do single check and return result:
    rcmatch=logical(strcmpi(myhaystack, myneedle));
end

return;

% Helper: Search chain 'hookname' in window 'win' for scissor restriction
% slots and remove all of them:
function DoRemoveScissorRestriction(win, hookname)
    while 1
        slot = Screen('HookFunction', win, 'Query', hookname, 'Builtin:RestrictToScissorROI');
        if slot~=-1
            Screen('Hookfunction', win, 'Remove', hookname, slot);
        else
            break;
        end
    end
return;
