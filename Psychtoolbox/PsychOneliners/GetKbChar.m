function [ch, when] = GetKbChar(varargin)
% [ch, when] = GetKbChar([optional KbCheck arguments...]);
%
% GetKbChar reimplements basic functionality of GetChar() by use of KbCheck
% and KbPressWait. It accepts optionally all arguments that KbCheck accepts
% and passes those arguments to KbCheck, e.g., a keyboard index in order to
% only query a specific keyboard for input.
%
% The function only recognizes standard alpha-numeric keys, i.e., letters
% and numbers, and a few special symbols like the ones on top of the
% numeric keys. It only recognizes the delete, space and return keys as
% special function keys, not other keys like Function keys, CTRL, ALT or
% cursor keys. It always assumes a US keyboard mapping.
%
% It polls the keyboard, so may miss very brief keystrokes and doesn't use
% the keyboard queue.
%
% Use this function if you need a GetChar like interface for simple string
% and number input in situations where GetChar doesn't work reliably, e.g.,
% on some Octave configurations, with Matlab in -nojvm mode or on
% MS-Windows Vista or Windows-7.
%

% History:
% 22.10.2010  mk  Wrote it.

persistent shiftkeys;

if isempty(shiftkeys)
    shiftkeys = [KbName('LeftShift'), KbName('RightShift')];
end

% Get keypress, KbCheck style:
when = KbPressWait(varargin{:});
keycode = zeros(1,256);
down = 1;
while down
    [down, secs, keyincode] = KbCheck(varargin{:});
    if down
        keycode = keycode + keyincode;
        WaitSecs('YieldSecs', 0.001);
    end
end

% Force keycode to 0 or 1:
keycode(keycode > 0) = 1;

% Shift pressed?
if any(keycode(shiftkeys))
    shift = 2;
else
    shift = 1;
end

% Remove shift keys:
keycode(shiftkeys) = 0;

% Translate to ascii style:
ch = KbName(keycode);

% If multiple keys pressed, only use 1st one:
if iscell(ch)
    ch = ch{1};
end

% Decode 1st or 2nd char, depending if shift key was pressed:
if length(ch) == 1
    if shift > 1 && ismember(ch, 'abcdefghijklmnopqrstuvwxyz')
        ch = upper(ch);
    end
elseif length(ch) == 2
    ch = ch(shift);
elseif length(ch) > 2
    if strcmpi(ch, 'Return')
        ch = char(13);
    end
    if strcmpi(ch, 'space')
        ch = char(32);
    end
    if strcmpi(ch, 'DELETE')
        ch = char(8);
    end
end

% Done.
return;
