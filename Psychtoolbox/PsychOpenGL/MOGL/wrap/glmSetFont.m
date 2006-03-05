function glmSetFont( fname, fweight, fsize )

% glmSetFont  Set the font
% 
% usage:  glmSetFont( fname, fweight, fsize )

% 09-Dec-2005 -- created (RFM)

% ---protected---

if nargin<1,
    fname=22;
end
if nargin<2,
    fweight=1;
end
if nargin<3,
    fsize=50;
end

% valid name codes:    0-4, 16, 20-23, 98-99
%       kFontIDNewYork                = 2,
%       kFontIDGeneva                 = 3,
%       kFontIDMonaco                 = 4,
%       kFontIDTimes                  = 20,
%       kFontIDHelvetica              = 21,
%       kFontIDCourier                = 22,
%       kFontIDSymbol                 = 23,
%       kFontIDMobile                 = 24
%
% valid weight codes:  0=normal, 1=bold, 2=italic, 4=underline, 8=outline, 16=shadow
% valid sizes:         6 pixels and higher

moglcore('glmSetFont',fname,fweight,fsize);

return
