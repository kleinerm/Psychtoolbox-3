function [calFormat,n,m] = ImageToCalFormat(image)
% [calFormat,n,m] = ImageToCalFormat(image)
%
% Take an m by n by 3 image and convert it
% to a format that may be used to Psychtoolbox
% calibration routines.
%
% 8/04/04	dhb		Wrote it.
% 7/16/07   dhb     Update help line.

[m,n,k] = size(image);
calFormat = squeeze(reshape(image,m*n,1,k))';
