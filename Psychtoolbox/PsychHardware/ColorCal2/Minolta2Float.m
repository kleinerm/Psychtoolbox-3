function floatMatrix = Minolta2Float(minoltaMatrix)
% floatValue = Minolta2Float(minoltaMatrix)
%
% Description:
% Converts a Minolta formatted integer matrix to a floating point matrix.
%
% Input:
% minoltaMatrix (integer matrix) - Minolta value.
%
% Output:
% floatMatrix (double matrix) - Floating point representation of the Minolta value.

floatMatrix = zeros(size(minoltaMatrix));

% Convert all positive values.  Positive values are defined as those
% smaller than 50000.  All values positive and negative must be divided by
% 10000.
i = minoltaMatrix < 50000;
floatMatrix(i) = minoltaMatrix(i) / 10000;

% Convert all negative values.
i = minoltaMatrix >= 50000;
floatMatrix(i) = -(minoltaMatrix(i) - 50000) / 10000;
