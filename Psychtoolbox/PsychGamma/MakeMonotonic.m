function output = MakeMonotonic(input)
% output = MakeMonotonic(input)
%
% Make input monotonically increasing.
%
% 3/1/99  dhb  Handle multiple columns.
% 8/03/07 dhb  Old routine just enforced non-decreasing.  Fixed to make strictly increasing.

[m,n] = size(input);

output = input;
for j = 1:n
	for i = 1:m-1
	  if (output(i,j) >= output(i+1,j))
	    output(i+1,j) = output(i,j)+eps;
	  end
	end
end

  
