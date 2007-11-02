function rval = kPsychNeed32BPCFloat
% rval = kPsychNeed32BPCFloat
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request support for internal
% framebuffers that store color information with 32 bits per color
% component in floating point format.
%
% If requested and supported by the graphics hardware, Psychtoolbox will
% configure itself to store and process all color information as 32 bit
% floating point numbers.
% FIXME: More info...
%
% This is part of the Psychtoolbox imaging pipeline and only works on modern
% graphics hardware, e.g., ATI Radeon X1000 and later, NVidia Geforce-6000
% and later. Check the www.psychtoolbox.org Wiki for graphics hardware
% recommendations and for a description of PTB's image processing
% capabilities.
rval = 32;
return
