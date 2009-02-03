function bool = circle(rad)
% cirbool = circle(rad)
%
% circle creates a circle with diameter == ceil(2*rad)
% Ellipse returns a (tighly-fitting) boolean matrix which is true for all
% points on the surface of the ellipse and false elsewhere

% DN 2008
% DN 2009-02-02 Turned into proxy for ellipse

bool = ellipse(rad);
