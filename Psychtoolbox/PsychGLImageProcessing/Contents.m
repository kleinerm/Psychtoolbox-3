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
% BitsPlusPlus.m                  -- Setup function for imaging pipelines built-in Bits++ support.
% ImagingStereoDemo.m             -- Counterpart to StereoDemo.m, but using imaging pipeline
%                                    for increased fidelity, flexibility, ease of use.
%
% SetAnaglyphStereoParameters.m   -- Function for runtime tuning of Anaglyph stereo parameters,
%                                    see ImagingStereoDemo for example of use.

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
% kPsychNeedImageProcessing.m     -- Request explicit support for image processing.
% kPsychNeedMultiPass.m           -- Indicate that some of the used plugins will need more than two passes.
% kPsychNeedOutputConversion.m    -- Indicate that display output is going to some special output device that
%                                 -- needs special output formatting, e.g., Bits++ or Brightside HDR.

