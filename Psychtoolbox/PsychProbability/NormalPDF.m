function prob = NormalPDF(x,u,theVar)
% prob = NormalPDF(x,u,theVar)
%
% Computes the p(x) for every entry of the list x. The result is a list of
% probabilities.

% 7/24/04  awi  Added this line.

[m,n] = size(x);
uvec = u .* ones(m,n);
z = (x-uvec) ./ sqrt(theVar);
prob = (1/sqrt(2*pi*theVar))*exp(- (z.^2)/2 );
