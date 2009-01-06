function mat = randdim(in,dim)
% mat = randdim(in,dim)
% randomizes dimension DIM of matrix IN
% IN can have any number of dimensions
%
% DN 2008 Wrote it

dimsize     = size(in,dim);

if dimsize==1 || dim>ndims(in)
    mat = in;
    return;
end

v(1:ndims(in))  = {':'};                % Create the index that will transform x. a(':') werkt net zo goed als a(:)
v{dim}          = randperm(dimsize);    % Throw in random order indices for the dim-to-be-randomized.
mat             = in(v{:});             % Use this indexing cell