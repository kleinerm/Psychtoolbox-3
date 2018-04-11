function number = GetEchoNumber(window,msg,x,y,textColor,bgColor,varargin)
% number = GetEchoNumber(window, msg, x, y [, textColor][, bgColor][, deviceIndex][, untilTime=inf][, KbCheck args...])
%
% Get a number typed at the keyboard. Entry is terminated by <return> or
% <enter>. Typed characters are displayed on the screen. Useful for i/o in
% a Screen window. Equivalent to "number = str2num(GetEchoString(...))".
%
% Returns the empty matrix if no valid number is entered within the timeout
% period defined by the optional 'untilTime' deadline. Returns a column vector
% with multiple numbers if more than one number is entered, e.g., typing
% multiple numbers separated by a space or a comma.
%
% Typed characters are displayed in the window. The delete or backspace key
% is handled correctly, ie., it erases the last typed number.
%
% 'window' = Window to draw to. 'msg' = A message string displayed to
% prompt for input. 'x', 'y' = Start position of message prompt.
% 'textColor' = Color to use for drawing the text. 'bgColor' = Background
% color for text. By default, the background is transparent. If a non-empty
% 'bgColor' is specified it will be used. The current alpha blending
% setting will affect the appearance of the text if 'bgColor' is specified!
%
% Please note that if 'bgColor' is not specified, this means mistyped numbers
% can't be visually deleted/undone by use of the backspace key.
%
% This function uses GetKbChar() and thereby KbCheck() to get keyboard input.
% The optional  'deviceIndex' argument optionally allows to select the
% deviceIndex of the keyboard to use, and 'untilTime' allows to specify a
% response deadline. If the user doesn't press ENTER until 'untilTime', the
% function will time out and return with a empty 'number' as result. Further
% optional arguments will be passed on to the function GetKbChar().
%
% See also: GetNumber, GetString, GetEchoString, GetKbChar

%
% 2/4/97  dhb   Wrote it.
% 3/15/97 dgp   Replaced sscanf by str2num, which copes better with nonnumeric input,
%               returning an empty matrix instead of a null string.
% 3/15/97 dgp   Call GetEchoString instead of doing the work here.
% 3/18/97 dhb   Got rid of obsolete 's' interface.
% 10/22/10  mk  Switch to use of KbGetChar for keyboard input.
% 09/06/13  mk  Fix/Improve help text.
% 02/02/18  mk  Improve help text again.

if nargin < 6
    bgColor = [];
end

if nargin < 5
    textColor = [];
end

string = GetEchoString(window,msg,x,y,textColor,bgColor,1,varargin{:});
number = str2num(string); %#ok<ST2NM>

return;
