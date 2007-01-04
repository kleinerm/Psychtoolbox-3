function rval = kPsychNeed16BPCFixed
% rval = kPsychNeed16BPCFixed
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request support for internal
% framebuffers that store color information with 16 bits per color
% component in fixed point format.
%
% If requested and supported by the graphics hardware, Psychtoolbox will
% configure itself to store and process all color information with more
% than 8 bits per color component (ideally with 16 bit per color component)
% precision.
%
% FIXME: More info...
%
% It is part of the Psychtoolbox imaging pipeline and only works on modern
% graphics hardware, e.g., ATI Radeon 9600 and later, NVidia GeforceFX 5000
% and later. Check the www.psychtoolbox.org Wiki for graphics hardware
% recommendations and for a description of PTB's image processing
% capabilities.
rval = 128;
return
