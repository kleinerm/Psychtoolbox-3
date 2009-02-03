function bool = ellipse(a,b,horpow,verpow)
% ellipse(a) creates a circle with
% diameter == ceil(2*a)
%
% ellipse(a,b) creates an ellipse with
% horizontal axis == ceil(2*a) and vertical axis == ceil(2*b)
%   
% ellipse(a,b,power) generates a superellipse according to the
% geometric formula (x./a).^power + (y./b).^power < 1
%
% ellipse(a,b,horpow,verpow) generates a generalized superellipse according
% to the geometric formula(x./a).^horpow + (y./b).^verpow < 1
%
% For more info on superellipses, see
%   http://en.wikipedia.org/wiki/Superellipse
%
% ellipse returns a (tighly-fitting) boolean matrix which is true for all
% points on the surface of the ellipse and false elsewhere

% DN 2008
% DN 2009-02-02 Updated to do circles and input argument handling more
%               efficiently

error(nargchk(1, 4, nargin, 'struct'));
if nargin < 2
    b = a;
end
if nargin < 3
    horpow = 2;
end
if nargin < 4
    verpow = horpow;
end
    

a       = a + .5;                     % to produce a ellipse with horizontal axis == ceil(2*hor semi axis)
b       = b + .5;                     % to produce a ellipse with vertical axis == ceil(2*vert semi axis)

[x,y]   = meshgrid(-a:a,-b:b);

bool    = abs(x./a).^horpow + abs(y./b).^verpow  < 1;

% return in a tight-fitting matrix
cropcoords  = cropblackedges(bool);
bool        = bool(cropcoords(3):cropcoords(4),cropcoords(1):cropcoords(2));
