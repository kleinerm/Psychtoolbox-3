function bool = circle(rad)
% cirbool = circle(rad)
%
% circle creates a circle with diameter == ceil(2*rad)
% Ellipse returns a (tighly-fitting) boolean matrix which is true for all
% points on the surface of the ellipse and false elsewhere

% DN    2008

rad     = rad + .5;                     % to produce a circle with diameter == ceil(2*rad)

[x,y]   = meshgrid(-rad:rad,-rad:rad);

bool    = x.^2+y.^2 < rad.^2;

% return in a tight-fitting matrix
cropcoords  = CropBlackEdges(bool);
bool        = bool(cropcoords(3):cropcoords(4),cropcoords(1):cropcoords(2));