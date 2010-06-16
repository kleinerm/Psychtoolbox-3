function XYZ = uvYToXYZ(uvY)
% XYZ = uvYToXYZ(uvY)
%
% Compute tristimulus coordinates from chromaticity and luminance.
%
% These are u',v' chromaticity coordinates in notation
% used by CIE.  See CIE Colorimetry 2004 publication, or Wyszecki
% and Stiles, 2cd, page 165.
%
% Note that there is an obsolete u,v chromaticity diagram that is similar
% but uses 6 in the numerator for u rather than the 9 that is used for u'.
% See CIE Coloimetry 2004, Appendix A, or Judd and Wyszecki, p. 296.
%
% See also XYZTouvY, XYZToxyY, xyYToXYZ.
%
%
% 10/31/94	dhb		Wrote it

[m,n] = size(uvY);
XYZ = zeros(m,n);
for i = 1:n
  XYZ(1,i) = (9/4)*uvY(3,i)*uvY(1,i)/uvY(2,i);
  XYZ(2,i) = uvY(3,i);
	denom = 9*uvY(3,i)/uvY(2,i);
  XYZ(3,i) = (denom - XYZ(1,i)-15*XYZ(2,i))/3;
end
