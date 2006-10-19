function out = EnforcePos(in)
%  out = EnforcePos(in)
%
% Enforce the constraint that spectral power must be
% positive. 
%
% A sophisticated algorithm might do something with small
% positive values as well.
%
% 10/23/93  dhb  Wrote it.

out = in;

index = find(in < 0);
if ~isempty(index)
    out(index) = zeros(length(index), 1);
end

