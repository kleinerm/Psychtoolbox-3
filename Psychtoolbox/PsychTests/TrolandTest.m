% TrolandTest
%
% Test the conversion routines between various "intensity"
% measures, for monochromatic lights.
%
% Since these routines were written, there are additional
% routines that I think work for full spectra.  See
%   TrolandsToRetIrradiance
%   RetIrradianceToTrolands
%
% 8/16/96  dhb, abp  Made ourselves write it late at night.
% 8/12/11  dhb       Expanded comment.

% Load in vLambda if its not there
if (exist('vLambda') ~= 1)
	load T_xyz1931
	vLambda = T_xyz1931(2,:);
	S_vLambda = S_xyz1931;
	clear T_xyz1931 S_xyz1931
end

% Set up some initial values
quanta = 1000;
wl = 580;

% Convert from quanta to trolands and then to power and back to quanta
trolands = QuantaToTrolands(wl,quanta,S_vLambda,vLambda);
power = TrolandsToPower(wl,trolands,S_vLambda,vLambda);
quanta1 = EnergyToQuanta(wl,power);

% Convert from quanta to power to trolands and back to quanta
power1 = QuantaToEnergy(wl,quanta1);
trolands1 = PowerToTrolands(wl,power1,S_vLambda,vLambda);
quanta2 = TrolandsToQuanta(wl,trolands1,S_vLambda,vLambda);

% Display results
fprintf('Quantities in each line below should agree.\n');
fprintf('\tquanta = %g, quanta1 = %g, quanta2 = %g\n',...
	quanta,quanta1,quanta2);
fprintf('\ttrolands = %g, trolands1 = %g\n',trolands,trolands1);
fprintf('\tpower = %g, power1 = %g\n',power,power1);
