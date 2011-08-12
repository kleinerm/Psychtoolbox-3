
function [lensTransmit,lensDensity] = LensTransmittance(S,species,source,ageInYears)
% [lensTransmit,lensDensity] = LensTransmittance(S,[species],[source],[ageInYears)
%
% Return an estimate of the transmittance of the lens.
%
% Allowable species:
%   Human (Default)
%
% Allowable sources:
%   StockmanSharpe (Default) - Stockman, Sharpe, & Fach (1999).
%   CIE                      - Formula from CIE 170-1:2006.
%   WyszeckiStiles           - W&S, Table 1(2.4.6), p. 109.  First data set in table.
%   None                     - Unity transmittance.
%
% The answer is returned in a row vector.  This function
% depends on data contained in directory
% PsychColorimetricData:PsychColorimetricMatFiles.
%
% The CIE source will take again in years.  The
% default is 32.  The acceptable range is 20-80.
% 
%
% 7/8/03  dhb  Made this a separate function.
% 7/11/03 dhb  Species arg, change name.
% 7/23/03 dhb  Add Stockman estimate.
% 7/26/03 dhb  Extend functions, rather than zero truncate.
% 8/12/11 dhb  Start to write CIE version.  Return lensDensity too.

% Default
if (nargin < 2 || isempty(species))
	species = 'Human';
end
if (nargin < 3 || isempty(source))
	source = 'StockmanSharpe';
end
if (nargin < 4 || isempty(ageInYears))
	ageInYears = 32;
end

% Load correction for lens density
switch (species)
	case 'Human',
		switch (source)
			case 'None',
				lensTransmit = ones(S(3),1)';
			case 'WyszeckiStiles',
				load den_lens_ws;
				lensDensity = SplineSrf(S_lens_ws,den_lens_ws,S,1);
				lensTransmit = 10.^(-lensDensity)';
			case 'StockmanSharpe',
				load den_lens_ssf;
				lensDensity = SplineSrf(S_lens_ssf,den_lens_ssf,S,1);
				lensTransmit = 10.^(-lensDensity)';
            case 'CIE'
                if (ageInYears < 20)
                    error('Specified age must be 10 or older');
                elseif (ageInYears <= 60)
                    wls = MakeItWls(S);
                elseif (ageInYears <= 80)
                else
                    error('Specified age must be 80 or younger');
                end
                
			otherwise,
				error('Unsupported lens density estimate specified');
		end

	otherwise,
		switch (source)
			case ('None'),
				lensTransmit = ones(S(3),1)';
			otherwise,
				error('Unsupported species specified');
		end
end
