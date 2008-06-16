function [keyIsDown,secs, keyCode, deltaSecs] = KbCheck(deviceNumber)
% [keyIsDown, secs, keyCode, deltaSecs] = KbCheck([deviceNumber])
% 
% Return keyboard status (keyIsDown), time (secs) of the status check, and
% keyboard scan code (keyCode).
% 
%    keyIsDown      1 if any key, including modifiers such as <shift>,
%                   <control> or <caps lock> is down. 0 otherwise.
% 
%    secs           Time of keypress as returned by GetSecs.
% 
%    keyCode        A 256-element logical array.  Each bit
%                   within the logical array represents one keyboard key. 
%                   If a key is pressed, its bit is set, othewise the bit 
%                   is clear. To convert a keyCode to a vector of key  
%                   numbers use FIND(keyCode). To find a key's keyNumber 
%                   use KbName or KbDemo.
% 
%    deltaSecs      Time in seconds since this KbCheck query and the most
%                   recent previous query (if any). This value is in some
%                   sense a confidence interval, e.g., for reaction time
%                   measurements. If KbCheck returns the information that a
%                   key is pressed by the subject, then the subject could
%                   have pressed the key down anytime between this
%                   invocation of KbCheck at time 'secs' and the most
%                   recent previous invocation. Therefore, 'deltaSecs'
%                   tells you about the interval in which depression of the
%                   key(s) might have happened: [secs - deltaSecs; secs].
%                   for practical purpose this means that "measured" RT's
%                   can't be more accurate than 'deltaSecs' seconds - the
%                   interval between the two most recent keyboard checks.
%                   Please note however, that standard computer keyboards
%                   can incur additional delays and timing uncertainty of
%                   up to 50 msecs, so the real uncertainty can be higher
%                   than 'deltaSecs' -- 'deltaSecs' is just a lower bound!
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
% Keys pressed by the subject often show up in the Matlab command window as
% well, cluttering that window with useless character junk. You can prevent
% this from happening by disabling keyboard input to Matlab: Add a
% ListenChar(2); command at the beginning of your script and a
% ListenChar(0); to the end of your script to enable/disable transmission of
% keypresses to Matlab. If your script should abort and your keyboard is
% dead, press CTRL+C to reenable keyboard input -- It is the same as
% ListenChar(0). See 'help ListenChar' for more info.
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
% KbCheck queries the first USB-HID keyboard device by default. Optionally,
% when multiple keyboards are attached to your machine, you can pass in a
% 'deviceNumber':  When 'deviceNumber' is -1, KbCheck will query all
% keyboard devices and return their "merged state" - The 'keyCode' vector
% will represent the state of all keys of all keyboards, and the
% 'keyIsDown' flag will be equal to one if at least one key on any of the
% keyboards is pressed. When 'deviceNumber' is greater than 0, it will
% query only the specified HID keyboard device corresponding to that
% 'deviceNumber'. The function GetKeyboardIndices() allows to query the
% device numbers of all attached keyboards, or keyboards matching specific
% criteria.
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
% 6/13/08 abl Option for OS X to poll all keyboard devices by passing deviceNumber == -1, \
%             based on kas's modification of KbWait

% ptb_kbcheck_disabledKeys is a vector of keyboard scancodes. It allows
% to define keys which should never be reported as 'down', i.e. disabled
% keys. The vector is empty by default. If you have special quirky hardware,
% e.g., some Laptop keyboards, that reports some keys as 'always down', you
% can work around this 'stuck keys' by defining them in the ptb_kbcheck_disabledKeys
% vector.
global ptb_kbcheck_disabledKeys;

% Store timestamp of previous KbCheck:
persistent oldSecs;

% Cache operating system type to speed up the code below:
persistent macosx;
% ...and all keyboard indices as well:
persistent kbs;

if isempty(macosx)
    % First time invocation: Query and cache type of OS:
    macosx = IsOSX;
    
    % Set initial oldSecs to minus infinity: No such query before...
    oldSecs = -inf;
    
    % Query indices of all attached keyboards, in case we need'em:
    if macosx
        kbs=GetKeyboardIndices;
    end
end

if macosx
    if nargin==1
        if deviceNumber==-1
            % Query all attached keyboards
            keyIsDown=0; keyCode=zeros(1,256);  % preallocate these variables
            for i=kbs
                [DeviceKeyIsDown, secs, DeviceKeyCode]= PsychHID('KbCheck', i);
                keyIsDown = keyIsDown | DeviceKeyIsDown;
                keyCode = keyCode | DeviceKeyCode;
            end
        else
            % Query a specific keyboard device #
            [keyIsDown, secs, keyCode]= PsychHID('KbCheck', deviceNumber);
        end
    elseif nargin == 0
        % Query primary keyboard:
        [keyIsDown, secs, keyCode]= PsychHID('KbCheck');
    elseif nargin > 1
        error('Too many arguments supplied to KbCheck'); 
    end
else
   % We use the built-in KbCheck facility of Screen on GNU/Linux and MS-Windows
   % for KbChecks until a PsychHID implementation is ready.
    [keyIsDown,secs, keyCode]= Screen('GetMouseHelper', -1);
end

% Compute time delta since previous keyboard query, and update internal
% cached value:
deltaSecs = secs - oldSecs;
oldSecs = secs;

% Any dead keys defined?
if ~isempty(ptb_kbcheck_disabledKeys)
   % Yes. Disable all dead keys - force them to 'not pressed':
   keyCode(ptb_kbcheck_disabledKeys)=0;
   % Reevaluate global key down state:
   keyIsDown = any(keyCode);
end

return;
