function destination = Magnify2DMatrix(source, scalingFactor) 
% Magnifies a two-dimensional sourceMatrix by a factor specified by scalingFactor.
% 
% 10/15/06 rhh Wrote it using lots of loops.
% 11/12/06 rhh Revised it.  Denis Pelli suggested revising the code to take
%               advantage of Matlab's matrix processing abilities and David Brainard
%               showed explicitly how this could be done.

[heightOfSource widthOfSource] = size(source);
% Generate row copying instructions index.
rowCopyingInstructionsIndex = [];
for i = 1 : heightOfSource
    rowCopyingInstructionsIndex = horzcat(rowCopyingInstructionsIndex, ones(1, scalingFactor) * i);
end   

% Generate column copying instructions index.
columnCopyingInstructionsIndex = [];
for i = 1 : widthOfSource
    columnCopyingInstructionsIndex = horzcat(columnCopyingInstructionsIndex, ones(1, scalingFactor) * i);
end

% The following code uses Matlab's matrix indexing quirks to magnify the
% matrix.  It is easier to understand how it works by looking at a specific
% example:
% 
% >> n = [1 2; 3 4] % Matlab, please give me a matrix with four elements.
%
% n =
% 
%      1     2
%      3     4
% 
% >> % Matlab, please generate a new matrix by using the provided copying
% >> % instructions index.  My copying instructions index says that you
% >> % should print the first column twice, then print the second column
% >> % twice.  Thanks.
% >> m = n(:, [1 1 2 2])
% 
% m =
% 
%      1     1     2     2
%      3     3     4     4
%
% >> % Matlab, please generate a new matrix by using the provided copying
% >> % instructions index.  My copying instructions index says that you
% >> % should print the first row twice, then print the second row
% >> % twice.  Thanks.
% >> m = n([1 1 2 2], :)
% 
% m =
% 
%      1     2
%      1     2
%      3     4
%      3     4
%

destination = source(rowCopyingInstructionsIndex, columnCopyingInstructionsIndex);