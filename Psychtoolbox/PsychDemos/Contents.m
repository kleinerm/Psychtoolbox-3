% Psychtoolbox:PsychDemos
%
% help Psychtoolbox     % For an overview, triple-click me & hit enter.
% help PsychDemos       % For demos, triple-click me & hit enter.
%
%
%    PsychExampleExperiments - Subfolder with real experiment scripts for
%                              different tasks, contributed by users.
%
%    AlphaImageDemo          - Simple demo of OpenGL alpha blending.
%    AlphaRotateDemo         - A variant of AlphaImageDemoOSX, demonstrates rotated drawing.
%    ArcDemo                 - Demonstrate the 'FrameArc', 'FillArc' functions.
%    ARToolkitDemo           - Use ARToolkit to track and visualize 3D objects in live-video.
%    AudioTunnel3DDemo       - Very sketchy demo on how to use OpenAL for 3D spatialized sound.
%    AudioTunnel3DDemo2      - Another sketchy demo on how to use OpenAL for 3D spatialized sound.
%    BasicAMAndMixScheduleDemo - Demonstrates basic use of sound schedules, volume controls, amplitude modulation and audio mixing of multiple voices.
%    BasicSoundChannelHoppingDemo - Demonstrates how to access individual channels of a multi-channel soundcard.
%    BasicSoundInputDemo     - Demonstrate basic usage of PsychPortAudio() for sound capture.
%    BasicSoundOutputDemo    - Demonstrate basic usage of PsychPortAudio() for sound playback.
%    BasicSoundPhaseShiftDemo - Demonstrate playback a live phase-shifted sine tone.
%    BasicSoundScheduleDemo  - Demonstrate basic usage of sound schedules and buffers with PsychPortAudio().
%    BlurredMipmapDemo       - Apply blurring via OpenGL mip-mapping to live video or movies.
%    BlurredVideoCaptureDemo - Demonstrate application of GPU accelerated image filters to live video.
%    BubbleDemo              - Like GazeContingentDemoOSX, but with multiple apertures.
%    CalDemo                 - Demonstrate use of calibration structure and associated routines.
%    ClutAnimDemo            - Demonstrate lookup table animation.
%    ContrastModulatedNoiseTheClumsyStyleDemo - Demonstrate creation of contrast modulated noise.
%                                               Clumsy and inefficient, but works on old hardware.
%
%    ContrastModulatedNoiseTheElegantStyleDemo - Demonstrate creation of contrast modulated noise.
%                                                Elegant and efficient, but needs recent hardware.
%
%    DatarecordingFromISCANDemo - Template for asynchronous data collection and timestamping of gaze samples from ISCAN eye tracker.
%    DatarecordingFromSerialPortDemo - Template for asynchronous data collection and timestamping from serial port.
%    DelayedSoundFeedbackDemo - Demonstrates a audio feedback loop via PsychPortAudio() with exactly controlled latency.
%    DKLDemo                 - Demonstrate DKL color space.
%    DotDemo                 - Animate dots with Screen('DrawDots).
%    DotDemoStencil          - Dot motion demo demonstrating use of stencil buffers.
%    DotRotDemo              - Dot rotating motion demo.
%    DrawFormattedTextDemo   - Demonstrate formatted text drawing via DrawFormattedText()
%    DrawFormattedText2Demo  - Demonstrate formatted text drawing with more options via DrawFormattedText2()
%    DrawHighQualityUnicodeTextDemo - Demonstrate drawing of high quality, anti-aliased and Unicode text.
%    DrawManuallyAntiAliasedTextDemo - Manually anti-alias text for special purpose applications.
%    DrawMirroredTextDemo    - Shows how to draw horizontally mirrored text.
%    DrawSomeTextDemo        - Very simple example of using 'DrawText'.
%    DriftDemo               - Animation of grating using textures.
%    DriftDemo2              - More efficient animation of gratings using one texture.
%    DriftDemo3              - Even more efficient animation of gratings under some constraints.
%    DriftDemo4              - Very efficient animation of gratings on graphics hardware with shader support.
%    DriftDemo5              - Animation of dual-gratings via use of alpha blending and color masking.
%    DriftDemo6              - Very efficient animation of dual-gratings on graphics hardware with shader support.
%    DriftWaitDemo           - Show how to optimally time animations.
%    ECVP2013                - Demos for the ECVP2013 beginner tutorial.
%    ErrorCatchDemo          - How to handle errors gracefully (try/catch).
%    ExpandingRingsDemo      - Shows how GLSL procedural textures can be used to
%                              create some expanding rings stimulus.
%    FastFilteredNoiseDemo   - Fast shader based noise filtering.
%    FastMaskedNoiseDemo     - Like FastNoiseDemo, but the noise patch is
%    FastNoiseDemo           - Demo and benchmark on how to quickly draw noise stimuli.
%                              drawn through an aperture to demonstrate fast masking.
%    FitGammaDemo            - Demonstrate gamma fitting routine FitGamma.
%    FontDemo                - How to check for font availability and set font.
%    GarboriumDemo           - Demonstrate superimposed drawing of many Gabors by use of alpha-
%                              blending, 'DrawTextures' batch drawing and PTB imaging pipeline.
%    GazeContingentDemo      - Implementation of a simple gaze contingent display
%    GPGPUDemos              - Demos for use of GPGPU computing with Psychtoolbox.
%    GratingDemo             - Basic display of a grating on the screen.
%    HDRMinimalisticOpenGLDemo - Basic demo on how to render OpenGL 3D stimuli on a HDR display.
%    HDRViewer               - A simple image viewer for displaying HDR images on a HDR display.
%    ImageUndistortionDemo   - Show how to do gpu accelerated geometric distortions on images.
%    ImageWarpingDemo        - Show how to do gpu accelerated warping of image regions.
%    ImagingStereoMoviePlayer - Minimalistic movie player for stereo movies, also HDR and VR capable.
%    IsomerizationsInDishDemo - Compute photoreceptor isomerizations for retina in a dish.
%    IsomerizationsInEyeDemo  - Compute photoreceptor isomerizations for human eye.
%    KbDemo                  - Demonstrate KbCheck, KbName, KbWait, the polling keyboard input functions.
%    KbQueueDemo             - Record keyboard press/release, mouse/joystick button press/release in the background.
%    Kinect3DDemo            - Capture data from a Kinect box and view it as a textured 3D point-cloud or mesh.
%    KinectDemo              - Capture and display video and depths data from a Kinect box.
%    LinesDemo               - Draw many lines efficiently, similar to DotDemo.
%    LineStippleDemo         - Draw dotted and dashed lines.
%    MandelbrotDemo          - Visualize the Mandelbrot fractal set by use
%                              of a GLSL procedural texture.
%    MinExpEntStairDemo      - Demo if use of a  minimum expected entropy staircase procedure.
%    MouseMotionRecordingDemo - Record mouse motion via KbQueues.
%    MouseTraceDemo          - Track mouse around screen and draw contour.
%    MouseTraceDemo2         - More efficient tracking of mouse around screen drawing contour.
%    MouseTraceDemo3         - Like MouseTraceDemo2, but for multiple mice.
%    MovieDemos              - Subfolder: Demos on how to use movie playback.
%    MovingLineDemo          - Shows some horizontally moving lines to demonstrate flat panel display artifacts.
%    MultiTouchDemo          - Shows how to get (multi-)touch input from touchscreens and touchpads.
%    MultiTouchMinimalDemo   - Shows a more minimal example of how to get (multi-)touch input from touchscreens and touchpads.
%    MultiTouchPinchDemo     - Shows how to detect and handle a two-finger pinch gesture on a touchscreen.
%    NomogramDemo            - Compute photoreceptor nomograms.
%    OldNewRecogExp          - A full old-new recognition experiment.
%    OpenEXRImages           - Subfolder: OpenEXR HDR sample image files.
%    OpenGL4MatlabDemos      - Subfolder: Demos on how to use MOGL OpenGL functions.
%    PanelFitterDemo         - Demonstrate use of the builtin panel fitter.
%    ProceduralColorGratingDemo - Demo for fast drawing of color gratings.
%    ProceduralGaborDemo     - Demo for fast drawing of Gabors via procedural textures.
%    ProceduralGarboriumDemo - Same as GarboriumDemo, just with procedural gabor drawing.
%    ProceduralNoiseDemo     - Demo for fast drawing of noise patches via procedural texturing.
%    ProceduralSmoothedApertureSineGratingDemo - Demo for fast drawing of smoothed aperture sine grating.
%    ProceduralSmoothedDiscsDemo - Demo for fast drawing of smoothed aperture discs.
%    ProceduralSmoothedDiscMaskDemo - Demo for fast drawing of smoothed aperture disc used as a mask.
%    ProceduralSquareWaveDemo - Demo for fast drawing of square wave grating.
%    PsychExampleExperiments - Samples of real experiments, often contributed by users.
%    PsychRTBoxDemo          - Demonstrates basic use of the RTBox reaction time button response box.
%    PsychTutorials          - Tutorials on how to achieve certain effects.
%    RaspberryPiGPIODemo     - Show basic use of GPIO's on a RaspberryPi running Raspian GNU/Linux.
%    ReceivingTriggerFromSerialPortDemo - Template for asynchronous trigger collection and timestamping from serial port.
%    RenderDemo              - Render CIE xyY as a color patch.
%    SadowskiDemo            - Shows the "Sadowski" color afterimage effect.
%    SimpleHDRDemo           - Minimal demo for using a HDR display, showing some basic stimuli.
%    SimpleHDRLinuxStereoDemo - Like SimpleHDRDemo, but with Linux/X11 stereo hack for stereoscopic HDR stimulation.
%    SimpleImageMixingDemo   - Shows how to mix two images via alpha-blending, using an alpha-mask which itself is morphed between two masks.
%                              See also ImageMixingTutorial for a more complex demonstration of the technique.
%    SimpleSoundScheduleDemo - Simple demo for basic use of sound schedules with PsychPortAudio.
%    SimpleVoiceTriggerDemo  - Demo of a simple voice trigger with PsychPortAudio.
%    SpriteDemo              - Animates an image that follows the mouse.
%    StereoDemo              - Drive stereo-displays e.g., CrystalEyes.
%    StereoViewer            - A simple viewer for stereo image pairs.
%    TurnTableDemo           - Nurture your inner DJ! Some demo of advanced PsychPortAudio use and PowerMate use.
%    ValetonVanNorrenDemo    - Exercise our code that implements the V-VN (1983) cone adaptation model.
%    VideoCaptureDemo        - Simple demo for video capture functions.
%    VideoCaptureToMatlabDemo - Demo of returning live captured video to Octave or Matlab as image matrices.
%    VideoDelayLoopMiniDemo  - Delayed visual feedback with controllable delay.
%    VideoDVCamCaptureDemo   - Demonstrate video capture and recording from DV consumer cameras.
%    VideoIPWebcamCaptureDemo - Demo video capture from an Android device via the Android IPWebcam app.
%    VideoMultiCameraCaptureDemo - Demo of time-synchronized high-perf capture and recording from multiple firewire cameras on Linux and OSX.
%    VideoOfflineCaptureDemo - Capture video into internal memory, then return it after stop of capture.
%    VideoRecordingDemo      - Record captured video to movie files, possibly with audio.
%    VideoTextureExtractionDemo - Use video capture + some computer based tracking to extract textures of objects in video.
%    VignettingCorrectionDemo - Demonstrate how to do display devignetting aka per-pixel gain correction.
%    VRHMDDemo                - Demonstrate how to use a Virtual reality head mounted display in the most basic way.
%
