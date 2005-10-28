function macTransmit = MacularTransmittance(S,species,source)
% macTransmit = MacularTransmittance(S,[species],[source])
%
% Return an estimate of the transmittance of the
% macular pigment.
%
% Allowable species:
%   Human (Default)
%
% Allowable sources:
%   Bone  (Default)          - From Bone et al.  See CVRL database.
%   WyszeckiStiles           - From W&S, Table 2(2.4.6), p. 112.
%		Vos                      - From Vos.  See CVRL database.
%   None                     - Unity transmittance.
%
% The answer is returned in a row vector.  This function
% depends on data contained in directory PsychColorimetricData:PsychColorimetricMatFiles.
%
% 7/8/03  dhb  Made this a separate function.
% 7/11/03 dhb  Species arg, change name.
% 7/23/03 dhb  Change default.
% 7/26/03 dhb  Extend functions, rather than zero truncate.

% Default
if (nargin < 2 | isempty(species))
	species = 'Human';
end
if (nargin < 3 | isempty(source))
	source = 'WyszeckiStiles';
end

% Load correction for macular pigment density
switch (species)
	case 'Human',
		switch (source)
			case 'None',
				macTransmit = ones(S(3),1)';
			case 'WyszeckiStiles',
				load den_mac_ws;
				den_mac = SplineSrf(S_mac_ws,den_mac_ws,S,1);
				macTransmit = 10.^(-den_mac)';
			case 'Vos',
				load den_mac_vos;
				den_mac = SplineSrf(S_mac_vos,den_mac_vos,S,1);
				macTransmit = 10.^(-den_mac)';
			case 'Bone',
				load den_mac_bone;
				den_mac = SplineSrf(S_mac_bone,den_mac_bone,S,1);
				macTransmit = 10.^(-den_mac)';
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
