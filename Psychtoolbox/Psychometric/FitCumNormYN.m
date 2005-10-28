function [uEst,varEst] = FitCumNormYN(inputs,nYes,nNo)
% [uEst,varEst] = FitCumNormYN(inputs,nYes,nNo)
%
% Fits a cumulative normal function to the passed yes-no data.
% Requires the optimization toolbox.
%
% INPUTS:
%   inputs:   Input levels
%   nYes:     Number of yes responses at the corresponding input level
%   nNo:      Number of no responses at the corresponding input level
% OUTPUTS:
%   uEst
%   varEst
%
% See also: FitWeibTAFC, FitFitWeibYN, FitWeibAlphTAFC, FitLogitYN
%
% 9/22/93   jms  	Created from FitWeibullYN.
% 2/8/97    dhb  	Cleaned up and added some comments.
%                	Check that optimization toolbox is present.
% 10/4/00   dhb  	Fixed bugs along lines suggested by Keith Schneider.
%                	Case of uInitial = 0 wasn't handled properly, and
%                	variance search limits were set based on mean.
% 3/4/05		dhb	  Conditionals for optimization toolbox version.

% Set up an initial guess
uInitial = mean(inputs);
varInitial = std(inputs)^2;

% Stuff guess into a vector
x0(1) = uInitial;
x0(2) = varInitial;
vlb = [-1e10 1e-10*varInitial];
vub = [1e10 1e10*varInitial];

% Check for needed optimization toolbox, and version.
if (exist('fmincon') == 2)
	options = optimset;
	options = optimset(options,'Diagnostics','off','Display','off');
	options = optimset(options,'LargeScale','off');
	x1 = fmincon('CumNormYNFitFun',x0,[],[],[],[],vlb,vub,[],options,inputs,nYes,nNo);
	x = fmincon('CumNormYNFitFun',x1,[],[],[],[],vlb,vub,[],options,inputs,nYes,nNo);
elseif (exist('constr') == 2)
	options = foptions;
	options(1) = 0;
	x1 = constr('CumNormYNFitFun',x0,options,vlb,vub,[],inputs,nYes,nNo);
	x = constr('CumNormYNFitFun',x1,options,vlb,vub,[],inputs,nYes,nNo)
else
	error('FitCumNormYN requires the optional Matlab Optimization Toolbox from Mathworks');
end

% Extract fit parameters parameters
uEst =   x(1);
varEst = x(2);
