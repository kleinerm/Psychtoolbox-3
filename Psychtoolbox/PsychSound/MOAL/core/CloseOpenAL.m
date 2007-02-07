function CloseOpenAL
% CloseOpenAL - Shutdown OpenAL based audio subsystem.
%
% Use this at the end of your script in the cleanup- or error-handling
% routine.

% History:
% 7.2.2007 Written (MK).

moalcore('almClose');
return