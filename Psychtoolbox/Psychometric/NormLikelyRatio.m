function [likelyr,likelys,likelyn] = ...
  NormLikelyRatio(x,un,varn,us,vars)
% [likelyr,likelys,likelyn] = NormLikelyRatio(x,un,varn,us,vars)
%
% Compute the likelyhood ratio l(x) for univariate,
% Normally distributed signal and noise distributions. 
%
% xx/xx/xx  dhb  Wrote it.

likelys = NormalPDF(x,us,vars);
likelyn = NormalPDF(x,un,varn);
likelyr = likelys ./ likelyn;
