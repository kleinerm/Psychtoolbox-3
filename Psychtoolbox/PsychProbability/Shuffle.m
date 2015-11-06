function [Y,index] = Shuffle(X, bindDim)
% [Y,index] = Shuffle(X [, bindDim])
%
% Randomly sorts X.
%
% If X is a vector, sorts all of X, so Y = X(index).
% If X is an m-by-n matrix, sorts each column of X, so
%	for j=1:n, Y(:,j)=X(index(:,j),j).
%
% The optional 'bindDim' parameter allows this function to shuffle with the
% same order across the bindDim. It also work with higher dimension arrays.
% for example, if you have an n by m matrix X and hope shuffle each column
% with same random order (Shuffle the rows), rather than shuffle each
% column independently, you can run Shuffle(X, 2). This function also works
% on higher dimension arrays. say a 3-d array, If you bind the 2nd
% dimension, it will shuffle the rows on each page independently. If I bind
% the 2nd and 3rd dimension, then it will shuffle the layer of the 3-d
% array.
%
% Examples:
%
% create a 2-d array:
%   x = repmat((1:4)',1,5)
%
% Shuffle each column of x independently:
%   y1 = Shuffle(x)
%
% Shuffle columns of x with same order:
%   y2 = Shuffle(x,2)
%
% Create a 3-d array (4 by 5 by 3), each column contains a vector [1:4]'
%   x = reshape(repmat(reshape(kron([1,1,1],1:4),4,3),5,1),4,5,3)
%
% Shuffle each column independently on each page:
%   [y,ind]=Shuffle(x)
%
% Shuffle 1*5 rows independently on each page:
%   [y,ind]=Shuffle(x,[2])
%
% Shuffle 1*1*3 rows independently on each column:
%   [y,ind]=Shuffle(x,[3])
%
% Create a 3-d array (3 by 4 by 5), each row contains a vector [1:4]
%   x = reshape(repmat(reshape(kron(1:4,[1,1,1]),3,4),1,5),3,4,5)
%
% Shuffle 3*1*5 page along the 2nd dimension:
%   [y,ind]=Shuffle(x,[1,3])
%
% This is same as:
%   y2 = RandDim(x,2)
%   x = reshape(repmat(reshape(repmat(reshape(kron(1:4,[1,1,1]),3,4),1,5),3,4,5),1,2),3,4,5,2)
%   [y,ind]=Shuffle(x,[1,3])
%
% Also see SORT, Sample, Randi, RandDim, and RandSample.

% xx/xx/92  dhb  Wrote it.
% 10/25/93  dhb  Return index.
% 05/25/96  dgp  Made consistent with sort and "for i=Shuffle(1:10)"
% 06/29/96  dgp  Edited comments above.
% 05/18/02  dhb  Modified code to do what comments say, for matrices.
% 06/2/02   dhb  Fixed bug introduced 5/18.
% 9/10/15   niki Add bindDim. so that this function can shuffle with the same
%                order across the bindDim.

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

 
