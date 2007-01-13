function [keyIsDown,secs, keyCode] = KbCheck(deviceNumber)
% [keyIsDown, secs, keyCode] = KbCheck([deviceNumber])
% 
% Return keyboard status (keyIsDown), time (secs) of the status check, and
% keyboard scan code (keyCode).
% 
%    keyIsDown      1 if any key, including modifiers such as <shift>,
%                   <control> or <caps lock> is down. 0 otherwise.
% 
%    secs           time of keypress as returned by GetSecs.
% 
%    keyCode        OS9 & OSX: a 128-element logical array.  Each bit
%                   within the logical array represents one keyboard key. 
%                   If a key is pressed, its bit is set, othewise the bit 
%                   is clear. To convert a keyCode to a vector of key  
%                   numbers use FIND(keyCode). To find a key's keyNumber 
%                   use KbName or KbDemo.
% 
%                   WIN: a 256-element array. It seems that the first 
%                   128 elements correspond to the ascii sequence of the 
%                   characters, and may be consistent with the Mac key codes.
%
% KbCheck and KbWait determine whether any key is down now, including the
% meta keys: <caps lock>, <shift>, <command>, <control>, and <option>. The
% only key not reported is the start key (triangle) used to power on your
% computer.
% 
% Some users of Laptops experienced the problem of "stuck keys": Some keys
% are always reported as "down", so KbWait returns immediately and KbCheck
% always reports keyIsDown == 1. This is often due to special function keys.
% These keys or system functionality are assigned vendor specific
% key codes, e.g., the status of the Laptop lid (opened/closed) could be
% reported by some special keycode. Whenever the Laptop lid is open, this key
% will be reported as pressed. You can work around this problem by passing
% a list of keycodes to be ignored by KbCheck and KbWait. See
% "help DisableKeysForKbCheck" on how to do this.
%
% GetChar and CharAvail are character-oriented (and slow), whereas KbCheck
% and KbWait are keypress-oriented (and fast). If only a meta key was hit,
% KbCheck will return true, because a key was pressed, but CharAvail will
% return false, because no character was generated. See GetChar.
% 
% KbCheck and KbWait are MEX files, which take time to load when they're
% first called. They'll then stay loaded until you flush them (e.g. by
% changing directory or calling CLEAR MEX).
%
% OSX: ___________________________________________________________________
%
% KbCheck uses the PsychHID function, a general purpose function for
% reading from the Human Interface Device (HID) class of USB devices.
%
% KbCheck queries the first USB-HID keyboard device by default. Optionally
% you can pass in a 'deviceNumber' to query a different keyboard if multiple
% keyboards are connected to your machine.
% 
% Windows/Linux: __________________________________________________________
%
% KbCheck uses built-in helper functions of the Screen() mex file to query
% keyboard state.
% _________________________________________________________________________
% 
% See also: FlushEvents, KbName, KbDemo, KbWait, GetChar, CharAvail.

% TO DO:
%
%  - Mention that on USB systems there the USB bus is sampled at 100 Hz.
%  - We could augment this to to accept an optional keyboard device number. 

% 3/6/97  dhb  Wrote it.
% 8/2/97  dgp  Explain difference between key and character.
% 1/28/98 dgp  Explain CapsLock.
% 2/4/98  awi  Explain keyCode.
% 2/13/98 awi  Changed keyCode to logical array, pointers to KbDecode, KbExplore.
% 2/19/98 dgp  Shortened by moving some text to GetChar.m.
% 3/15/99 xmz  Added comment for Windows version.
% 6/23/00 awi  Added paragraph contrasting queuing of GetChar and KbCheck.
% 7/7/00  dgp  Cosmetic.
% 6/17/02 awi  ****** OS X-specific fork from the OS 9 version *******
%                Added conditional invocation of PsychHID on OSX
% 7/12/04 awi  Cosmetic.  Separted platform-specific help. Use IsOSX now. 
% 10/4/05 awi Note here cosmetic changes by dbp on unknown date between 7/12/04 and 10/4/05.  
% 10/24/06 mk Windows and Linux implementation: Use built-in helper code in Screen.
% 10/24/06 mk Add code for disabling "stuck keys".

% ptb_kbcheck_disabledKeys is a vector of keyboard scancodes. It allows
% to define keys which should never be reported as 'down', i.e. disabled
% keys. The vector is empty by default. If you have special quirky hardware,
% e.g., some Laptop keyboards, that reports some keys as 'always down', you
% can work around this 'stuck keys' by defining them in the ptb_kbcheck_disabledKeys
% vector.
global ptb_kbcheck_disabledKeys;

% Cache operating system type to speed up the code below:
persistent macosx;
if isempty(macosx)
   macosx = IsOSX;
end

if macosx
    if nargin==1
        [keyIsDown,secs, keyCode]= PsychHID('KbCheck', deviceNumber);
    elseif nargin == 0
        [keyIsDown,secs, keyCode]= PsychHID('KbCheck');
    elseif nargin > 1
        error('Too many arguments supplied to KbCheck'); 
    end
else
   % We use the built-in KbCheck facility of Screen on GNU/Linux and MS-Windows
   % for KbChecks until a PsychHID implementation is ready.
    [keyIsDown,secs, keyCode]= Screen('GetMouseHelper', -1);
end

% Any dead keys defined?
if ~isempty(ptb_kbcheck_disabledKeys)
   % Yes. Disable all dead keys - force them to 'not pressed':
   keyCode(ptb_kbcheck_disabledKeys)=0;
   % Reevaluate global key down state:
   keyIsDown = any(keyCode);
end

return;
