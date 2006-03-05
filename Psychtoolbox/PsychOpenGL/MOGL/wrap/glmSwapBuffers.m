function [ t, w ] = glmSwapBuffers( pauseflag )

% glmSwapBuffers  Swap front and back buffers
% 
% usage:  [ t, w ] = glmSwapBuffers( pauseflag )
% 
% Return argument 't' is the absolute time at which the call to
% aglSwapBuffers() returned, and 'w' is the time that aglSwapBuffers()
% took to return, i.e., the time spent waiting for a vertical retrace.
% These values are useful for checking video timing.

% 09-Dec-2005 -- created (RFM)

% ---protected---

if nargin==0,
    pauseflag=(nargout>0);
end

[t,w]=moglcore('glmSwapBuffers',pauseflag);

return
