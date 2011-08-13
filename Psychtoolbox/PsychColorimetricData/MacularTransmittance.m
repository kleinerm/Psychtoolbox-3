function [macTransmit,macDensity] = MacularTransmittance(S,species,source,fieldSizeDegrees)
% [macTransmit,macDensity] = MacularTransmittance(S,[species],[source],[fieldSizeDegrees])
%
% Return an estimate of the transmittance of the macular pigment transmittance
% as a function of wavelength.
%
% Allowable species:
%   Human (Default)
%
% Allowable sources:
%   CIE (Default)            - CIE 170-1:2006 values.
%   Bone                     - From Bone et al.  See CVRL database.
%   WyszeckiStiles           - From W&S, Table 2(2.4.6), p. 112.
%   Vos                      - From Vos.  See CVRL database.
%   None                     - Unity transmittance.
%
% For the CIE option, can pass fieldSizeDegrees [Default 2 degrees].
% The formulae for field size in 170-1:2006 do not reproduce the
% values tabulated for 10-degrees.  The formula gets the peak right,
% but the difference in the tabulated 2 and 10 degree densities is
% not constant, so the simple additive correction recommended
% is inconsistent with the tabular values.  Go figure.
%
% The Bone values match those in  CIE 170-1:2006, Table 6.4
% for a 2-degree observer.
% 
% The answer is returned in a row vector.  This function
% depends on data contained in directory
% PsychColorimetricData:PsychColorimetricMatFiles.
%
% 7/8/03  dhb  Made this a separate function.
% 7/11/03 dhb  Species arg, change name.
% 7/23/03 dhb  Change default.
% 7/26/03 dhb  Extend functions, rather than zero truncate.
% 8/12/11 dhb  Fixed default to match comments.
%         dhb  Add CIE option and made it default.
%         dhb  For CIE, can pass field size
%         dhb  Also return density
% 8/13/11 dhb  Linearly extrapolate read functions outside of range.


% Default
if (nargin < 2 || isempty(species))
	species = 'Human';
end
if (nargin < 3 || isempty(source))
	source = 'CIE';
end
if (nargin < 4 || isempty(fieldSizeDegrees))
    fieldSizeDegrees = 2;
end

% Load correction for macular pigment density
switch (species)
	case 'Human',
		switch (source)
			case 'None',
				macTransmit = ones(S(3),1)';
			case 'WyszeckiStiles',
				load den_mac_ws;
				macDensity = SplineSrf(S_mac_ws,den_mac_ws,S,2);
				macTransmit = 10.^(-macDensity)';
			case 'Vos',
				load den_mac_vos;
				macDensity = SplineSrf(S_mac_vos,den_mac_vos,S,2);
				macTransmit = 10.^(-macDensity)';
			case 'Bone',
				load den_mac_bone;
				macDensity = SplineSrf(S_mac_bone,den_mac_bone,S,2);
				macTransmit = 10.^(-macDensity)';
            case 'CIE'
                load den_mac_bone;
				macDensity = SplineSrf(S_mac_bone,den_mac_bone,S,2);
                
                % Adjust for field size.  Our bone values have a peak of
                % 0.35, but the CIE formula assume normalization to peak
                % of 1.  We simply adjust when applying the formula.
                densityAdjustFieldSize = 0.485*exp(-fieldSizeDegrees/6.132) - 0.485*exp(-2/6.132);
                macDensity = macDensity+densityAdjustFieldSize;
                macDensity(macDensity < 0) = 0;
                
				macTransmit = 10.^(-macDensity)';
			otherwise,
				error('Unsupported macular pigment density estimate specified');
		end

	otherwise,
		switch (source)
			case ('None'),
				macTransmit = ones(S(3),1)';
			otherwise,
				error('Unsupported species specified');
        end
end

