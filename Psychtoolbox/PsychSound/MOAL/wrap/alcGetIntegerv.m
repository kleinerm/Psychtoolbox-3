function data = alcGetIntegerv(enum)
% data = alcGetIntegerv(enum)
%
% Retrieve data buffer for given OpenAL ALC enum value.
%

% ---protected---
% ---skip---

if nargin~=1
    error('Invalid number of parameters.');
end

% Alloc a 256 element buffer. That should do.
data = zeros(1, 256, 'uint32');
moalcore('alcGetIntegerv', int32(enum), numel(data) * 4, data);

return;
