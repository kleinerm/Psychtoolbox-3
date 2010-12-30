function [XYZ,qual] = MeasXYZ(meterType)
% [XYZ,qual] = MeasXYZ([meterType])
%
% Measures XYZ tristimulus coordinates, luminance in
% cd/m^2.
% 
%		meterType == 1:  PR650 (default).
%       meterType == 3:  CRS colorimeter.
%		meterType == 4:  PR655.

%	9/14/93		jms		Added global no hardware switch
%	10/1/93		dhb		Removed print on low light, passed qual on up
%	10/4/93		dhb		Handle quality code 18 properly
%	1/16/94		jms		Removed 'exist' check and declared globals.
%	2/20/94		dhb		Modified to use CMETER.
%	8/9/94		dhb		Handle sync mode error condition.
% 9/7/94		dhb		Remove sync mode message.
% 09/11/00  dhb   Added meterType argument.
%           dhb   Encapsulate call in OS9 check.  Need to add Win code.
% 2/15/02   dhb   Rewrote completely to use PsychSerial.
% 5/21/02   dgp		Tidied up code, removing superfluous COMPUTER conditional.
% 2/23/03   dhb   Added CRS colorimeter, meter type 3.
% 2/26/03   dhb   Change definition of meter type for PR-650 to 1.

% Handle defaults
if (nargin < 1 || isempty(meterType))
    meterType = 1;
end

switch meterType
    % PR-650
    case 1,
        [XYZ, qual] = PR650measxyz;
    case 3,
        XYZ = CRSColorMeasXYZ;
        qual = 0;
    % PR-655
    case 4,
        [XYZ, qual] = PR655measxyz;
    otherwise,
        error('Unknown meter type');
end
