function lsf = WestLSFMinutes(distance)
%WESTLSFMINUTES  Compute Westheimer estimate of human LSF
%   lsf = WestLSFMinutes(distance)
%
%   Compute Westheimer's LSF function as a function
%   of passed distance.  Distance passed in minutes of arc.
%
%   This is only approximately consistent with Westheimer's PSF estimate.
%
%   This comes back normalized to a maximum of 1.
%
%   See also WestPSFMinutes, DavilaGeislerPSFMinutes, LsfToPsf, PsfToLsf

% 9/4/97  dhb  Wrote it.

lsf = 0.47*exp(-3.3*(distance.^2)) + ...
       0.53*exp(-0.93*(abs(distance)));
