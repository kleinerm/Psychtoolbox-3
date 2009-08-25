function uvY = XYZTouvY(XYZ)
% uvY = XYZTouvY(XYZ)
%
% Compute chromaticity and luminance from
% from tristimulus values.
%
% 10/31/94	dhb	Wrote it.
% 8/24/09   dhb Speed it up a lot by preallocating output.

% denom = XYZ(1,:) + 15*XYZ(2,:) + 3*XYZ(3,:);
% uvY = (diag([4 9 1])*XYZ)./denom([1 1 1]',:);

uvY = NaN*zeros(size(XYZ));
[m,n] = size(XYZ);
for i = 1:n
  denom = (XYZ(1,i) + 15*XYZ(2,i) + 3*XYZ(3,i));
  uvY(1,i) = 4*XYZ(1,i)/denom;
  uvY(2,i) = 9*XYZ(2,i)/denom;
  uvY(3,i) = XYZ(2,i);
end
