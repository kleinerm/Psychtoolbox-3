function image = CalFormatToImage(calFormat,n,m)
% image = CalFormatToImage(calFormat,n,m)
% 
% Convert a calibration format image back to a real
% image.
%
% See also ImageToCalFormat
%
% 8/04/04	dhb		Wrote it.
% 9/1/09    dhb     Update help.

k = size(calFormat,1);
image = reshape(calFormat',m,n,k);
