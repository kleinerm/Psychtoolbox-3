function K = BuildMarkovK(n,r,var)
% K = BuildMarkovK(n,r,var)
% K = BuildMarkovK(r,var)
%
% Build the covariance matrix for a Markov process as defined in Pratt, pp.
% 131 ff.
%
% This routine has two calling forms.  In the first form, three arguments
% are passed and the random variables are assumed to have the save
% variance.  Thus the passed var is a scalar.
%
% In the second form, the passed var is a column vector containing the
% variances of the individual random variables.
%
% 8/19/94		dhb		Wrote it.
% 2/6/96		dhb		Second calling form.
% 7/24/04       awi     Cosmetic.

% K = BuildMarkovK(n,r,var)
if (nargin == 3)
	if (length(var) ~= 1)
		disp('BuildMarkovK: Three arg calling form requires var be a scalar');
		error('K = BuildMarkovK(n,r,var)');
	end
	column = r .^ [0:n-1]';
	K = var*toeplitz(column);

% K = BuildMarkovK(r,var)
else
	var = r;
	r = n;
	n = length(var);
	sd = var.^0.5;
	column = r .^ [0:n-1]';
	var_var = sd*sd';
	K = var_var .* toeplitz(column);
end
