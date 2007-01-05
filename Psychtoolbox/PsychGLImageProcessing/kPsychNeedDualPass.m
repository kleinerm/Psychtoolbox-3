function rval = kPsychNeedDualPass
% rval = kPsychNeedDualPass
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request support for dual-pass
% processing in the builtin imaging pipeline.
%
% FIXME: More info...
%
% It is part of the Psychtoolbox imaging pipeline and only works on modern
% graphics hardware, e.g., ATI Radeon 9600 and later, NVidia GeforceFX 5000
% and later. Check the www.psychtoolbox.org Wiki for graphics hardware
% recommendations and for a description of PTB's image processing
% capabilities.
rval = 256;
return
