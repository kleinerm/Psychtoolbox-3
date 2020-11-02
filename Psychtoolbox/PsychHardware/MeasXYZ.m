function [XYZ, qual] = MeasXYZ(meterType)
% [XYZ, qual] = MeasXYZ([meterType=1])
%
% Measures XYZ tristimulus coordinates, luminance in
% cd/m^2.
%
%   meterType 1 is the PR650 (default)
%   meterType 2 is the CVI (need CVIToolbox) - Not yet implemented!
%   meterType 3 is the CRS Colorimeter
%   meterType 4 is the PR655
%   meterType 5 is the PR670
%   meterType 6 is the PR705 - Not yet implemented!
%   meterType 7 is the CRS ColorCal2.
%
%
% Returns XYZ tristimulus coordinates in 'XYZ'.
% Returns quality code in 'qual': 0 = Ok. Other numbers mean trouble, e.g., -8
% means "low light" ie. insufficient precision on some devices like the PR-xxx's.
%

%    9/14/93        jms     Added global no hardware switch
%    10/1/93        dhb     Removed print on low light, passed qual on up
%    10/4/93        dhb     Handle quality code 18 properly
%    1/16/94        jms     Removed 'exist' check and declared globals.
%    2/20/94        dhb     Modified to use CMETER.
%    8/9/94         dhb     Handle sync mode error condition.
%    9/7/94         dhb     Remove sync mode message.
%    09/11/00       dhb     Added meterType argument.
%                   dhb     Encapsulate call in OS9 check.  Need to add Win code.
%    2/15/02        dhb     Rewrote completely to use PsychSerial.
%    5/21/02        dgp     Tidied up code, removing superfluous COMPUTER conditional.
%    2/23/03        dhb     Added CRS colorimeter, meter type 3.
%    2/26/03        dhb     Change definition of meter type for PR-650 to 1.

persistent colorcal2matrix;

% Handle defaults
if nargin < 1 || isempty(meterType)
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

    case 5,
        [XYZ, qual] = PR670measxyz;

    case 7,
        % ColorCal2
        if isempty(colorcal2matrix)
            colorcal2matrix = ColorCal2('ReadColorMatrix');
        end

        s = ColorCal2('MeasureXYZ');
        if s.reliable
            % Mark as reliable:
            qual = 0;
        else
            % Mark as unreliable or faulty or even invalid:
            qual = -1;
        end

        XYZ = colorcal2matrix(1:3, :) * [s.x s.y s.z]';

    otherwise,
        error('Unknown or unsupported meter type');
end
