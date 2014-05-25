function [fractionBleached] = ComputeConeBleaching(intensity,units,source)
% [fractionBleached] = ComputeConeBleaching(intensity,units,source)
%
% Compute fraction of cone pigment bleached given some measure of 
% the intensity of light reaching the eye.
%
% As far as I can tell, the fundemantal measurements of the half-bleach
% constant were made by Rushton and Henry (1968, Vision Reserch, 8, 617-631).
% This fact I learned from CVRL (http://www.cvrl.org/database/text/intros/introbleaches.htm).
%
% I am pretty sure that the Rushton and Henry measurements were made for 560 nn
% light, and they give (see their Figure 2) a half-bleach constant of 4.3 log trolands (20,000).
% This number is also given in Boynton and Kaiser, Human Color Vision, 2nd edition,
% pp 211 and following.
%
% It's probably find to compute bleaching for L and M cones given retinal illuminance
% in trolands, given that these are effects that matter over log10 units.   But trolands
% are not going to help much for the S-cones.  According to CVRL there aren't good 
% measurements for the half-bleaching constant for S cones because putting enough short-wavelength
% light onto the retina to bleach the S cones is not good for the eyes.that's not going to help
% with the S cones.
%
% None-the-less, it seems nice to have this routine written so that it will return a number
% if you give it intensity either in trolands or in isomerizations/cone-sec.  For 560 nm
% light and the CIE 10 deg fundamentals, I compute that 1 td is 137 isomerizations/cone-sec
% for L cones and 110 isomerizations/cone-sec for M cones.  Take the average value of 123.5
% and multiply by (10.^4.3) to get a half-bleach constant in isomerizations/cone-sec of 
% 2.46e+06 (6.39 log10 isomerizations/cone-sec).  [Computations done 5/23/14 using IsomerizationsInEyeDemo and
% setting the fundamentals to 'CIE10deg' and wavelength to 560 nm by hand in the code.]
%
% [ASIDE: I used 10 deg fundamentals because I figure that Rushton's measurements are based
% on a fairly large field.  Because the macular pigment absorbs a fair amount of light, 
% this matters.  If I compute instead with 2-deg fundamentals, I get that 1 td is 23.7
% L cone isomerizations/cone-sec and 19.5 M cone isomerizations/cone-sec.   These
% numbers are ballpark consistent with Rodiek page 475 who gives 18.3 and 15.9 for a 
% monochromatic 540 THz light (555 nm)]. 
%
% This routine will do the computation either on the basis of input in trolands or input
% in isomerization/cone-sec, using the appropirate constant as above.  Note that the
% computation of isomerizations takes into account lens and macular pigment, while the
% troland value is the straight troland value.  A second advantage of using units of
% isomerizations/cone-sec is that you can compute this for other regions of the visual
% field and presumably the numbers will be about right.  You can also compute for S-cones
% on the assumption that the half-bleach constant is the same for S-cones as for L- and M-
% cones.
%
% As far as I can tell, the computations and analysis of bleaching do not take into account
% changes in isomerization rate that occur because of change in spectral sensitivity of cones
% with bleaching.  In the measurements for 560 nm light, this would not be an issue if all cones
% were L cones. I suppose it could matter if the background light were monochromatic at a wavelength
% away from the cone peaks, where the number of isomerizations is going to drop faster with bleaching
% than assumed, leading to less total bleaching.
%
% Units
%   'troalands' -- input intensity in trolands.  Note that the computation only makes
%                  sense for L and M cones if this is the input.
%   'isomerizations' -- nominal isomerization rate in isomerizations/cone-sec, comptued
%                  taking into account pre-retinal absorption as well as nominal cone
%                  axial density.  But not taking into account any pigment bleaching.
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
        switch (units)
            case 'trolands'
                Izero = 10^4.3;
            case 'isomerizations'
                Izero = 1^6.39;
            otherwise
                error('Unkown input units specified');
        end
        fractionBleached = (intensity./(intensity + Izero));
      
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
% fractionBleached = ComputeConeBleaching(trolands,'trolands','Boynton');
% plot(log10(trolands),fractionBleached,'r');


