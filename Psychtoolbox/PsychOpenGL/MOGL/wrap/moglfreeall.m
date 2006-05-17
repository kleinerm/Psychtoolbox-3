function moglfreeall
% moglfreeall -- Free all memory buffers inside mogl which were previously
% created via calls to ptr = moglmalloc(...) or ptr = moglcalloc(...).
%
% After successfull release, all Pointers will be invalid, *do not use them anymore*
% or Matlab/Octave will possibly crash or do other nasty things!
%
% moglcalloc()'ed or moglmalloc()'ed memory buffers can be released as well
% one by one via moglfree(), and they are automatically released when
% moglcore is clear'ed via clear moglcore, clear mex or clear all.
%

% 16.05.2006 Written (MK)

% ---protected---

moglcore('moglfreeall');
return;
