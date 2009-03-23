function rval = kPsychNeedDualWindowOutput
% rval = kPsychNeedDualWindowOutput
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request support for output of the same
% visual stimulus to two different onscreen windows, but with separate
% post-processing and output formatting for each window in the builtin
% imaging pipeline.
%
% FIXME: More info...
%
% It is part of the Psychtoolbox imaging pipeline and only works on modern
% graphics hardware, e.g., ATI Radeon 9600 and later, NVidia GeforceFX 5000
% and later. Check the www.psychtoolbox.org Wiki for graphics hardware
% recommendations and for a description of PTB's image processing
% capabilities.
rval = 16384;

return
