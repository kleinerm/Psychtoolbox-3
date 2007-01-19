function reply = Ask(window,message,textColor,bgColor,replyFun,rectAlign1,rectAlign2)
% reply = Ask(window,message,[textColor],[bgColor],[replyFun],[rectAlign1],[rectAlign2])
%
% Draw the message, using textColor, right-justified in the upper right of
% the window, call reply=eval(replyFun), then erase (by drawing text again
% using bgColor) and return. The default "replyFun" is 'GetClicks'. You may
% want to use 'GetString', 'GetEchoString', or 'GetNumber'.
% 
% "rectAlign1" and "rectAlign2", if present, are applied after the above
% alignment. The values should be selected from: RectLeft, RectRight,
% RectTop, RectBottom.
% 
% Typical uses:
% reply=Ask(window,'Click when ready.');
% reply=Ask(window,'What''s your name?',[],[],'GetString');
% reply=Ask(window,'Who are you?',[],[],'GetEchoString',RectLeft,RectTop);
% 
% See also GetString, GetEchoString, GetNumber.

% 3/9/97  dgp	Wrote it, based on dhb's WaitForClick.m
% 3/19/00  dgp	Suggest turning off font smoothing. Default colors.
% 8/14/04  dgp	As suggested by Paul Thiem, added an example (and better argument checking) 
%               to make it clear that replyFun must be supplied as a string and rectAlign1 as a value.
% 8/14/04  dgp	Call Screen 'WindowToFront'.

if ~Screen(window, 'WindowKind')
	error('Invalid window')
end

% Make the look a bit nicer than the default.
Screen('TextFont', window, 'Times');
Screen('TextSize', window, 30);

% Create the box to hold the text that will be drawn on the screen.
screenRect = Screen('Rect', window);
[tbx, tby] = Screen('TextBounds', window, message);
width = tbx(3);
height = tbx(4);
r = [0 0 width+30 height];
r = AlignRect(r, screenRect, RectRight, RectTop);

if nargin > 6
	if ~isa(rectAlign2,'double')
		error('Ask: rectAlign2 must be a double, e.g. RectLeft.');
	end
	r = AlignRect(r,screenRect,rectAlign2);
end
if nargin > 5
	if ~isa(rectAlign1,'double')
		error('Ask: rectAlign1 must be a double, e.g. RectLeft.');
	end
	r = AlignRect(r, screenRect, rectAlign1);
end
if nargin > 4 
	if isempty(replyFun)
		replyFun = 'GetClicks';
	end
	if isa(replyFun, 'double')
		error('Ask: replyFun must be [] or a string, e.g. ''GetClicks''.');
	end
end
if nargin < 5
	replyFun = 'GetClicks';
end
if nargin < 4 || isempty(bgColor)
	bgColor = WhiteIndex(window);
end
if nargin < 3 || isempty(textColor)
	textColor = BlackIndex(window);
end

% Draw our message on the screen and wait for the reply.
Screen('DrawText', window, message, r(RectLeft), r(RectBottom), textColor);
Screen('Flip', window, 0, 1);
reply = eval(replyFun);

% Erase the text off the screen.
bounds = [0 0 width height];
bounds = AlignRect(bounds, r, RectBottom,RectLeft);
% Text may extend beyond the nominal bounds suggested 
% by start and end points and line spacing. So we extend our erasing box
% by a generous amount to make sure we erase all traces of what we drew.
bounds = InsetRect(bounds, -2*height, -height);
Screen('FillRect', window, bgColor, bounds);
Screen('Flip', window, 0, 1);
