function pdf = ChiSquarePDF(x,df)
% pdf = ChiSquarePDF(x,df)
%
% Returns ChiSquare pdf of value x with df d.f. x may be a matrix. df must
% be a scalar.
%
% Based on Pocket Book of Integrals and Mathematical Functions Ronald J.
% Tallarida, CRC Press, 1991, p. 103
%
% Denis G. Pelli, 1993
%
% 5/19/96  dhb      Cosmetic changes.
% 7/24/04  awi      Cosmetic.


if (nargin ~= 2)
	error('Usage: pdf = ChiSquarePDF(x,df)');
end
if (df < 1)
	error('ChiSquarePDF: degrees of freedom must be at least 1');
end

pdf=exp(-x/2).*x.^((df-2)/2)/(2^(df/2)*gamma(df/2));
