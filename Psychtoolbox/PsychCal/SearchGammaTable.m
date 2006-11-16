function [values] = SearchGammaTable(targets,table)
% [values] = SearchGammaTable(targets,table)
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
%               dhb     Start work on converting to [0-1] universe.

% Check dimensions
[m,n] = size(targets);
if (m ~= 1)
  error('Passed targets should be a row vector');
end
[mt,nt] = size(table);
if (nt ~= 1)
  error('Passed table should be a column vector');
end

% Find difference between each entry of table and the target
% The magic of dot products and the way function min is
% implemented allow us to the search for all of the targets
% in just a few lines, but this uses lots of memory when n is
% large.  So instead we write the loop out.
%diffs = table*ones(1,n)-ones(mt,1)*targets;
%[null,indices] = min(abs(diffs));
%values = table(indices)';
indices = zeros(1,n);
values = zeros(1,n);
for j = 1:n
	diff = table-ones(mt,1)*targets(j);
	[value,index] = min(abs(diff));
	indices(j) = index;
	values(j) = table(index);
end

% Special case to find when values are zero and turn device
% right on off (indices = 1).  This is to deal with the possibility
% that some light is really coming off for non-zero settings
% but that we couldn't measure it.
zindex = find( values == 0 ) ;
if (~isempty(zindex))
	indices(zindex) = ones(1,length(zindex));
end

