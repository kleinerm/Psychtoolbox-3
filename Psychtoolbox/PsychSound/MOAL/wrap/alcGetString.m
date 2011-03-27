function s = alcGetString(enum)
% s = alcGetString(enum)
%
% Retrieve string for given OpenAL ALC enum value.
%

% ---protected---

if nargin~=1
    error('Invalid number of parameters.');
end

s=moalcore('alcGetString', int32(enum));

return;
