function lsf = DavilaGeislerLSFMinutes(distance)
% lsf = DavilaGeislerLSFMinutes(distance)
%
% Compute the LSF from Davila and Geisler, 1991, Figure 1.
% They give the parameters of a sum of Gau LSF function as a function
% of passed distance.  Distance passed in minutes of arc.
%
% Return has maximum of 1 at 0.
%
% 9/4/97  dhb  Wrote it.

weight1 = 0.409;
sigma1 = 0.417;
sigma2 = 1.42;
lsf = weight1*normpdf(distance,0,sigma1) + ...
       (1-weight1)*normpdf(distance,0,sigma2);
   
lsf = lsf/max(lsf(:));

