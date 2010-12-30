function moglfree(ptr)
% moglfree(ptr) -- Free a memory buffer inside mogl which was previously
% created via a call to ptr = moglmalloc(...) or ptr = moglcalloc(...).
% ptr = A pointer to the buffer to free. This is the handle returned by
% moglmalloc() or moglcalloc().
%
% After successfull release, ptr will be invalid, *do not use it anymore*
% or Matlab/Octave will possibly crash or do other nasty things!
%
% moglcalloc()'ed or moglmalloc()'ed memory buffers can be released as well
% all at once via moglfreeall, and they are automatically released when
% moglcore is clear'ed via clear moglcore, clear mex or clear all.
%

% 16.05.2006 Written (MK)

% ---protected---

if nargin < 1 || isempty(ptr)
   error('moglfree: Missing pointer to buffer for release.');
end;

moglcore('moglfree', ptr);
return;
