function photoreceptors = DefaultPhotoreceptors(kind)
% photoreceptors = DefaultPhotoreceptors(kind)
% 
% Return a structure containing default sources 
% for photoreceptor complements of various kinds.
%
% Available kinds
%   LivingHumanFovea (Default) - Human foveal cones in the eye
%   LivingHumanMelanopsin - Estimate of melanopsin gc spectral sensitivity in living eye
%   LivingDog - Canine
%   GuineaPig - Guinea pig in dish
%
% See also:  FillInPhotoreceptors, RetIrradianceToIsoRecSec
%  IsomerizationsInEyeDemo, IsomerizationsInDishDemo, ComputeCIEConeFundamentals. 
%
% 7/25/03  dhb  Wrote it.
% 12/04/07 dhb  Added dog parameters
% 8/14/11  dhb  Added fieldSizeDegrees and ageInYears fields to photoreceptors for LivingHumanFovea case.
%               These defaults match the CIE standard.
% 4/20/12  dhb  Add LivingHumanMelanopsin
%
% NOTES: Should probably update the parameters for LivingHumanFovea so that
% they produce the Stockman-Sharpe fundamentals.  This should be pretty
% straightforward, now that all the pieces are implemented as via ComputeCIEConeFundamentals. 

% Default
if (nargin < 1 || isempty(kind))
	kind = 'LivingHumanFovea';
end

% Fill it in
switch (kind)
	case 'LivingHumanFovea'
		photoreceptors.species = 'Human';
		photoreceptors.OSlength.source = 'Rodieck';
		photoreceptors.ISdiameter.source = 'Rodieck';
		photoreceptors.specificDensity.source = 'Rodieck';
		photoreceptors.lensDensity.source = 'StockmanSharpe';
		photoreceptors.macularPigmentDensity.source = 'Bone';
		photoreceptors.pupilDiameter.source = 'PokornySmith';
		photoreceptors.eyeLengthMM.source = 'Rodieck';
		photoreceptors.nomogram.source = 'StockmanSharpe';
		photoreceptors.nomogram.S = [380 1 401];
		photoreceptors.nomogram.lambdaMax = [558.9 530.3 420.7]';
		photoreceptors.types = {'FovealLCone' 'FovealMCone' 'FovealSCone'};
		photoreceptors.quantalEfficiency.source = 'Generic';
        photoreceptors.fieldSizeDegrees = 2;
        photoreceptors.ageInYears = 32;
    
    % This creates Tsujiumura's estimate of the melanopsin gc
    % spectral sensitivity in the human eye. The quantal efficiency
    % is just made up, though, so that the code runs.
    case 'LivingHumanMelanopsin'
        photoreceptors.species = 'Human';
		photoreceptors.lensDensity.source = 'CIE';
		photoreceptors.macularPigmentDensity.source = 'CIE';
        photoreceptors.axialDensity.source = 'Tsujimura';
        photoreceptors.axialDensity.value = 0.5;
		photoreceptors.nomogram.source = 'Govardovskii';
		photoreceptors.nomogram.S = [380 1 401];
		photoreceptors.nomogram.lambdaMax = [482]';
		photoreceptors.types = {'Melanopsin'};
        photoreceptors.quantalEfficiency.source = 'None';
        photoreceptors.quantalEfficiency.value = 1;
        photoreceptors.fieldSizeDegrees = 10;
        photoreceptors.ageInYears = 32;

    case 'LivingDog'
		photoreceptors.species = 'Dog';
		photoreceptors.OSlength.source = 'PennDog';
		photoreceptors.ISdiameter.source = 'PennDog';
		photoreceptors.specificDensity.source = 'Generic';
		photoreceptors.lensDensity.source = 'None';
		photoreceptors.macularPigmentDensity.source = 'None';
		photoreceptors.pupilDiameter.source = 'PennDog';
		photoreceptors.eyeLengthMM.source = 'PennDog';
		photoreceptors.nomogram.source = 'Govardovskii';
		photoreceptors.nomogram.S = [380 1 401];
		photoreceptors.nomogram.lambdaMax = [555 429 506]';
		photoreceptors.types = {'LCone' 'SCone' 'Rod'};
		photoreceptors.quantalEfficiency.source = 'Generic';
        
	case 'GuineaPig'
		photoreceptors.species = 'GuineaPig';
		photoreceptors.OSlength.source = 'SterlingLab';
		photoreceptors.OSdiameter.source = 'SterlingLab';
		photoreceptors.ISdiameter.source = 'SterlingLab';
		photoreceptors.specificDensity.source = 'Bowmaker';
		photoreceptors.lensDensity.source = 'None';
		photoreceptors.macularPigmentDensity.source = 'None';
		photoreceptors.pupilDiameter.source = 'None';
		photoreceptors.eyeLengthMM.source = 'None';
		photoreceptors.nomogram.source = 'Govardovskii';
		photoreceptors.nomogram.lambdaMax = [529 430 500]';
		photoreceptors.nomogram.S = [380 1 401];
		photoreceptors.types = {'MCone' 'SCone' 'Rod'};
		photoreceptors.quantalEfficiency.source = 'Generic';
	otherwise
		error('Unknown photoreceptor kind specified');
end
