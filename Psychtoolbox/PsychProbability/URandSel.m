function sel = URandSel(in,n)
% sel = RandSel(in,n)
%   randomly selects N elements from set IN, each element from IN will
%   be selected up to one time (so sampling without replacing).
%   IN can have any number of dimensions and elements

% 2008-08-27 DN Wrote it

psychassert(n<=numel(in),'More return elements requested (n: %d) than number of elements in input (n: %d)',n,numel(in));

selind  = NRandPerm(numel(in),n);
sel     = in(selind);
