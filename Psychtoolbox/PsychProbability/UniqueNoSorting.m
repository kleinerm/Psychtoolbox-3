function oriunis = UniqueNoSorting(sequence,mode)
% output = UniqueNoSorting(sequence,mode)
%
% SEQUENCE is a vector, MODE is a specification of how you want your output
% to be not sorted. 
%
% Basically the same as unique, but returns the unique values either: 
% "in order of appearance"        - (MODE = 'first' for 'first occurrence')
% or
% "in order of disappearance"     - (MODE = 'last'  for 'last occurrence')
%
% MODE defaults to 'first'
%
% JvR 2008-05-06

if nargin == 1
    mode = 'first';
end

[inds,inds] = unique(sequence,mode);
inds        = sort(inds);
oriunis     = sequence(inds);

