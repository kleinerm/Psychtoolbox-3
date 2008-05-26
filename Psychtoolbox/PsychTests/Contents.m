% Psychtoolbox/PsychTests/Contents.m
%
% help Psychtoolbox % For an overview, triple-click me & hit enter.
% help PsychDemos % For demos, triple-click me & hit enter.
%
%   AlphaAdditionTest               - Combine planes by OpenGL alpha addition and verify the result.
%   AlphaBlendingTest               - Multiple tests of OpenGL alpha blending. 
%   AlphaBlendSettingTest           - Set and readback alpha blending settings by screen; verify match. 
%   AlphaMultiplicationTest         - Test alpha multiplication by 0 and 1 for perfect precision.
%   AlphaMultiplicationAccuracyTest - Test precision of alpha multiplication for values between 0 and 1.
%   ConvolutionKernelTest           - Test routine for correctness, accuracy and speed of PTB imaging convolution shaders.
%   DeinterlacerTest                - Simple correctness test for GLSL video image deinterlacer. INCOMPLETE.
%   DrawingIntoTexturesTest         - Tests if using a texture as an offscreen window, i.e., for drawing, works.
%   DriftTexturePrecisionTest       - Test subpixel accuracy of texture interpolators: What is the smallest
%                                     fraction of a pixel that one can scroll, using built-in bilinear interpolation?
%   FitCumNormYNTest                - Fit a cumulative normal to yes-no data.
%   CopyWindowTest                  - Test CopyWindow functionality.
%   DaqTest                         - Test PsychHID and routines to control the  USB-1208FS digital acquistion device.
%   DrawingStuffTest                - FrameRect, DrawLine, FillPoly, FramePoly.
%   EventAvailTest                  - Test EventAvail
%   FillPolyTest                    - Test drawing concave polygons.
%   FitWeibullTAFCTest              - Fit a Weibull to 2AFC data.
%   FitWeibullYNTest                - Fit a Weibull to yes-no data.
%   FlipTest                        - Test frame synchroniziation.
%   FloatTexturePrecisionTest       - Test effective precision of floating point 16bpc textures.
%   FrameSequentialStereoTest       - Test routine for timing and stimulus onset on quad-buffered frame-sequential stereo hardware.
%   GetCharTest                     - Tests of GetChar.
%   GetSecsTest                     - Timing test of clock used by Psychtoolbox, e.g., GetSecs, WaitSecs, Screen...
%   GraphicsDisplaySyncAcrossDualHeadsTest - Test synchronization of refresh cycles of different display heads.
%   HighColorPrecisionDrawingTest   - Test drawing precision of a variety of Screen() functions, esp. wrt. high precision framebuffers.
%   HighPrecisionLuminanceOutputDriversImagingPipelineTest - Test precision of a variety of high precision luminance device output drivers.
%   JavaClockTest                   - Timing test of clock used by Java functions (e.g. GetChar)
%   LabLuvTest                      - Test routines that convert to CIELAB and CIELUV.
%   MakeTextureTimingTest           - Time memory allocation by MakeTexture
%   MatlabTimingTest                - Test for MATLAB timing glitch caused by sigsetjmp().
%   MexTimingLoopTest               - Test for MATLAB timing glitch without return to MATLAB.
%   OSAUCSTest                      - Test OSA UCS <-> XYZ conversion routines.
%   PsychHIDTest                    - PsychHID MEX file for HID-compliant USB devices.
%   PupilDiameterTest               - Test functions that compute pupil diameter from luminance.
%   PsychPortAudioTimingTest        - Test signal generator for test of PsychPortAudios timing with external measurement equipment.
%   QuestTest                       - Some Quest simulations, more elaborate than QuestDemo.
%   ResolutionTest                  - Use Screen Resolutions to print table of display resolutions.
%   ScreenTest                      - Thorough test of hardware/software performance.
%   SimpleTimingTest                - 
%   StandaloneTimingTest            - Test for timing glitch outside of MATLAB process. 
%   StructsFileTest                 - Test routines for reading and writing struct arrays to text files.
%   SyncedCLUTUpdateTest            - Visual test of clut write synching to vertical retrace.
%   TextBoundsTest                  - Test Screen('TestBounds')
%   TextBugTest                     - Look for interference between
%   TextFontTest                    - Test setting the text font.
%   TextInitBugTest                 - Test for failure of 'DrawText' default font.
%   TextInOffscreenWindowTest       - Compare text rendered into onscreen and offscreen windows. 
%   TextureChannelsTest             - Test assignment of matrix layers to RGBA texture channels
%   TextureTest                     - Exercise Screen('DrawTexture').
%   TrolandTest                     - Colorimetric conversions.
%   VBLSyncTest                     - Tests syncing of PTB-OSX to the vertical retrace.
%   WavelengthSamplingTest          - Test conversion between representations of wavelength sampling information.
