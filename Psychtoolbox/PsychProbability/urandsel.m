function sel = urandsel(in,n)
% sel = randsel(in,n)
%   randomly selects N elements from set IN, elements are replenished.
%   IN can have any number of dimensions and elements

% 2008-08-27 DN Wrote it

psychassert(n<=numel(in),'More return elements requested (n: %d) than number of elements in input (n: %d)',n,numel(in));

selind  = nrandperm(numel(in),n);
sel     = in(selind);
