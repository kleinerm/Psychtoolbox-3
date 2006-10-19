function [spectrum,qual] = MeasSpd(S,meterType)
% [spectrum,qual] = MeasSpd([S],[meterType])
%
% This routine splines the raw return values from the
% meter to the wavelength sampling S.  The splining
% handles conversion of power units according to 
% to the wavelength sampling delta.  If S is not passed,
% it is set to [380 5 81].
%
% Tries to handle low light level case gracefully by returning
% zero as the answer. 
%
% meterType == 1:  PR650 (default)

% 9/3/93		dhb		Added default handling of S.
% 9/14/93		jms		Added global no hardware switch
% 10/1/93		dhb		Removed print on error, passed qual on up
% 10/4/93		dhb		Handle quality code 18 properly.
% 1/16/94		jms		Removed 'exist' check and declared globals.
% 2/20/94		dhb		Modified for CMETER.
% 8/11/94		dhb		Handle sync mode error condition.
% 9/7/94		dhb		Remove sync mode message.
% 11/6/96   dhb   Remove extra call to CMETER('Measure').
% 6/17/98   dhb   Add meterType switch.
% 7/1/98    dhb,jmk Fix bug in switch.
% 10/4/99   dhb,mdr Take return of -1 (timeout) to mean no light.
%           dhb,mdr Remove gHardware and gHardwareMsg globals.
% 4/4/00    dhb,jdt Pass S to PC version.
% 09/11/00  dhb   Remove colortron support.  It never worked right.
% 1/10/02   dhb,ly Make OS9 version use SERIAL.
% 2/15/02   dhb   Change name of called routine.
% 5/21/02   dgp   Tidied up code, removing superfluous COMPUTER conditional.
% 2/26/03   dhb   Change definition of PR-650 meter type to 1.

% Handle defaults
if nargin < 2 || isempty(meterType)
  meterType = 1;
end
if nargin < 1 || isempty(S)
  S = [380 5 81];
end 

switch meterType
	% PR-650
  case 1,
		[spectrum, qual] = PR650measspd(S);				
	otherwise,
		error('Unknown meter type');
end
	
