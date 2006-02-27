function secs = KbWait(deviceNumber)
% secs = KbWait([deviceNumber])
% 
% Waits until any key is down and returns the time in seconds. 
%
% GetChar and CharAvail are character oriented (and slow), whereas KbCheck
% and KbWait are keypress oriented (and fast).
%
% Using KbWait from the MATLAB command line: When you type "KbWait" at the
% prompt and hit the enter/return key to execute that command, then KbWait
% will detect the enter/return key press and return immediatly.  If you
% want to test KbWait from the command line, then try this:
%
%  WaitSecs(0.2);KbWait
%
% OS 9: ___________________________________________________________________
%
% Command-Period always causes an immediate exit.
%
% _________________________________________________________________________
%
% See also: KbCheck, GetChar, CharAvail, KbDemo.

% 3/6/97    dhb  Wrote it.
% 8/2/97    dgp  Explain difference between key and character. See KbCheck.
% 9/06/03   awi  ****** OS X-specific fork from the OS 9 version *******
%                  Added OS X conditional.   
% 7/12/04   awi  Cosmetic.  OS 9 Section. Uses IsOSX.
% 4/11/05   awi  Added to help note about testing kbWait from command line.
% 11/29/05  mk   Fixed really stupid bug: deviceNumber wasn't queried!
% 02/22/06  mk   Modified for Linux: Currently a hack.

if IsOSX
    if nargin==1
        secs= PsychHID('KbWait', deviceNumber);
    elseif nargin == 0
        secs= PsychHID('KbWait');
    elseif nargin > 1
        error('Too many arguments supplied to KbWait'); 
    end
end

if IsLinux
    % This is a hack: Until we have KbWait implemented via PsychHID, we use
    % Screen's built-in KbWait emulation:
    secs=Screen('GetMouseHelper', -2);
end;
