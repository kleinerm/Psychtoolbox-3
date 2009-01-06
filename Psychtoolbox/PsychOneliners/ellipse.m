function bool = ellipse(a,b,horpow,verpow)
% ellipse(a,b) creates an ellipse with
% horizontal axis == ceil(2*a) and vertical axis == ceil(2*b)
%   
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

% DN    2008

if nargin == 3
    verpow = horpow;
elseif nargin == 2
    horpow = 2;
    verpow = 2;
elseif nargin < 2 || nargin > 4
    error('ellipse: Expecting between two and four input arguments');
end
    

a       = a + .5;                     % to produce a ellipse with horizontal axis == ceil(2*hor semi axis)
b       = b + .5;                     % to produce a ellipse with vertical axis == ceil(2*vert semi axis)

[x,y]   = meshgrid(-a:a,-b:b);

bool    = (x./a).^horpow + (y./b).^verpow  < 1;

% return in a tight-fitting matrix
cropcoords  = CropBlackEdges(bool);
bool        = bool(cropcoords(3):cropcoords(4),cropcoords(1):cropcoords(2));