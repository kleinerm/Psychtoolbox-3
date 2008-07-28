% Psychtoolbox:PsychGLImageProcessing  -- OpenGL image processing functions.
%
% This subfolder contains subroutines, tests and demos on how to use
% the PTB built-in image processing pipeline.
%
% The built in pipeline provides the infrastructure to define image processing
% or stimulus image postprocessing operations on stimuli drawn by PTB's Screen
% 2D drawing commands or MOGL 3D OpenGL drawing commands.
%
% The pipeline provides the data flow and API to define type and parameterization
% of the image processing operations. Operations are usually implemented via
% OpenGL GLSL shader plugins, vertex & fragment shader programs written in the
% high level OpenGL Shading langugage GLSL and executed directly inside the
% graphics processing unit GPU. While this is the most efficient way, operations
% may also be defined in standard OpenGL code, Matlab M-Functions or C callable
% functions.
%
% Typical applications, realized via plugins are:
%
% * Stereo display algorithms.
% * High dynamic range display output drivers.
% * Standard operations like 2D convolution, noise, linear superposition.
% * Geometric undistortion of displays.
% * Per pixel output corrections like gamma, color, gain, display PDF.
%
% This part of PTB is under heavy construction. This intro will get extended
% soon with more useful infos...
%
%
% TODO: Write useful docu.
%
% Files:
%
% Add2DConvolutionToGLOperator.m  -- Create and add a shader for 2D image convolution to a GLOperator.
% Add2DSeparableConvolutionToGLOperator.m  -- Create and add a shader for 2D separable image convolution to a GLOperator.
% AddImageUndistortionToGLOperator -- Add geometric image correction to a GLOperator.
% AddToGLOperator.m               -- Add a shader with options to a GLOperator.
% BitsPlusPlus.m                  -- Setup function for imaging pipelines built-in Bits++ support.
% CountSlotsInGLOperator.m        -- Count number of processing slots in a given GLOperator.
% CreateDisplayWarp.m             -- Internal helper function for setup of geometric display undistortion.
% CreateGLOperator.m              -- Create a new GLOperator as container for imaging operations.
% CreateProceduralGabor.m         -- Create a procedural texture for fast drawing of Gabor patches.
% CreateProceduralSineGrating.m   -- Create a procedural texture for fast drawing of sine grating patches.
% CreatePseudoGrayLUT             -- Create a lookup table for pseudogray conversion -- Internal helper function.
% DisplayUndistortionBezier.m     -- Interactive geometric display calibration for simple needs.
% DisplayUndistortionBVL.m        -- Interactive geometric display calibration. Recommended!
% HookProcessingChains.m          -- Documentation about hook chains and their options.
% ImagingStereoDemo.m             -- Counterpart to StereoDemo.m, but using imaging pipeline
%                                    for increased fidelity, flexibility, ease of use.
%
% PsychImaging.m                  -- Generic setup routine for the imaging pipeline. Allows to setup
%                                    and initialize the pipeline for many common tasks.
% PsychVideoSwitcher              -- Setup routine for the Xiangru Li et al. "VideoSwitcher" video attenuator device.
%
% SetAnaglyphStereoParameters.m   -- Function for runtime tuning of Anaglyph stereo parameters,
%                                    see ImagingStereoDemo for example of use.
%
% Constants for imagingmode flag of Screen('OpenWindow', ...., imagingmode);
% One can 'or' them together, e.g., imagingmode = mor(kPsychNeed16BPCFixed, kPsychNeedFastBackingStore);
%
% kPsychNeed16BPCFixed.m          -- Request 16 bit per color component, fixed point framebuffer.
% kPsychNeed16BPCFloat.m          -- Request 16 bit per color component, floating point framebuffer.
% kPsychNeed32BPCFloat.m          -- Request 32 bit per color component, floating point framebuffer.
% kPsychNeedDualPass.m            -- Indicate that some of the used image processing plugins will need
%                                    at two render passes for processing.
% kPsychNeedFastBackingStore.m    -- Enable minimal imaging pipeline. This flag is implied when using any
%                                    of the other flags.
% kPsychNeedFastOffscreenWindows.m - Only enable support for fast Offscreen windows, nothing else.
% kPsychNeedHalfWidthWindow       -- Tell imaging pipe to create internal buffers half the real window width. Internal flag, not useful for end-user code.
% kPsychNeedHalfHeightWindow      -- Tell imaging pipe to create internal buffers half the real window height. Internal flag, not useful for end-user code.
% kPsychNeedImageProcessing.m     -- Request explicit support for image processing.
% kPsychNeedMultiPass.m           -- Indicate that some of the used plugins will need more than two passes.
% kPsychNeedOutputConversion.m    -- Indicate that display output is going to some special output device that
%                                 -- needs special output formatting, e.g., Bits++ or Brightside HDR.

