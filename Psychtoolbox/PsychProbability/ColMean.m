function theMean = ColMean(matrix)
% theMean = ColMean(matrix)
%
% Take the column mean of a matrix.  Unlike mean, always returns a vector,
% even if the matrix has only 1 row.
%
% 2/13/96		dhb		Wrote it.
% 7/24/04       awi     Cosmetic.


[m,n] = size(matrix);
if (m == 1)
	theMean = matrix;
else
	theMean = mean(matrix);
end
	
