function theMean = ColStd(matrix)
% theMean = ColStd(matrix)
%
% Take the column standard deviation of a matrix.  Unlike std, always
% returns a vector, even if the matrix has only 1 row.
%
% 2/13/96		dhb		Wrote it.

[m,n] = size(matrix);
if (m == 1)
	theMean = zeros(size(matrix));
else
	theMean = std(matrix);
end
	
