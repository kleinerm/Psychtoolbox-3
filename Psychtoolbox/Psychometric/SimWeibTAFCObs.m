function response = SimWeibTAFCObs(log10Intensity,alpha,beta)
% response = SimWeibTAFCObs(log10Intensity,alpha,beta)
%
% Return response 0 or 1 according to a Weibull psychometric function
% with given alpha and beta.  0 means incorrect, 1 means correct.
%
% 8/19/94		dhb, ccc		Wrote it.
% 8/25/94		dhb, ccc		Changed name.

probCorr = ComputeWeibTAFC(10.^log10Intensity,alpha,beta);
response = CoinFlip(1,probCorr);
