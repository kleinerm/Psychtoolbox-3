function mMasked=MaskImageOut(m [,alphaIn])

% Accept an image matrix "m" and return "nMasked", holding the same image
% but with adjusted alpha values.  MaskImageOut sets full opacity
% for pixels with value zero and sets full transparency for pixels with
% non-zero value.
%
% If the optional alphaIn argument is specified then MaskImageIn sets
% zero pixels to alphaIn opacity instad of to full opacity, 255. 
%
% see also: SetImageAlpha, MaskImageIn, AlphaDemo. 

% HISTORY
%
% mm/dd/yy
%
% 1/28/05   awi  Wrote it.


if nargin<2
   alphaIn=255;
end
    
dims=size(m);
numDims=length(dims);
if numDims > 3
    error('The matrix argument must have no more than three dimensions');
    
if numDims==2
    mMasked=repmat(m, [1, 1, 3]);
else
    mMasekd=m;
end

sumPlanes=m(:,:,1) + m(:,:,2) + m(:,:,3)
isZero=sumPlanes == 0;
nMasked(:,:,4)=isNonZero * alphaIn;









