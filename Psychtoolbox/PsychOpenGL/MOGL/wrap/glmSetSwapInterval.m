function glmSetSwapInterval( nframes )

% glmSetSwapInterval  Set number of video frames between buffer flushes
% 
% usage:  glmSetSwapInterval( nframes )
% 
% Note:  The AGL documentation indicates that 'nframes' will control the
% number of vertical refreshes between buffer flushes.  In practice, it seems
% that nframes=0 disables vertical refresh syncing, and nframes=1 or higher
% just enables it.

% 14-Dec-2005 -- created (RFM)

% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

moglcore('glmSetSwapInterval',nframes);

return
