function lut = CreatePseudoGrayLUT
% lut = CreatePseudoGrayLUT
%
% Create and return a 3 rows by 4096 columns RGB8 uint8 color lookup table
% 'lut', suitable as lut for the generic LUT based luminance output
% formatter of the Psychtoolbox imaging pipeline. This is a helper function
% for PsychImaging(), usually not directly called by user code.
%
% This will setup the pipeline for output of luminance images with about
% 1786 different levels of perceived luminance on a standard 8 bits per
% color channel RGB framebuffer of a standard graphics card.
%
% Usage:
%
% At the beginning of your script, replace the standard ...
%
% win = Screen('OpenWindow', screenid, ...);
%
% ... call by the following commands:
%
% PsychImaging('PrepareConfiguration');
% PsychImaging('AddTask', 'General', 'EnablePseudoGrayOutput');
% win = PsychImaging('OpenWindow', screenid, ....);
%
%
% Further explanation:
%
% The technique to represent 10.7 bits (log2(1786)) of luminance on a 8 bit
% display is called "Pseudo Gray" or "Bit-Stealing". It adds small delta
% values to the different color channels to "tilt" the RGB color vector of
% each pixel a bit away from the "pure luminance' axis. This slight tilt,
% combined with the different emission characteristics of red, green and
% blue monitor phospor and the different sensitivity of the human eye for the
% three different colors, will create the perception (or illusion) of extra
% luminance values. So far the theory. In practice you'll need a well
% calibrated and gamma corrected color monitor for this to work, and there
% may be some smallish color artifacts in your stimuli.
%
% Anyway, this is the "poor man's solution" to high bitdepths luminance
% output, mostly here for illustration and demo purposes. If you need well
% controlled high quality high resolution luminance output, check out our
% different drivers for different high precision display devices like video
% attenuators, the VideoSwitcher, CRS Bits++ box and the ATI 10 bit
% framebuffer driver. See "help PsychImaging" for an overview and usage.
%
% The LUT encoding used here is based solely on the algorithm described at
% this webpage by Richard W. Franzen:
%
% <http://r0k.us/graphics/pseudoGrey.html>
%
% The webpage refers to multiple different sources of this type of
% algorithm, apparently the principle was described by multiple independent
% authors. There is also a reference to the "Bit stealing" technique by:
%
% Tyler C.W., Chan H., Liu L., McBride B. & Kontsevich L.L. (1992)
% "Bit-stealing: How to get 1786 or more grey levels from an 8-bit color
% monitor", Proc. SPIE #1666, pp 351-364.
%
% However, i haven't ever read that article, so i don't know if it proposes
% exactly the same procedure although readers of that article told me that
% it is "basically the same".
%

% History:
% 06/01/08  mk  Written.

% "Boost table" btable maps 4 LSB's to boost values for red, green and blue channel:
btable = uint8(zeros(3,16));
% These slots are boosted by one in red channel:
btable(1, 1+[5:8, 14:15]) = 1;
% These slots are boosted by one in green channel:
btable(2, 1+[9:15]) = 1;
% These slots are boosted by one in blue channel:
btable(3, 1+[2:4, 7:8, 11:13]) = 1;

% Allocate uint8 lut for 3 channels RGB, with 1786 different levels of
% pseudo-gray, but distributed over 4096 slots -- Oversampled to simplify
% lut generation in code below:
lut = uint8(zeros(3, 2^12));

% Initialize LUT with standard pseudo-gray encoding:
for i=0:length(lut)-1
    % Initialize slot 'i':
    
    % Compute 4 bit LSB index into table of color boost values:
    boostidx = mod(i,16) + 1;
    % Read from table:
    boost = btable(:, boostidx);
    
    % Base color value is just the 8 MSB in the normal 0-255 range:
    base = uint8(bitshift(i, -4));
    
    % Add boost value to each base value to get final value:
    lut(:, i+1) = uint8([base; base; base] + boost);
end

% Debug code: Compute number of unique color triplets, aka displayable
% levels of pseudo-gray in LUT: This comes out as the expected 1786 levels:
% dlut = double(lut);
% hashlut = dlut(1,:) + 256 * (dlut(2,:) + 256 * dlut(3,:));
% numuniquelevels = length(unique(hashlut))

% Done. Return the 'lut', which is ready for use with the generic
% attenuator driver of PsychImaging():
return;
