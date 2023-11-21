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
%   AnalyzeTiming                   - Analyze timing logs from FlipTimingWithRTBoxPhotoDiodeTest.
%   AsyncFlipTest                   - Test robustness and performance of Screen('AsyncFlipBegin') et al.
%   BatchAnalyzeTiming              - Batch version of AnalyzeTiming.
%   BeampositionTest                - Test GPU scanout position ("beamposition") queries.
%   CIEConeFundamentalsTest         - Test/demonstrate routines for producing cone fundamentals according to CIE 170-1:2006
%   CIEConeFundamentalsFieldSizeTest - Test behavior of underying formulae as field size exceeds the 10-deg limit of the standard.
%   CIEXYZPhysTest                  - Test properties of CIE physiological XYZ color matching function.
%   CLUTMappingBugTest              - Test proper function of PsychImaging 'EnableCLUTMapping' task.
%   Color3DLUTTest                  - Test PsychColorCorrection() method for 3D-CLUT color correction.
%   ConvolutionKernelTest           - Test routine for correctness, accuracy and speed of PTB imaging convolution shaders.
%   DatapixxGPUDitherpatternTest    - Low level diagnostic of GPU dithering bugs via Datapixx et al.
%   DeinterlacerTest                - Simple correctness test for GLSL video image deinterlacer. INCOMPLETE.
%   DrawingIntoTexturesTest         - Tests if using a texture as an offscreen window, i.e., for drawing, works.
%   DrawTextFontSwitchSpeedTest - Test speed of text drawing when switching between different font type/style/size settings.
%   DriftTexturePrecisionTest       - Test subpixel accuracy of texture interpolators: What is the smallest
%                                     fraction of a pixel that one can scroll, using built-in bilinear interpolation?
%   eGalaxTrace-*.evemu             - Linux evdev traces with recorded single/multi-touch input from an eGalax touchscreen.
%   FitCumNormYNTest                - Fit a cumulative normal to yes-no data.
%   FlipTestConfigurations.zip      - Input configuration files for FlipTimingWithRTBoxPhotoDiodeTest.m
%   FlipTimingWithRTBoxPhotoDiodeTest - Benchmark of visual stimulus onset timing and timestamping. See ECVP 2010 poster in PsychDocumentation/
%   CopyWindowTest                  - Test CopyWindow functionality.
%   DaqTest                         - Test PsychHID and routines to control the  USB-1208FS digital acquistion device.
%   DrawingStuffTest                - FrameRect, DrawLine, FillPoly, FramePoly.
%   FillPolyTest                    - Test drawing concave polygons.
%   FitConeFundamentalsTest         - Test/explore fitting CIE cone fundamentals with absorbance obtained from nomograms.
%   FitWeibullTAFCTest              - Fit a Weibull to 2AFC data.
%   FitWeibullYNTest                - Fit a Weibull to yes-no data.
%   FloatTexturePrecisionTest       - Test effective precision of floating point 16bpc textures.
%   FrameSequentialStereoTest       - Test routine for timing and stimulus onset on quad-buffered frame-sequential stereo hardware.
%   GetCharTest                     - Tests of GetChar.
%   GetSecsTest                     - Timing test of clock used by Psychtoolbox, e.g., GetSecs, WaitSecs, Screen...
%   GraphicsDisplaySyncAcrossDualHeadsTest - Test synchronization of refresh cycles of different display heads.
%   GraphicsDisplaySyncAcrossDualHeadsTestLinux - Linux version of the test.
%   HDRTest                         - Perform some basic correctness tests and evaluation for HDR display operation, using a Colorimeter.
%   HIDIntervalTest                 - Sample HID keyboard and mouse, plot distribution of detected event times.
%   HighColorPrecisionDrawingTest   - Test drawing precision of a variety of Screen() functions, esp. wrt. high precision framebuffers.
%   HighPrecisionLuminanceOutputDriversImagingPipelineTest - Test precision of a variety of high precision luminance device output drivers.
%   JavaClockTest                   - Timing test of clock used by Java functions (e.g. GetChar)
%   KeyboardLatencyTest             - Get a feeling for keyboard and mouse latency via some sound-based measurement procedure.
%   LabLuvTest                      - Test routines that convert to CIELAB and CIELUV.
%   LoadGenerator                   - Create cpu load by spinning in an infinite loop. Used in conjunction with FlipTimingWithRTBoxPhotoDiodeTest.
%   LosslessMovieWritingTest        - Test lossless encoding and decoding of video in movie files.
%   MakeTextureTimingTest           - Time texture creation -> upload -> destruction for given texture by MakeTexture et al.
%   MelanopsinFundamentalTest       - Test the PTB routines generate a good melanopsin fundamental.
%   MonoImageToSRGBTest             - Test/demo for routine PsychColorimetric/MonoImageToSRGB.
%   MultiWindowLockStepTest         - Exercise asynchronous flip scheduling and timestamping on multiple onscreen windows in parallel.
%   MultiWindowVulkanTest           - Test multi-window / multi-display exclusive operation under Vulkan.
%   OSAUCSTest                      - Test OSA UCS <-> XYZ conversion routines.
%   OSXCompositorIdiocyTest         - Test for potential OSX compositor brokeness.
%   OMLBasicTest                    - Very basic correctness test for OpenML flip timestamping.
%   OSSchedulingAccuracyTest        - Test timing accuracy of operating system scheduler for timed waits.
%   PBTAndIsetbioColorimetryTest    - Compare PTB and VSET colorimetric calculations.
%   PosterBatchAnalyzeTimestamps    - Batch analysis of timestamp logs generated by FlipTimingWithRTBoxPhotoDiodeTest for ECVP 2010 poster.
%   PupilDiameterTest               - Test functions that compute pupil diameter from luminance.
%   PutImageTest                    - Test Screen('PutImage') when used with 'NormalizedHighresColorRange'.
%   PsychPortAudioDataPixxTimingTest - Test PsychPortAudio's timing with a DataPixx device and a audio line cable.
%   PsychPortAudioTimingTest        - Testsignal generator for test of PsychPortAudios timing with external measurement equipment.
%   QuestTest                       - Some Quest simulations, more elaborate than QuestDemo.
%   ResolutionTest                  - Use Screen Resolutions to print table of display resolutions.
%   RodFundamentalTest              - Test the PTB routines generate a good rod fundamental.
%   StructsFileTest                 - Test routines for reading and writing struct arrays to text files.
%   SyncedCLUTUpdateTest            - Visual test of clut write synching to vertical retrace.
%   TextBoundsTest                  - Test Screen('TestBounds')
%   TextBugTest                     - Look for interference between
%   TextInitBugTest                 - Test for failure of 'DrawText' default font.
%   TextInOffscreenWindowTest       - Compare text rendered into onscreen and offscreen windows.
%   TextOffByOneBugTest             - Tests for fix off an off-by-one bug in Screen('Drawtext').
%   TextToStuffColorMismatchTest    - Test if text is drawn in the color it was requested to be drawn.
%   TextureChannelsTest             - Test assignment of matrix layers to RGBA texture channels.
%   TextureSharingTest              - Test OpenGL context resource sharing.
%   TrolandTest                     - Test colorimetric conversions.
%   VBLSyncTest                     - Tests visual stimulus onset timing and timestamping.
%   VREyetrackingTest               - Test eye gaze tracking in VR/AR HMDs and other XR devices.
%   VRRTest                         - Test support of your setup for Variable refresh rate mode.
%   WavelengthSamplingTest          - Test conversion between representations of wavelength sampling information.
