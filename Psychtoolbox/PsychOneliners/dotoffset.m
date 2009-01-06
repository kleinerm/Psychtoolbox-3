% [dx,dy,dz,a,e] = dotoffset(r,d,ndots)
% computes DX, DY and DZ offsets to move a point in a direction specified
% by R. Output for NDOTS offsets are computed independently. Only azimuthal
% direction can be specified or both azimuth and elevation, in which case a
% number of different randomizations are possible (see third form of R
% below). D is the distance by with dots are to be offset and should be
% either scalar or have length NDOTS.
%
% Outputs A and E are the azimuth and elevation angles used by DOTOFFSET
% for each offset.
% 
% R can take three forms:
% - scalar: direction (in degrees) of offset in azimuth (0 is translation
%   in z direction, 90 in x direction towards 3 o'clock), used for all
%   outputted offsets
% - vector with length NDOTS: direction (in degrees) of offset in azimuth
%   specified in input for each outputted offset
% - 2x2 cellmatrix: the first row specifies azimuth direction, the second
%   the elevation angle. A number of different inputs for the azimuth and
%   the elevation row are possible and lead to the following outputs:
%   - first column is a vector with length NDOTS: direction (in degrees) of
%     offset specified for each outputted offset. Second column is ignored.
%   - first column is a vector with length other than NDOTS: for each
%     outputted offset, offset direction is randomly selected from the
%     values in the vector (see 'help randsel'). Second column is ignored.
%   - scalar in the first column and NaN in the second column (e.g.
%     {45,NaN;67,NaN} or {45,90;67,NaN}). Direction specified in the first
%     column is used for all outputted offsets.
%   - scalar in the first and second column (both non-NaN) (e.g.
%     {45,90;67,NaN} or {45,90;0,359}). For each offset, a random direction
%     is computed within specified limits. Angles are randomly computed
%     between lower (1st column) and upper (2nd column) boundaries (see
%     'help randlim').
%
% Function can also be used to compute points laying on (part of) a circle
% or sphere with radius D.

% 2008-07-29 DN  wrote it.
% 2008-08-06 DN  now included all possible inputs I could think of

function [dx,dy,dz,a,e] = dotoffset(r,d,ndots)

psychassert(length(d)==1 || length(d)==ndots,'Input D should be scalar or have length NDOTS');

if isscalar(r) && ~iscell(r)
    e   = 0;
    a   = r;
elseif isvector(r) && length(r)==ndots && ~iscell(r)
    a   = r(:).';   % assure column vector
    e   = 0;
elseif iscell(r) && size(r,1)==2 && size(r,2)==2
    % azimuth
    if isvector(r{1,1}) && length(r{1,1})==ndots
        % direction for each offset specified
        a   = r{1,1}(:).';   % assure column vector
    elseif isvector(r{1,1}) && length(r{1,1})>1
        % a set of directions to randomly draw from
        a   = randsel(r{1,1},ndots);
    elseif isnan(r{1,2}) || r{1,1}==r{1,2}
        % scalar, same direction for all offsets
        a   = r{1,1};
    else
        % randomly compute direction betwee lower and upper boundaries
        a   = randlim([1,ndots],r{1,1},r{1,2});
    end
    % elevation
    if isvector(r{2,1}) && length(r{2,1})==ndots
        e   = r{2,1}(:).';   % assure column vector
    elseif isvector(r{2,1}) && length(r{2,1})>1
        e   = randsel(r{2,1},ndots);
    elseif isnan(r{2,2}) || r{2,1}==r{2,2}
        e   = r{2,1};
    else
        e   = randlim([1,ndots],r{2,1},r{2,2});
    end
else
    error('Input not recognized: R must be scalar, a vector of length NDOTS or a 2x2 cellmatrix')
end

% a is azimuth angle, e is elevation angle
dx = d .* cosd(e) .* sind(a);
dy = d .* sind(e);
dz = d .* cosd(e) .* cosd(a);

if isscalar(dx)
    dx = dx*ones(1,ndots);
end
if isscalar(dy)
    dy = dy*ones(1,ndots);
end
if isscalar(dz)
    dz = dz*ones(1,ndots);
end
if isscalar(a) && nargout>=4
    a = a*ones(1,ndots);
end
if isscalar(e) && nargout>=5
    e = e*ones(1,ndots);
end
