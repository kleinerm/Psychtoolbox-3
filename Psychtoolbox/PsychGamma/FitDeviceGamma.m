function [gammaFit,gammaInputFit,fitComment] = ...
  FitDeviceGamma(gammaRaw,gammaInputRaw,fitType,nInputLevels)
% function [gammaFit,gammaInputFit,fitComment] = ...
%   FitDeviceGamma(gammaRaw,gammaInputRaw,[fitType],[nInputLevels])
%
% Fit the measured gamma function.  Appends 0 measurement,
% arranges data for fit, etc.
% 
% The returned gamma functions are normalized to a maximum of 1.
%
% If present, argument fitType is passed on to FitGamma.
%
% 11/14/06  dhb  Convert for [0-1] universe.  Add nInputLevels arg.

% Set up optional args
if (nargin < 3 || isempty(fitType))
    fitType = [];
end
if (nargin < 4 || isempty(nInputLevels))
    nInputLevels = 256;
end

% Extract device characteristics
[n,m] = size(gammaRaw);
nDevices = m;

% Fit gamma curve
gammaInputFit = linspace(0,1,nInputLevels)';
if (gammaInputRaw(1) ~= 0)
  gammaInputRaw = [0 ; gammaInputRaw];
  gammaRaw = [zeros(1,nDevices) ; gammaRaw];
end
gammaRawN = NormalizeGamma(gammaRaw);

[gammaFit,xFit,fitComment] = FitGamma(gammaInputRaw,gammaRawN,...
                             gammaInputFit,fitType);


