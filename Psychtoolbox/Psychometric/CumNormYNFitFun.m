function [f,g] = CumulativeFitFun(x,level,nYes,nNo)
% [f,g] = CumulativeFitFun(x,level,nYes,nNo)
%
% 9/22/93   jms  Created from FitWeibullYN.

u    = x(1);
theVar  = x(2);

pYes = NormalCumulative(level,u,theVar);

% Handle range problem, can't take log(0);
tol = 1e-4;
z_index = find(pYes == 0);
if (~isempty(z_index))
  pYes(z_index) = tol*ones(length(z_index),1);
end
o_index = find(pYes == 1);
if (~isempty(o_index))
  pYes(o_index) = (1-tol)*ones(length(o_index),1);
end

% Compute error
tmp = nYes.*log(pYes) + nNo.*log(1 - pYes);
f =  -sum(tmp);
g = -1;






