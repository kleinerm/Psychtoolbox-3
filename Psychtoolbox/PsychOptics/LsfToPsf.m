function psf = LsfToPsf(lsf,varargin)
%LSFToPSF  Convert a line spread function to a point spread function.
%   psf = LSFTOPSF(lsf,varargin)
%
%   This works by taking the lsf into the one-dimensional frequency domain
%   (i.e. get the 1D MTF) and then creating a circularly symmetric version
%   of this.  The 2D frequency representation is then converted back to the
%   spatial domain to produce the psf.  This produces a spatially-symmetric
%   PSF consistent with the measured line spread function.
%
%   This method is described by Marchand, 1964, JOSA, 54, 7, pp. 915-919
%   and is one of several methods provided.  In 1964, taking the Fourier
%   transform was computationally intense. 
%
%   There is a second paper by Marchand (1965, JOSA, 55, 4, 352-354) which
%   treats the more general case where you have line spread functions for
%   many orientations and want to recover an psf that is not necessarily
%   spatially-symmetric.
%
%   The lsf must be spatially symmetric.  This makes sense given that we
%   are going to recover a spatially symmetric psf.
%
%   The lsf must be defined on a support with an even number of samples
%   that have the general feature that the zero position is at location
%   n/2 + 1, where n is the number of samples. This means that there is one
%   more negative position than positive position in the spatial support.
%   Doing it this way is important because of the way the FFT works. If you
%   don't you are likely to get complex values in the MTF despite the fact
%   that the LSF is real and symmetric.
%
%   Although this routine will still return something if the passed lsf
%   does not drop to zero at the outer edges of its support, it will not be
%   a reasonable answer in this case.
%
%   The returned psf is normalized to have unit volume.
%
%   See also PSFTOLSF

% Check for even support and symmetry
n1D = length(lsf);
if (mod(n1D,2) ~= 0)
    error('LSF must be defined on spatial support with even dimension.');
end
centerPosition = n1D/2 + 1;
negValues = lsf(centerPosition-1:-1:2);
posValues = lsf(centerPosition+1:end);
if (any(posValues ~= negValues))
    error('LSF is not spatially symmetric');
end
    
% Convert LSF to 1-D MTF
%
% Taking the absolute value
lsfNormalized = lsf/sum(lsf(:));
lsfMTFCenteredRaw = fftshift(fft(lsf));
lsfOTFCenteredImag = imag(lsfMTFCenteredRaw);
if (any(abs(lsfOTFCenteredImag > 1e-8)))
    error('Imaginary part of 1D MTF is bigger than makes sense from numerical error alone');
end
lsfOTFCentered = abs(lsfMTFCenteredRaw);

% Create symmetric 2D MTF from 1D MTF
%
% Values outside the frequency support of the lsf-based OTF are set to
% zero.  This should be fine as long as the spatial support is large enough
% so that the MTF has fallen to zero at highest frequencies specified.
radiusMatrix = MakeRadiusMat(n1D,n1D,centerPosition);
psfOTFCentered = interp1(0:n1D/2-1,lsfOTFCentered(centerPosition:end),radiusMatrix,'linear',0);

% Do inverse fft shift to put this into the format that iff2 wants
psfOTF = ifftshift(psfOTFCentered);

% Construct phase
%
% The generated OTF is constructed from the magnitude, so it zero phase everywhere.
% Generate the phase component for a circularly symmetric function with an
% even number of rows,cols.
%
% Honestly, we are a bit confused as to why this is necessary, but if we
% don't do it the ifft2 does not do what we expect and if we do, we get a
% psf that then reproduces our lsf when we go the other way.  If we were
% real signal processing mavens, we might understand.
circularlySymmetricPhase = GenerateCircularlySymmetricPhaseComponent(size(psfOTF,1), size(psfOTF,2));
psfOTFWithPhase = psfOTF .* exp(1i*circularlySymmetricPhase);

% Take ifft2 to get the PSF.
%
% This should be real 
psfRaw = ifft2(psfOTFWithPhase);
psfImag = imag(psfRaw);
if (any(abs(psfImag) > 1e-8))
    error('Imaginary component of derived psf too big for comfort.');
end
psf = abs(psfRaw);

% Normalize to unit volume
psf = psf/sum(psf(:));

end

