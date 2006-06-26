% clearmex -- Implementation of "clear mex" for GNU/Octave.
%
% This routine does what Matlab's "clear mex" would do, but
% it takes the special requirements of Octave into account.
%
% You can use this command in scripts for Matlab as well, it will
% do the right thing.

% History:
% 05/11/06 written (MK)

% Call clear functions for all Psychtoolbox MEX/OCT files.
clearScreen;
clearGetSecs;
clearWaitSecs;
clearmoglcore;
