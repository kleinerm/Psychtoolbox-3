function string = GetEchoString(windowPtr, msg, x, y, textColor, bgColor)
% string = GetEchoString(window, msg, x, y, [textColor], [bgColor])
% 
% Get a string typed at the keyboard. Entry is terminated by
% <return> or <enter>.
%
% Typed characters are displayed in the window. The delete
% character is handled correctly. Useful for i/o in a Screen window.
%
% See also: GetNumber, GetString, GetEchoNumber
%
% 2/4/97    dhb       Wrote GetEchoNumber.
% 2/5/97    dhb       Accept <enter> as well as <cr>.
%           dhb       Allow string return as well.
% 3/3/97    dhb       Updated for new DrawText.  
% 3/15/97   dgp       Created GetEchoString based on dhb's GetEchoNumber.
% 3/20/97   dhb       Fixed bug in erase code, it wasn't updated for new
%                       initialization.
% 3/31/97   dhb       More fixes for same bug.
% 2/28/98   dgp       Use GetChar instead of obsolete GetKey. Use SWITCH and LENGTH.
% 3/27/98   dhb       Put an abs around char in switch.
% 12/26/08  yaosiang  Port GetEchoString from PTB-2 to PTB-3.
% 03/20/08  tsh       Added FlushEvents at the start and made bgColor and
%                     textcolor optional

if nargin < 6
    bgColor = [];
end

if nargin < 5
    textColor = [];
end

% Flush GetChar queue to remove stale characters:
FlushEvents('keyDown');

% Write the message
Screen('DrawText', windowPtr, msg, x, y, textColor, bgColor);
Screen('Flip', windowPtr, 0, 1);

string = '';
while true
    char = GetChar;
    switch (abs(char))
        case {13, 3, 10}
            % ctrl-C, enter, or return
            break;
        case 8
            % backspace
            if ~isempty(string)
                string = string(1:length(string)-1);
            end
        otherwise
            string = [string, char];
    end

    output = [msg, ' ', string];
    Screen('DrawText', windowPtr, output, x, y, textColor, bgColor);
    Screen('Flip', windowPtr);
end
