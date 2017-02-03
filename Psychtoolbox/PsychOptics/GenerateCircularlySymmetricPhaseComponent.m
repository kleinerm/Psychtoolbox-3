function circularlySymmetricPhase = GenerateCircularlySymmetricPhaseComponent(rows,cols)
%CIRCULARLYSYMMETRICPHASE  Create phase appropriate for circularly symmetric function.
%   circularlySymmetricPhase = GenerateCircularlySymmetricPhaseComponent(rows,cols) 
%
%   Create the frequency domain phase values that we need to insert before
%   the ifft2 to recover a real circularly symmetric function in the space
%   domain.  We don't understand why we need to do it, it's black magic
%   that works.  Or at least I don't.  Nicolas Cottaris figured this out
%   and may understand it better than I do.
%
%   See also LSFTOPSF

%   Passed row and colum dimension must be even.
if (mod(rows,2) ~= 0)
    error('Number of rows must be even');
end
if (mod(cols,2) ~= 0)
    error('Number of cols must be even');
end

% Nicolas figured out that this works.
x = 0:cols-1;
y = 0:rows-1;
[X,Y] = meshgrid(x,y);
circularlySymmetricPhase = ((-1).^(X+Y+1)+1)/2*pi;

end