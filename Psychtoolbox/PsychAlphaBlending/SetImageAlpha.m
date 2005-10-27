function mAlpha=SetImageAlpha(m, alpha)

% Accept a matrix holding an image and set the alpha plane of the image
% to value "alpha". 
%

% HISTORY
%
% mm/dd/yy
%
% 1/28/05   awi  Wrote it.


dims=size(m);
numDims=length(dims);
if numDims > 3
    error('The matrix argument must have no more than three dimensions');
end    
if numDims==2
    mAlpha=repmat(m, [1, 1, 3]);
else
    mAlpha=m;
end
mAlpha(:,:,4)=alpha;


