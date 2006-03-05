function packim = glmPackPixels( im )

% glmPackPixels  Pack an image for writing to the screen with glmPutPixels
% 
% usage:  packim = glmPackPixels( im )

% 09-Dec-2005 -- created (RFM)

% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

% convert greylevel to rgb
if size(im,3)==1,
    im=repmat(im,[ 1 1 3 ]);
end

packim=moglcore('glmPackPixels',im);

return
