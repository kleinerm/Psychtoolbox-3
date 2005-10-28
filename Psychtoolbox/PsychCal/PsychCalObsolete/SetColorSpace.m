function [cal,errorRet] = SetColorSpace(cal,T_linear,S_linear,quiet)
% [cal,errorRet] = SetColorSpace(cal,T_linear,S_linear,[quiet])
%
% Initialize the color space for use in calibration.  Requires
% a calibration structure bag which contains the standard
% fields.  These are checked for and a message is printed if
% they are not there.
%
% Checks that wavelength sampling is consistent and splines
% if not.
%
% errorRet indicates status of the operation.
%   == 0: OK
%   == 1: Bad condition number on linear/device conversion matrix 
%
% quiet flag suppresses error messages, default 0.
%
% SetColorSpace has been renamed "SetSensorColorSpace".  The old
% name, "SetColorSpace", is still provided for compatability 
% with existing scripts but will disappear from future releases 
% of the Psychtoolbox.  Please use SetSensorColorSpace instead.
%
% See Also: PsychCal, PsychCalObsolete, SetSensorColorSpace
 
% 9/13/93     dhb   Wrote it.
% 10/16/93    jms   Added optional calData arg which is freed if passed.
% 10/30/93    dhb   Added nDevices, nBases arguments.
%             dhb   Removed optional freeing.  It is a little too clever.
% 11/11/93    dhb   Re-wrote entirely in Matlab.  The C version is too clever.
% 11/17/93    dhb   Call back through new, less clever, C routines.
%             dhb   Support to store M_ambient_linear with compute vars
% 8/4/96      dhb   Converted to modern format from original DHB scheme.
% 8/21/97     dhb   Converted for structures.
% 3/10/98	  dhb	Store T_linear and S_linear.  Seems like a good idea.
%		            Remove nBasesIn, which is never used.
% 					Change nBasesOut to nPrimaryBases.
% 1/4/00      mpr   Added quiet flag to suppress display of messages I don't care about.
% 2/25/99     dhb   Fix case of SplineCmf.
% 4/5/02      dhb   Call through new interface.
% 4/11/02     awi   Added help comment to use SetSensorColorSpace instead.
%                   Added "See Also"
% 4/23/04     dhb   Make quiet the default.


if nargin < 4 | isempty(quiet)
  quiet = 1;
end

[cal,errorRet] = SetSensorColorSpace(cal,T_linear,S_linear,quiet);
