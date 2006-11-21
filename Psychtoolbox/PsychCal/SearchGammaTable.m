function [values] = SearchGammaTable(targets,input,table)
% [values] = SearchGammaTable(targets,input,table)
%
% Return the [0-1] entry from the passed table that produces
% output closest to the [0-1] target.
%
% The targets are assumed to be a row vector.
% The table is assumed to be a column vector.
% The returned indices and values are are row vectors.
%
% Works by exhaustive search.  A binary search might be faster
% but would be harder to vectorize.  I suspect that this is
% a fast Matlab implementation, but those who want to try are
% welcome to try to do better.  (Remember, though, that this
% routine gains in efficiency the more searches are done at once.
% This is because it contains no dreaded loops.)
%
% 4/2/94		dhb		Added code that checks for special case of zero output.
% 4/4/94		dhb		Fixed code added on 4/2.
% 4/5/94		jms		Fixed code added on 4/2.
% 1/21/95		dhb		Write search as a loop.  Loses time and elegance,
%						but prevents allocation of arrays that may be huge.
% 11/16/06      dhb     Renamed as SearchGammaTable.
%               dhb     Start work on converting to [0-1] universe.  Change
%                       name and interface.
% 11/20/06      dhb     Finish update by calling through MATLAB's interpolation function.

% Check dimensions
[m,n] = size(targets);
if (m ~= 1)
    error('Passed targets should be a row vector');
end
[mi,ni] = size(input);
if (ni ~= 1)
    error('Passed input should be a column vector');
end
[mt,nt] = size(table);
if (nt ~= 1)
    error('Passed table should be a column vector');
end
if (mi ~= mt || ni ~= nt)
    error('Input and table must be the same size');
end

% Invert via linearly interpolation of the passed table
values = interp1(table,input,targets','linear')';


