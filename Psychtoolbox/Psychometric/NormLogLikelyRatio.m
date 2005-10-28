function [loglikelyr,loglikelys,loglikelyn] = ...
  NormLogLikelyRatio(x,un,varn,us,vars)
% [loglikelyr,loglikelys,loglikelyn] = ...
%   NormLogLikelyRatio(x,un,varn,us,vars)
%
% Compute log of the likelyhood ratio l(x) for univariate.
% Normally distributed signal and noise distributions. 
%
% xx/xx/xx  dhb  Wrote it.

[likelyr,likelys,likelyn] = NormLikelyRatio(x,un,varn,us,vars);
loglikelys = log( likelys );
loglikelyn = log( likelyn );
loglikelyr = log( likelyr );
