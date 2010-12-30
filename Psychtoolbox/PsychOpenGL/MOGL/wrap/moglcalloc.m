function ptr = moglcalloc(nelements, nrbytes)
% ptr = moglcalloc(nelements, nrbytes) -- Allocate a memory buffer inside mogl.
% This function allocates an internal memory buffer inside mogl for use
% with OpenGL functions that use buffers for passing data forth and back
% between Matlab and OpenGL, e.g., glFeedbackBuffer() or glSelectBuffer().
%
% The allocated memory buffer is filled with zeros, just like the C-Function
% calloc() would do.
%
% nelements = The number of elements to allocate.
% nrbytes = The number of bytes for each element to allocate.
%
% -> nelements * nrbytes will be allocated.
%
% On successfull allocation, ptr will be a handle for the memory buffer.
% You can think of ptr as a memory pointer in the C-language.
%
% moglcalloc()'ed memory buffers can be released - one by one - via
% moglfree(ptr), all at once via moglfreeall, and they are automatically
% released when moglcore is clear'ed via clear moglcore, clear mex or
% clear all.
%

% 16.05.2006 Written (MK)

% ---protected---

if nargin < 1 || isempty(nelements) || nelements <1
   error('moglcalloc: Invalid or missing nelements size value for requested buffer.');
end;

if nargin < 2 || isempty(nrbytes) || nrbytes <1
   error('moglcalloc: Invalid or missing nrbytes size value for requested buffer.');
end;

ptr = moglcore('moglcalloc', nelements, nrbytes);
return;
