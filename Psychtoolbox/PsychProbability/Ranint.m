function i = Ranint(dim,n)
% i = Ranint([dim],n)
%
% Choose "dim" random integers in the range 1:n.
%
% For historical reasons, the usage is a little weird.  If you omit the dim
% argument, then a single random integer is drawn from the range 1:n. 
% 
% Also see Randi.

% 4/10/94   dhb   Added optional first argument.
% 8/19/94   dhb   Clarified comment.
% 1/20/97   dhb   Delete obsolete rand('uniform').
% 4/12/99   dgp   Changed names of arguments to be consistent with Matlab and Randi.
% 7/24/04   awi   Cosmetic.

if nargin == 1
	n = dim;
	dim = 1;
end

list = rand(dim,1);
i = floor( n .* list ) + 1;

