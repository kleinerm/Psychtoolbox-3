function y = MultiNormalDraw(n,u,K)
% x = MultiNormalDraw(n,u,K)
%
% Make n multivariate normal draws with mean u and covariance matrix K.
% Each draw is in a single column of y, which has n columns.
%
% The routine operates by computing the appropriate linear transformation
% of a N(0,I) multivariate normal draw.  This transformation is given by 
% y= C'x + u where K = C'C.  This works because the covariance of a
% distribution y = Cx is in general given by Ky = C Kx C'.  In our case 
% Kx= I so Ky = C'C = K.
%
% K = 0 is handled as a special case

% 12/31/93  dhb     Added this line.
% 7/24/04   awi     Cosmetic.


[m,null] = size(u);
x = zeros(m,n);
if (MatMax(K) == 0)
  C = zeros(m,m);
else
  C = chol(K);
end

for i = 1:n
  x(:,i) = NormalDraw(m,0,1);
  y(:,i) = C'*x(:,i) + u;
end


