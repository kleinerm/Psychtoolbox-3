function w=Bandpass2(n,fLow,fHigh)
% w=Bandpass2(size,fLow,fHigh) returns a "window", i.e. a matrix meant to be
% used as a band-pass filter. The matrix size is mxn if "size" is [m,n],
% and nxn if "size" is n. The matrix elements represent gain at each freq,
% uniformly spaced from about -1 to 1 of Nyquist frequency (see
% FREQSPACE). fLow and fHigh are the radial cut-off frequencies on this
% scale. The filter has gain 1 in the frequency interval [fLow,fHigh], and
% gain 0 outside it. Add EPS to create complementary filters that add to
% 1, e.g.
% 	Bandpass2(n,0,f)+Bandpass2(n,f+eps,1)==Bandpass2(n,0,1)
% For circular symmetry make fHigh<=1. Setting fLow=0 and fHigh=Inf will
% produce an all-pass filter. Here's a typical use, to produce bandpass
% noise:
% 	noise=randn(n,n);
% 	filter=Bandpass2(n,fLow/fNyquist,fHigh/fNyquist);		
% 	if any(any(filter~=1)) % skip all-pass filter
% 		ft=filter.*fftshift(fft2(noise));
% 		noise=real(ifft2(ifftshift(ft)));
% 	end
% Also see OrientationBandpass, Bandpass, FREQSPACE.

% 5/15/94 dgp wrote it
% 5/25/96 dgp polished code and comments
% 6/5/96 dgp replaced "!=" by "~=". Oops, c habits die hard. Thanks Nam!
% 7/3/96 jny & dgp replaced erroneous formula for freqs by FREQSPACE. 
%             This decreases the freq spacing from 2/(n-1) to 2/n and shifts
%             the location of zero frequency slightly (for n even).
% 7/10/96 dgp eliminated call to FREQSPACE since not everyone has the
%             Signal Processing or Image Processing Toolbox.
% 7/11/96 dgp allow n to be 1 or 2 dimensional
% 7/22/96 dgp handle trivial no-filtering case quickly.

if nargin~=3
	error('Usage: w=Bandpass2(n,fLow,fHigh)')
end
if any(n<2) | any(n~=floor(n))
	error('First arg ''n'' must be an integer greater than 1')
end
if length(n)==1
	n=[n n];
end
if fLow<0 | fHigh<0
	error('Radial frequencies can''t be negative')
end
% handle common special case quickly
if fLow==0 & fHigh>2^0.5
	w=ones(n);
	return
end
% call to meshgrid based on FREQSPACE.m
t1 = ((0:n(2)-1)-floor(n(2)/2))*(2/(n(2)));
t2 = ((0:n(1)-1)-floor(n(1)/2))*(2/(n(1)));
[t1,t2] = meshgrid(t1,t2);
t1=t1.*t1+t2.*t2; % radial frequency squared
clear t2
w=ones(size(t1));				% start with all-pass filter
d=find(t1<fLow.^2 | t1>fHigh.^2); % find out-of-band frequencies
if ~isempty(d)
	w(d)=zeros(size(d));		% zero the gain at those frequencies
end
