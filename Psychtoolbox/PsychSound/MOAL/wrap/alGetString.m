function s = alGetString(enum)
% s = alGetString(enum)
%
% Retrieve string for given OpenAL enum value.
%

% ---protected---

if nargin~=1
    error('Invalid number of parameters.');
end

s=moalcore('alGetString', int32(enum));

return;
