function [alpha,beta,thresh92] = FitWeibAlphTAFC(inputs,nCorrect,nError,...
	alpha0,beta0)
% [alpha,beta,thresh92] = FitWeibAlphTAFC(inputs,nCorrect,nError,...
%	  alpha0,beta0)
%
% Maximum likelihood fit of a Weibull function to psychometric data,
% search only on alpha with fixed beta as passed.
%
% Requires the optimization toolbox.
%
% INPUTS:
%   inputs:   contains the input levels
%   nCorrect: contains the number of yes responses at 
%             the corresponding input level
%   nError:   contains the number of no responses at 
%             the corresponding input level
%	alpha0:     Initial guess for alpha, empty for default [1].
%	beta0:		Value for beta
% OUTPUTS:
%   alpha				Weibull alpha parameter
%   beta				Weibull beta parameter
%   thresh92		92% percent correct threshold
%
% See also: FitWeibTAFC, FitFitWeibYN, FitCumNormYN, FitLogitYN
%
% 8/25/94	dhb, ccc    Cleaned comments, return 92% correct threshold.
% 2/8/97    dhb         Added check for optimization toolbox.
% 4/26/97   dhb         Change threshold to thresh92.
% 4/18/02   dhb         Fix reference to undefined variable 'levels'.
%           dhb         Suppress warnings in calls to optimization toolbox.
% 3/5/05	dhb			Update for optimization toolbox version 2.

% Initial parameters
if (isempty(alpha0))
	x0(1) = mean(inputs);
else
	x0(1) = alpha0;
end

% Check for needed optimization toolbox, and version.
if (exist('fminunc') == 2)
	options = optimset;
	options = optimset(options,'Diagnostics','off','Display','off');
	options = optimset(options,'LargeScale','off');
	x1 = fminunc('WeibAlphTAFCFitFun',x0,options,beta0,inputs,nCorrect,nError);
	x = fminunc('WeibAlphTAFCFitFun',x1,options,beta0,inputs,nCorrect,nError);
elseif (exist('fminu') == 2)
	options = foptions;
	state = warning; warning('off');
	x1 = fminu('WeibAlphTAFCFitFun',x0,options,[],beta0,inputs,nCorrect,nError);
	x = fminu('WeibAlphTAFCFitFun',x1,options,[],beta0,inputs,nCorrect,nError);
	warning(state);
else
	error('FitWeibTAFCAlph requires the optional Matlab Optimization Toolbox from Mathworks');
end

% Extract parameters
alpha = x(1);
beta = beta0;

% Compute threshold from fit parameters
thresh92 = FindThreshWeibTAFC(0.92,alpha,beta);
