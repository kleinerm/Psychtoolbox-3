function w=Bandpass(n,fLow,fHigh)
% w=Bandpass(n,fLow,fHigh) returns a 1xn window, a band-pass filter. The
% elements represent gain at each freq, uniformly spaced from -1 to 1 of
% Nyquist frequency. fLow and fHigh are the positive cut-off frequencies
% on this scale. We pass frequencies in the interval [fLow,fHigh). The
% asymmetric use of bounds guarantees that complementary filters will add
% to 1: Bandpass(n,0,f)+Bandpass(n,f,Inf)==ones(1,n). If fHigh is omitted
% it's assumed to be Inf. Also see Bandpass2.

% 1994   dgp       Wrote it
% 7/3/96 jny & dgp Replaced erroneous formula for freqs by FREQSPACE. 

if nargin<2 || nargin>3
	error('Usage: w=Bandpass(n,fLow,[fHigh])')
end
if nargin<3
	fHigh=Inf;
end;
if n<2 | n~=floor(n)
	error('First arg ''n'' must be an integer greater than 1')
end
if fLow<0 | fHigh<0
	error('Frequencies can''t be negative')
end
if rem(n,2) % copied from FREQSPACE
	t=-1+1/n:2/n:1-1/n;
else
	t=-1    :2/n:1-2/n;
end
t=abs(t);
w=ones(size(t));			% start with all-pass filter
d=find(t<fLow | t>=fHigh);	% find out-of-band frequencies
if ~isempty(d)
	w(d)=zeros(size(d));	% zero the gain at those frequencies
end
