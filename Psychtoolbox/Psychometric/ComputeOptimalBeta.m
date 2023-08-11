function beta = ComputeOptimalBeta(ps,pn,Vh,Vfa,Vm,Vcr)
% beta = ComputeOptimalBeta(ps,pn,Vh,Vfa,Vm,Vcr)
%
% Compute the optimal beta to be used in a yes-no
% experiment with the passed prior odds and payoffs.
%
% xx/xx/xx  dhb  Wrote it.

beta = (pn/ps)*(Vcr + Vfa)/(Vh + Vm);

