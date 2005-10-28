function w=OrientationBandpass(siz,oLow,oHigh)
% w=orientationBandpass(size,oLow,oHigh) returns a "window", i.e. a matrix
% meant to be used as a band-pass filter. The matrix size is mxn if "size"
% is [m,n], and nxn if "size" is n. The matrix elements represent gain at
% each freq, uniformly spaced from about -1 to 1 of Nyquist frequency (see
% FREQSPACE). oLow and oHigh are the cut-off orientations in degrees (e.g.
% 0 to 360). The interval will include all angles from oLow up to but not
% including orientation oHigh. oHigh must be in the range [oLow,oLow+180].
% Setting oHigh=oLow+180 will produce an all-pass filter. The filter has
% gain 1 in the orientation intervals [oLow,oHigh) and
% [oLow+180,oHigh+180), and gain 0 outside those intervals. Note the
% asymmetry: oLow is included and oHigh is excluded. Frequency zero, which
% has no orientation, is always passed with gain 1. The logical OR
% of filters for several contiguous bands, e.g. 0 to 20 and 20 to 40 will
% equal the filter for the composite band, 0 to 40. oLow can be any finite
% value, but only the range 0 to 180 is unique. o+180 is equivalent to o.
% Here's a typical use, to produce orientation-bandpass noise:
% 	noise=randn(n,n);
% 	filter=OrientationBandpass(n,oLow,oHigh);		
% 	if any(any(filter~=1)) % skip all-pass filter
% 		ft=filter.*fftshift(fft2(noise));
% 		noise=real(ifft2(ifftshift(ft)));
% 	end
% Also see Bandpass2, Bandpass, FREQSPACE.

% 4/30/02 dgp wrote it, based on Bandpass2.
% 5/2/02  dgp Made interval bounds asymmetric, omitting oHigh.

if nargin~=3
	error('Usage: w=OrientationBandpass(size,oLow,oHigh)')
end
if length(siz)<1 | length(siz)>2
	error('"size" must be an integer or a pair of integers. Usage: w=OrientationBandpass(size,oLow,oHigh)')
end
if length(siz)==1
	siz=[siz siz];
end
if ~(isfinite(oLow) & isfinite(oHigh))
	error('oLow and oHigh must be finite')
end
if oHigh<oLow | oHigh>oLow+180
	error(sprintf('oHigh %g must be in the range [oLow,oLow+180]. oLow is %g.',oHigh,oLow));
end
% handle common special case quickly
if oHigh-oLow==180
	w=ones(siz);
	return
end
% call to meshgrid based on FREQSPACE.m
t1 = ((0:siz(2)-1)-floor(siz(2)/2))*(2/(siz(2)));
t2 = ((0:siz(1)-1)-floor(siz(1)/2))*(2/(siz(1)));
[t1,t2] = meshgrid(t1,t2);
a=atan2(t2,t1)*180/pi; % direction, -180 to 180 deg.
t1=t1.*t1+t2.*t2; % radial frequency squared
clear t2
a=mod(a,180); % orientation, 0 to 180 deg.
oLow=mod(oLow,180);
oHigh=mod(oHigh,180);
w=ones(size(t1));						% start with all-pass filter
if oHigh>=oLow
	d=find((a<oLow | a>=oHigh) & t1>0); % find out-of-band frequencies
else
	d=find((a<oLow & a>=oHigh) & t1>0); % find out-of-band frequencies
end
if ~isempty(d)
	w(d)=zeros(size(d));				% zero the gain at those frequencies
end
