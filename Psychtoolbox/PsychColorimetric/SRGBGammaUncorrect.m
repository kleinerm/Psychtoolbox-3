function rgb = SRGBGammaCorrect(RGB)
% rgb = SRGBGammaCorrect(RGB)
%
% Undo standard sRGB gamma correction, taking [0-255] -> [0-1].
%
% Conversion as speciedi at:
% http://www.srgb.com/basicsofsrgb.htm
%
% 2/9/06	dhb				Wrote it.

% Apply sRGB gamma correction according to formulae
RGB = double(RGB);
rgb = RGB;
index = find(rgb < 11);
if (~isempty(index))
	rgb(index) = RGB(index)/(255*12.92);
end
index = find(rgb >= 11);
if (~isempty(index))
    rgb(index) = (((RGB(index)/255)+0.055)/1.055).^2.4;
end


