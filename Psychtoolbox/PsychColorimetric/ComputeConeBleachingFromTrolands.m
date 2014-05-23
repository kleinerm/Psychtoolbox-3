function [fractionBleached] = ComputeConeBleachingFromTrolands(trolands,source)
% [axialOpticalDensity] = ComputeConeBleachingFromTrolands(trolands,source)
%
% The literature seems happy to provide formulae for fraction of cone
% pigment bleached given the retinal illuminance in trolands.
%
% This makes little sense in general, because we can't say much from 
% the troland value about what is happening to the S-cones.  For the
% L and M cones, the approximation is probably pretty reasonable given
% that these are log unit sorts of effects.
%
% Source
%   'Boynton' -- Boynton and Kaiser, Human Color Vision, 2nd edition,
%                pp. 211 and following.  [Default.]
% 05/23/14 dhb  Wrote it.

%% Specify source 
if (nargin < 2 || isempty(source))
    source = 'Boynton';
end

%% Do it
switch (source)
    case 'Boynton'
        Izero = 10^4.3;
        fractionBleached = (trolands./(trolands + Izero));
    otherwise
        error('Unknown source specified');
end

end

%% Test code.
%
% If you run the lines below you should get a plot that
% looks like Figure 6.3 (p. 212) in Boyton and Kaiser.
% 
% trolands = logspace(0,7,1000);
% fractionBleached = ComputeConeBleachingFromTrolands(trolands,'Boynton');
% plot(log10(trolands),fractionBleached,'r');


