function rval = kPsychNeedFastBackingStore
% rval = kPsychNeedFastBackingStore
%
% Return a flag that you can pass to the 'imagingmode' parameter of
% Screen('OpenWindow') in order to request use of fast image buffers.
%
% If requested, Psychtoolbox will use an especially fast buffering scheme
% to implement Offscreen windows and to implement the dontclear=1 mode of
% Screen('Flip'). This can significantly speed up drawing if you use
% Screen('Flip') with dontclear=1 or if you make heavy use of Offscreen
% windows.
%
% It is part of the Psychtoolbox imaging pipeline and only works on modern
% graphics hardware, e.g., ATI Radeon 9600 and later, NVidia GeforceFX 5000
% and later. Check the www.psychtoolbox.org Wiki for graphics hardware
% recommendations and for a description of PTB's image processing
% capabilities. If you need to use older hardware, Offscreen windows will
% still work, but at a significantly lower speed.
rval = 1;
return
