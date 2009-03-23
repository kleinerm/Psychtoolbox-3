function rc = alcASASetListener(eproperty, data)
% rc = alcASASetListener(property, data);
%
% Interface to function ALEnum alcAsaSetListener(ALuint property, (ALvoid*) data, ALuint size);

% 23-Mar-2009 -- created (MK)

% ---protected---

global ALC; %#ok<NUSED>

if nargin < 2
    error('Insufficient number of parameters.');
end

% Define size and type of input 'data':
switch(eproperty)
    case { ALC.ASA_REVERB_ON, ALC.ASA_REVERB_QUALITY, ALC.ASA_REVERB_ROOM_TYPE }
        data = int32(data);
        nbytes = 4;
    case { ALC.ASA_REVERB_PRESET }
        data = char(data);
        nbytes = length(data);
    otherwise
        data = single(data);
        nbytes = 4;
end

rc = moalcore('alcASASetListener', eproperty, data, nbytes);
return;
