function xy = uvToxy(uv)
% xy = uvToxy(uv)
%
% Convert CIE u'v' chromaticity to CIE xy chromaticity.
%
% Note, these are u'v', not the obsolete 1960 uv.
%
% See also uvYToXYZ, XYZTouvY, xyTouv
%
% 7/15/03  dhb, bx  Wrote it.
% 3/17/04  dhb      Fixed typos.  This must not have been tested previously.
% 5/06/11  dhb      Improve comment.

uvY = [uv ; ones(1,size(uv,2))];
XYZ = uvYToXYZ(uvY);
xyY = XYZToxyY(XYZ);
xy = xyY(1:2,:);

