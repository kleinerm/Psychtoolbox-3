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

window = Screen(screen,'OpenWindow',0,[],32);

% THE FOLLOWING STEP IS IMPORTANT.
% make sure the graphics card LUT is set to a linear ramp
% (else the encoded data will not be recognised by Bits++).
% There is a bug with the underlying OpenGL function, hence the scaling 0 to 255/256.  
% This demo will not work using a default gamma table in the graphics card,
% or even if you set the gamma to 1.0, due to this bug.
% This is NOT a bug with Psychtoolbox!
Screen('LoadNormalizedGammaTable', window, linspace(0,(255/256),256)'*ones(1,3));

BitsPlusSetClut(window, zeros(256, 3));

% draw a black (?) background on front and back buffers to clear out any old LUTs
Screen('FillRect',window, 0);
Screen('Flip', window);
Screen('FillRect',window, 0);
Screen('Flip', window);

Screen('CloseAll');
