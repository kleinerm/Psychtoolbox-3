function unpackim = glmUnpackPixels( packim )

% glmUnpackPixels  Unpack an image read from the screen with glmGetPixels
% 
% usage:  unpackim = glmUnpackPixels( packim )

% 09-Dec-2005 -- created (RFM)

% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

% recover dimensions
mn=double(packim(end-3:end));
n=256*mn(1)+mn(2);
m=256*mn(3)+mn(4);
padn=mod( 4-mod(3*n,4) , 4 );

% reshape
im=reshape(packim(1:end-8),[ 3*n+padn m ]);
im=im(1:3*n,:);
im=cat(3,im(1:3:end,:),im(2:3:end,:),im(3:3:end,:));
unpackim=double(permute(flipdim(im,2),[ 2 1 3 ]));

return
