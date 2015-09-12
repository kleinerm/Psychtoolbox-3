function [Y,index] = Shuffle(X)
% [Y,index] = Shuffle(X)
%
% Randomly sorts X.
% If X is a vector, sorts all of X, so Y = X(index).
% If X is an m-by-n matrix, sorts each column of X, so
%	for j=1:n, Y(:,j)=X(index(:,j),j).
%
% Also see SORT, Sample, Randi, and RandSample.

% xx/xx/92  dhb  Wrote it.
% 10/25/93  dhb  Return index.
% 5/25/96   dgp  Made consistent with sort and "for i=Shuffle(1:10)"
% 6/29/96	  dgp  Edited comments above.
% 5/18/02   dhb  Modified code to do what comments say, for matrices.
% 6/2/02    dhb  Fixed bug introduced 5/18.
% 9/10/15   niki Add bindDim. so that this function can shuffle with the
% same order across the bindDim. It also work with higher dimension arrays.

%{
Example:
x = reshape(repmat(reshape(kron([1,1,1],1:4),4,3),5,1),4,5,3)
[y,ind]=Shuffle(x)
[y,ind]=Shuffle(x,[1])
[y,ind]=Shuffle(x,[2])
[y,ind]=Shuffle(x,[3])
x = reshape(repmat(reshape(kron(1:4,[1,1,1]),3,4),1,5),3,4,5)
[y,ind]=Shuffle(x,[1,3])
y2 = RandDim(x,2)

x = reshape(repmat(reshape(repmat(reshape(kron(1:4,[1,1,1]),3,4),1,5),3,4,5),1,2),3,4,5,2)
[y,ind]=Shuffle(x,[1,3])
%}

if nargin<2
    bindDim = [];
end

num_dim = ndims(X);
siz_unbind = size(X);
siz_unbind(bindDim) = 1;

sort_dim = find(siz_unbind>1,1);
permute_ind = [sort_dim, bindDim, setdiff(1:num_dim, [bindDim, sort_dim])];
X = permute(X, permute_ind); 

siz = size(X);
bind_ind = 2:(length(bindDim)+1);
unbind_ind = (length(bindDim)+2):num_dim;
num_bind = prod(siz(bind_ind));
num_unbind = prod(siz(unbind_ind));

rsp_siz = [siz(1), num_bind, num_unbind];
X = reshape(X, rsp_siz);
[~,index] = sort(rand([siz(1),1, num_unbind]));

index = repmat(index,1,num_bind) ...
    + repmat(0:siz(1):siz(1)*(num_bind-1), [siz(1),1,num_unbind])...
    + repmat(reshape(0:siz(1)*num_bind:siz(1)*num_bind*(num_unbind-1),1,1,num_unbind), siz(1), num_bind);

Y = X(index);
Y = permute(reshape(Y,siz), permute_ind);
index = permute(reshape(index,siz), permute_ind); 
