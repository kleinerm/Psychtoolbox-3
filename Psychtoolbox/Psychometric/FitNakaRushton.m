function [params,f] = FitNakaRushton(contrast,response,params0)
% [params,f] = FitNakaRushton(contrast,response,[params0])
%
% Find the parameters that best fit the data.  If the initial guess
% isn't good, this can get stuck in local minima.  
%
% 8/1/05    dhb, pr     Wrote it.
% 8/2/07    dhb         Change argument order.

% If params0 isn't passed, try to pick a good default.
if (nargin < 3 || isempty(params0))
    params0(1) = max(response);
    params0(2) = mean(contrast);
    params0(3) = 2;
end

% Set up minimization options
options = optimset;
options = optimset(options,'Diagnostics','off','Display','off');
options = optimset(options,'LargeScale','off');
vlb = [0      0.001	0.001];
vub = [1000   100000	100];
params = fmincon('FitNakaRushtonFun',params0,[],[],[],[],vlb,vub,[],options,...
	contrast,response); 
f = FitNakaRushtonFun(params,contrast,response);
