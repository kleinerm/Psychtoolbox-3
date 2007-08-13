function nrslots = CountSlotsInGLOperator(gloperator)
% nrslots = CountSlotsInGLOperator(gloperator)
%
% Returns number of processing slots (== individual processing operations)
% in the given GL image processing operator 'gloperator'.

% History:
% 08/11/07 Written (MK).

if nargin < 1 || isempty(gloperator) || Screen('WindowKind', gloperator)~=4
    error('You must provide the handle of a valid GL imaging operator ''gloperator''!');
end

nrslots = 0;

while(1)
    if Screen('Hookfunction', gloperator, 'Query', 'UserDefinedBlit', nrslots)~=-1
       nrslots = nrslots + 1;
    else
        break;
    end
end

return;
