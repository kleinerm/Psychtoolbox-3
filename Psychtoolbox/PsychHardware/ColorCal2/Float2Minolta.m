function minoltaMatrix = Float2Minolta(floatMatrix)
% minoltaMatrix = Float2Minolta(floatMatrix)
%
% Description:
% Converts a matrix of floating point values to a matrix of Minolta
% formatted values.
%
% Input:
% floatMatrix (double matrix) - Matrix of floating point values.
%
% Output:
% minoltaMatrix (integer matrix) - Matrix of integer Minolta values.

minoltaMatrix = zeros(size(floatMatrix));

% Convert the negative values.
i = floatMatrix < 0;
minoltaMatrix(i) = -floatMatrix(i)*10000 + 50000;

% Convert the positive values.
i = floatMatrix >= 0;
minoltaMatrix(i) = floatMatrix(i)*10000;

% Floor all the values in the minolta matrix since they must be integers.
minoltaMatrix = floor(minoltaMatrix);
