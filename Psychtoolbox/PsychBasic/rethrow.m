function rethrow(msg)
% rethrow(msg) - Replacement for Matlab 6.5s builtin rethrow.
% This is hopefully useful for older Matlab installations and
% for the Octave port. 
error(msg);
return;
