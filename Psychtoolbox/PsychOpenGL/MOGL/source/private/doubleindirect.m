function b = doubleindirect( funcp )

% DOUBLEINDIRECT  Flag whether a function declaration uses double indirection
%                 in its input arguments
% 
% usage:  b = doubleindirect( funcp )

% 24-Jan-2005 -- created;  adapted from code in autocode.m (RFM)

b=0;
for i=1:numel(funcp.argin.args),
	if length(funcp.argin.args(i).type.stars)>=2,
		b=1;
		break
	end
end

return
