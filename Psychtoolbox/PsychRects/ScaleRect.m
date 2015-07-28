function r = ScaleRect(r,horizontalFactor,verticalFactor)
% r = ScaleRect(r,horizontalFactor,verticalFactor)
%
% Scales a rect, by multiplying the left and right coordinates by the 
% horizontal scale factor and multiplying the top and bottom coordinates
% by the vertical scale factor.
% Input can be multiple rects, concatenated into a Mx4 matrix
%
% See also PsychRects/Contents, Expand, InsetRect.

% 5/27/96 dgp Denis Pelli.
% 7/10/96 dgp PsychRects.
% 8/5/96  dgp check rect size.
% 1/13/10 dcn Input can now be multiple rects, concatenated in row
%             direction.
% 7/26/15 dcn Further vectorized, horizontal- and verticalFactor can now
%             also be vectors.

if nargin~=3
	error('Usage: r=ScaleRect(r,horizontalFactor,verticalFactor)');
end

if size(r,2)~=4
	error('Wrong size rect argument. Usage: r=ScaleRect(r,horizontalFactor,verticalFactor)');
end

horizontalFactor = horizontalFactor(:);
verticalFactor = verticalFactor(:);
if size(r,1)==size(horizontalFactor,1) && size(r,1)==size(verticalFactor,1)
    r = r.*SetRect(horizontalFactor,verticalFactor,horizontalFactor,verticalFactor);
else
    szs = [size(r,1) size(horizontalFactor,1) size(verticalFactor,1)];
    maxsz = max(szs);
    if ~all(szs==1 | szs==maxsz)
        error('All input must either have one row or the same number of rows as the longest input')
    end
    horizontalFactor = repmat(horizontalFactor,maxsz/szs(2),1);
    verticalFactor   = repmat(verticalFactor,maxsz/szs(3),1);
    r  = repmat(r,maxsz/szs(1),1).*SetRect(horizontalFactor,verticalFactor,horizontalFactor,verticalFactor);
end
