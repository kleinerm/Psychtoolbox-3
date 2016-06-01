% CIEConeFundamentalsFieldSizeTest
%
% Look at how the quantities in the CIE standard that depend on field size vary with 
% field size, particularly for field sizes greater than 10-degrees, which
% is the outer limit of what the standard sanctions.
%
% My conclusions are
%   1. that since we know macular pigment is declining towards zero with
%   field size (see refs below) and the CIE expoential formula has this
%   property, it is resonable to use the CIE formula for macular pigment
%   density for field sizes greater than 10-degrees.
%   2. the CIE formula for pigment optical density asymptotes to a constant
%   as one extends past 10-degrees.  Whether this is true in the retina or
%   not, I am not sure.  But as field size gets larger, using this formula
%   is going to be no worse than simply using the 10-degree values, since
%   they are the same.  
%   3. one needs to take any estimate of the CMFs for large field sizes
%   with a grain of salt.  There is going to be variation across the field,
%   so any point estimate is likely to be wrong somewhere.  The CIE
%   formulae, extended using the formulae past their bounds, are a good
%   first guess for the mean field properties, but being aware that there
%   is variation within the field, as well as individual variation around
%   the CIE estimates, is important when considering things like the effect
%   of inadvertant stimulation of cones when one tries to isolate
%   melanopsin using silent substitution.  Note in particular that the CIE
%   formula is trying to capture large field color matches where subjects
%   are instructed to ignore the center of the field as best they can.  In
%   a threshold experiment, this might not be how subjects were instructed
%   and would in any case be rather hard to do.  And if you used annular
%   stimuli, you'd be a bit off and might want to think about how to
%   estimate the fundamentals from the annulus.  Studying the Moreland and
%   Alexander paper below in detail might help with thinking on that.
%
% Refs:
%   Mooreland & Alexander (1997).  Effect of macular pigment on color 
%   matching with field sizes in the 1 deg to 10 deg range.  Doc. Opth.
%   Proc. Ser., 59, 363-368.
%         Moreland and Alexander make color matches for annuli and for
%         circular fields, and develop a formula for the equivalent macular
%         pigment density for color matches of various field sizes.  In
%         these matches, observers are instructed to to ignore the central
%         Maxwell?s spot, I am pretty sure. M&A say the data are consistent
%         with the idea that obsevers look near the edge of the field but
%         not quite at it.  Their data are for field sizes of 10 degrees,
%         and the estimates are fit with exponentials.  M&A used these data
%         together with measurements of macular pigment density by Moreland
%         & Bhatt (1884) to develop an equivalent (for uniform fields)
%         macular pigment density to be used in predicting color matches
%         out to 10 degrees.  The formula is an exponential decay.
%         Something like this made it into the CIE standard, although it
%         may have been tweaked to make sure the color matching data are
%         consistent with the 10-deg and 2-deg CMFs.  I have not thought
%         hard about the underlying calculations.
%
% Moreland, J.D. and Bhatt, P. (1984). Retinal distribution ofmacular
% pigment. In: Verriest, G. (ed.), Colour Vision Deficiencies VII. Doc.
% Ophthalmol. Proc. Ser. 39: 127-132. W. Junk, The Hague.
%         Uses color-matching data for field sizes out to 18 deg (I think)
%         to develop estimates of macular pigment density as a function of
%         eccentricity.  Key feature of the data is that density estimates
%         decline according to an exponential and reach zero at
%         eccentricity (radius) of about 7 degrees.  This paper also
%         reviews earlier estimates and they all look like decaying
%         exponentials.
%
%   Putnam and Bland (2014).  Macular pigment optical density spatial
%   distribution measured in a subject with oculocutaneous albinism.
%   Journal of Optometry, 7, 241-245.
%
% See ComputeCIEConeFundamentals, CIEConeFundamentalsTest.
%
% 5/25/16  dhb  Wrote it.
% 6/1/16   dhb  Polished it up a bit, and added to PTB distribution.

% Initialize and clear
clear; close all;

%% Generate field sizes
fieldSizesDegrees = 1:1:60;
S = 460;

%% Macular pigment transmittance
%
% This looks like it matches Figure 5.1 (p. 17) of the CIE 2006 standard
% between 1 and 10 degrees.
for ii = 1:length(fieldSizesDegrees)
    % This is the CIE formula, and presumably gives some sort of effective
    % macular pigment density over the field, with some sort of exclusion
    % of the center for 10 degrees.  I think this because presumbably these
    % values are chosen to make the color matching functions produced using
    % spatially uniform macular pigment density come out right.  I believe
    % this formula comes from Mooreland & Alexander (1997).
    [macTran(ii),macDen(ii)] = MacularTransmittance(S,'Human','CIE',fieldSizesDegrees(ii));
    
    % Isetbio has a function that provides macular density as a function of
    % eccentricity.  This is probably a different function than the above,
    % since I believe the above comes from an "equivlent" density
    % appropriate for a uniform field.  But we can at least look at this
    % too.  The original data are in Putnam & Bland (2014).
    if (exist('macularDensity','file'))
        macDenIsetbio(ii) = macularDensity(fieldSizesDegrees(ii));
    end
end
macFig = figure; clf; hold on
plot(fieldSizesDegrees,macDen,'ro','MarkerSize',8,'MarkerFaceColor','r');
plot(fieldSizesDegrees,macDen,'r');

% This is what the isetbio function returns.  Not clear it is really
% comparable in exact form.  Here multiplicatively scaled to have the
% same max as the CIE function, but not clear that is the right thing
% to do.  When you compare, this falls off faster than the CIE formula,
% but this makes a certain amount of sense since the CIE formula is trying
% to express an equivalent macular pigment density over a uniform field
% (but excluding the very center, which subjects are instructed to ignore
% in making large field color matches).
if (exist('macularDensity','file'))
    plot(fieldSizesDegrees,max(macDen(:))*macDenIsetbio/max(macDenIsetbio(:)),'bo','MarkerSize',8,'MarkerFaceColor','b');
    plot(fieldSizesDegrees,max(macDen(:))*macDenIsetbio/max(macDenIsetbio(:)),'b');
end

ylim([0 1]);
xlabel('Field Size (Degrees)');
ylabel('Macular Pigment Density (460 nm)');


%% Photopigment optical density
for ii = 1:length(fieldSizesDegrees)
    % This is the CIE formula, and presumably gives some sort of effective
    % macular pigment density over the field, with some sort of exclusion
    % of the center for 10 degrees.  I think this because presumbably these
    % values are chosen to make the color matching functions produced using
    % spatially uniform macular pigment density come out right.  I believe
    % this formula comes from Mooreland & Alexander (1997).
    [LPhotopigmentOpticaDensity(ii)] = PhotopigmentAxialDensity('LCone','Human','CIE',fieldSizesDegrees(ii));
    [SPhotopigmentOpticaDensity(ii)] = PhotopigmentAxialDensity('SCone','Human','CIE',fieldSizesDegrees(ii));
end
photopigFig = figure; clf; hold on
plot(fieldSizesDegrees,LPhotopigmentOpticaDensity,'ro','MarkerSize',8,'MarkerFaceColor','r');
plot(fieldSizesDegrees,SPhotopigmentOpticaDensity,'bo','MarkerSize',8,'MarkerFaceColor','b');
plot(fieldSizesDegrees,LPhotopigmentOpticaDensity,'r');
plot(fieldSizesDegrees,SPhotopigmentOpticaDensity,'b');

ylim([0 1]);
xlabel('Field Size (Degrees)');
ylabel('Photopigment Density');
legend({'L & M cones','S cones'},'Location','NorthWest');
