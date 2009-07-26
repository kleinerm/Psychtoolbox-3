function [ ret ] = Speak(saytext, voice)
% Use speech output to speak a given text.
%
% Usage:
%
% [ ret ] = Speak(text [, voice ]);
%
% 'text' must be a text to speak. The optional 'voice' parameter allows
% to select among different system voices.
%
% The function returns an optional 'ret'urn code 0 on success, non-zero
% on failure to speak the requested text.
%
% Note: Speak is only supported on Mac OS/X.
%
% Examples:
% Say "Hello darling" with standard system voice:
% Speak('Hello darling');
%
% Say same text with voice named "Albert":
% Speak('Hello darling', 'Albert');
%

% History:
% 24.07.09 mk   Written.

if ~IsOSX
   error('Speech synthesis is currently only supported on Mac OS/X systems.');
end

if nargin < 1
   error('You must provide the text string to speak!');
end 

% Build command string for speech output and do a system() call:
if nargin > 1
   % Pass optional voice argument:
   ret = system(sprintf('say -v ''%s'' ''%s''', voice, saytext));
else
   % Only text, no voice spec:
   ret = system(sprintf('say ''%s''', saytext));
end

return;
