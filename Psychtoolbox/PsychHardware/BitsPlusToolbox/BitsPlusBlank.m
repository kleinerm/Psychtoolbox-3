function BitsPlusBlank(screen)
% BitsPlusBlank(screen)
%
% Just zero the Bits++ clut to save the screen.
% Probably shouldn't be called when screen is open.
%
% 5/4/03  dhb  Wrote it.
% 9/20/03 dhb  Needs screen pointer.
% 18/4/05 ejw  Converted it to run with OSX version of Psychtoolbox

if (nargin ~= 1)
    error('USAGE: BitsPlusBlank(screen)');
end

window = Screen('OpenWindow',screen, 0, [], 32);

% THE FOLLOWING STEP IS IMPORTANT.
% make sure the graphics card LUT is set to a linear ramp
% (else the encoded data will not be recognised by Bits++).
Screen('LoadNormalizedGammaTable', window, linspace(0, 1, 256)'*ones(1, 3));

theClut = zeros(256, 3);
%theClut(:, 2) = 2^16-1; % (DEBUG) Turn the screen green.
BitsPlusSetClut(window, theClut);

% draw a black (?) background on front and back buffers to clear out any old LUTs
Screen('FillRect',window, 0);
Screen('Flip', window);
Screen('FillRect',window, 0);
Screen('Flip', window);

Screen('CloseAll');
