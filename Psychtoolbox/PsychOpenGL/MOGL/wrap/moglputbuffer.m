function moglputbuffer(inputmatrix, ptr, nrbytes)
% moglputbuffer(inputmatrix, ptr, nrbytes) -- Copy content of matrix to buffer.
%
% This copies the content of the matrix 'inputmatrix' into the mogl memory
% buffer 'ptr' which was previously created via calls to ptr = moglmalloc(...)
% or ptr = moglcalloc(...). At most nrbytes are copied - if the target buffer
% is too small for nrbytes, only the amount is copied that fits into the buffer.
%
% Be careful to avoid setting nrbytes to a size that exceeds the size of
% inputmatrix, or Matlab/Octave will possibly crash or do other nasty things!

% 16.05.2006 Written (MK)

% ---protected---

  if nargin < 1 || isempty(inputmatrix)
     error('Missing required argument inputmatrix.');
     end;

  if nargin < 2 || isempty(ptr)
     error('Missing required argument ptr.');
     end;

  if nargin < 3 || isempty(nrbytes) || nrbytes < 1
     error('Missing or invalid (smaller than 1) argument nrbytes.');
     end;

moglcore('moglcopymatrixtobuffer', inputmatrix, ptr, nrbytes);
return;
