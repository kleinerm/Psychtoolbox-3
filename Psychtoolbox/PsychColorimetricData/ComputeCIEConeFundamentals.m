function T_quantal = ComputeCIEConeFundamentals(S,fieldSizeDegrees,ageInYears,pupilDiameterMM,lambdaMax,whichNomogram,LserWeight)
% T_quantal = ComputeCIEConeFundamentals(S,fieldSizeDegrees,ageInYears,pupilDiameterMM,[lambdaMax],[whichNomogram],[LserWeight])
%
% Function to compute normalized cone quantal sensitivities
% from underlying pieces, as specified in CIE 170-1:2006.
%
% This standard allows customizing the fundamentals for
% field size, observer age, and pupil size in mm.
%
% Note from DHB: Although this will compute something over any wavelength
% range, I'd recommend not going lower than 390 or above about 780 without
% thinking hard about how various pieces were extrapolated out of the range
% that they are specified in the standard.  Indeed, the lens optical
% density measurements only go down to 400 nm.
%
% This will also compute from absorbance based on a nomogram, where
% whichNomogram can be any source understood by the routine
% PhotopigmentNomogram.  To obtain this behavior, pass a lambdaMax vector.
% You can then also optionally pass a nomogram source (default: StockmanSharpe).
%
% If you pass lambaMax and its length is 4, the first two values are treated as
% the peak wavelengths of the ser/ala variants of the L cone pigment, and these
% are then weighted according to LserWeight and (1-LserWeight).  The default
% for LserWeight is 0.56.
%
% The nominal values of lambdaMax to fit the CIE 2-degree fundamentals are
% 558.9, 530.3, and 420.7 nm for the LMS cones respectively.  These in fact
% do a reasonable job of reconstructing the CIE 2-degree fundamentals.  Thus
% starting with these as nominal values and shifting is a reasonable way to
% produce fundamentals tailored to observers with different known photopigments.
% Relevant to that enterprise, S & S (2000) estimate the wavelength difference
% between the ser/ala variants to be be 2.7 nm (ser longer).
%
% See also: ComputeRawConeFundamentals, CIEConeFundamentalsTest, 
% FitConeFundamentalsTest, FitConeFundamentalsWithNomogram, StockmanSharpeNomogram.
%
% 8/13/11  dhb  Wrote it.
% 8/14/11  dhb  Clean up a little.

%% Get some basic parameters.  The Stockman-Sharpe data
% are not provided below 390, and things are cleaner if we
% start there rather than default 380.
whatCalc = 'LivingHumanFovea';
photoreceptors = DefaultPhotoreceptors(whatCalc);

%% Override default values so that FillInPhotoreceptors does
% our work for us.
photoreceptors.nomogram.S = S;
photoreceptors.fieldSizeDegrees = fieldSizeDegrees;
photoreceptors.ageInYears = ageInYears;
photoreceptors.pupilDiameter.value = pupilDiameterMM;
photoreceptors.lensDensity.source = 'CIE';
photoreceptors.macularPigmentDensity.source = 'CIE';
photoreceptors.axialDensity.source = 'CIE';

% Aborbance.  Use tabulated CIE values unless a nomogram and
% lambdaMax values are passed.
if (nargin > 4 && ~isempty(lambdaMax))
    if (nargin < 6 || isempty(whichNomogram))
        whichNomogram = 'StockmanSharpe';
    end
    staticParams.whichNomogram = whichNomogram;
    params.lambdaMax = lambdaMax;
else
    load T_log10coneabsorbance_ss
    photoreceptors.absorbance = 10.^SplineCmf(S_log10coneabsorbance_ss,T_log10coneabsorbance_ss,photoreceptors.nomogram.S,2);
    params.absorbance = photoreceptors.absorbance;
end

%% Do the work.  Note that to modify this code, you'll want a good
% understanding of the order of precedence enforced by FillInPhotoreceptors.
% This is non-trivial, although the concept is that if a quantity that
% can be computed is specified directly in the passed structure is
% actually specified, the speciefied value overrides what could be computed.
photoreceptors = FillInPhotoreceptors(photoreceptors);

%% Set up for call into the low level routine that computes the CIE fundamentals.
staticParams.S = photoreceptors.nomogram.S;
staticParams.fieldSizeDegrees = photoreceptors.fieldSizeDegrees;
staticParams.ageInYears = photoreceptors.ageInYears;
staticParams.pupilDiameterMM = photoreceptors.pupilDiameter.value;
staticParams.lensTransmittance = photoreceptors.lensDensity.transmittance;
staticParams.macularTransmittance = photoreceptors.macularPigmentDensity.transmittance;
if (nargin < 7 || iempty(LserWeight))
    staticParams.LserWeight = 0.56;
else
    staticParams.LserWeight = LserWeight;
end
params.axialDensity = photoreceptors.axialDensity.value;

%% Drop into more general routine to cmopute
T_quantal = ComputeRawConeFundamentals(params,staticParams);

