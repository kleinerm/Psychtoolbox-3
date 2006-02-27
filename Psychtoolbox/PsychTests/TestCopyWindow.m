function TestCopyWindow(sf, sd)
% TestCopyWindow(sf, sd)
%
% Basic correctness test of different path's of Screen('CopyWindow')
% Copies a small rectangular region from one window into another window.
% Each source region is copied/replicated 25 times into a 5 by 5 matrix.
%
% Tested paths are:
% Offscreen -> Onscreen : This is the most often used (and fastest) path.
% Offscreen -> Offscreen: From an offscreen window to a different offscreen window.
% Onscreen  -> Onscreen : From one area of onscreen window to a different area.
% Onscreen  -> Offscreen: From an area of onscreen window into a offscreen window.
%
% The current CopyWindow implementation has a couple of restrictions:
% * One can't copy from an offscreen window into the -same- offscreen window.
% * One can't copy from an onscreen window into a -different- onscreen window.
% * Sizes of sourceRect and targetRect need to match for Onscreen->Offscreen copy.
% The optional [copyMode] argument is accepted but ignored.
%
% Result of these test need to be checked via visual inspection and common sense.

% History:
% 01/30/06 mk Wrote it.

AssertOpenGL;

% Source rectangle scaling factor:
if nargin < 1
   sf=1;
end;
sf

% Target rectangle scaling factor:
if nargin < 2
   sd=1;
end;
sd

w=Screen('OpenWindow', 0, 0, [], 32, 2);
% Clear to black background:
Screen('FillRect', w, 0);
x=200;
y=200;

%Screen('Textfont', w, 'Courier New');
%Screen('TextSize', w, 30);
%Screen('TextStyle', w, 1);
Screen('DrawText', w, 'Rect drawn to onscreen window at topleft=100,100:', 0, 0, [255 255 255]);

% Draw initial rect to onscreen window:
DrawRect(w, 100, 100);
% Show it:
Screen('Flip', w);
% Wait
WaitKey;
Screen('Flip', w);

% Draw same rect into an offscreen window:
[woff1, srcRect] = Screen('OpenOffscreenWindow', w, [0 255 0], [0 0 101 101]);
srcRect=srcRect*sf
DrawRect(woff1, 0, 1);
%Screen('Textfont', woff1, 'Courier New');
%Screen('TextSize', woff1, 60);
%Screen('TextStyle', woff1, 1);
Screen('DrawText', woff1, ';-)', 0, 0, [255 255 0]);

% Test offscreen -> onscreen copy:
% Replicate image across the full screen:
Screen('DrawText', w, 'Rect copied from offscreen to onscreen window from top-left to bottom-right', 0, 0, [0 255 0]);
for yi=1:5
   for xi=1:5
      Screen('CopyWindow', woff1, w, srcRect, [1 + xi*100, 1 + yi*100, 101*sd + xi*100, 101*sd + yi*100]);
   end;
end;
Screen('Flip',w);
WaitKey;
Screen('Flip',w);

% Test offscreen -> offscreen copy:
woff2 = Screen('OpenOffscreenWindow', w);
Screen('FillRect', woff2, 0);
%Screen('Textfont', woff2, 'Courier New');
%Screen('TextSize', woff2, 18);

% Replicate image across the full offscreen-window:
for yi=1:5
   for xi=1:5
      Screen('CopyWindow', woff1, woff2, srcRect,  [1 + xi*100, 1 + yi*100, 101*sd + xi*100, 101*sd + yi*100]);
   end;
end;
% Show content of offscreen window 2 by copying it to onscreen window:
Screen('CopyWindow', woff2, w);
Screen('DrawText', w, 'Rect copied from offscreen-window 1 to full-size offscreen-window2 from top-left to bottom-right', 0, 0, [255 255 255]);
Screen('Flip',w,0,2);
WaitKey;
Screen('Flip',w,0,0);

% Test onscreen -> onscreen copy:
% Replicate image across the full screen:
DrawRect(w, 100, 100);
srcRect=[100 100 100 + 100*sf 100 + 100*sf]
Screen('DrawText', w, 'Rect copied from onscreen to onscreen window from top-left to bottom-right', 0, 0);
for yi=1:5
   for xi=1:5
      Screen('CopyWindow', w, w, srcRect,  [1 + xi*100, 1 + yi*100, 101*sd + xi*100, 101*sd + yi*100]);
   end;
end;
Screen('Flip',w);
WaitKey;
Screen('Flip',w);

% Test onscreen -> offscreen copy:
% Replicate image across the full screen:
DrawRect(w, 100, 100);
woff3 = Screen('OpenOffscreenWindow', w, [], []);

for yi=1:5
   for xi=1:5
      Screen('CopyWindow', w, woff3, srcRect,  [0 + xi*100, 0 + yi*100, 100*sd + xi*100, 100*sd + yi*100]);
   end;
end;
% Show the new offscreen window:
Screen('Flip',w);
Screen('CopyWindow', woff3, w);
Screen('DrawText', w, 'Rect copied from onscreen to offscreen window from top-left to bottom-right', 0, 0, 0);
Screen('Flip',w);
WaitKey;


Screen('CloseAll');
return;

function DrawRect(win, x, y)
	% Draw filled rect:
	Screen('FillRect', win, [0 0 255], [x y x+100 y+100]);
	Screen('FrameRect', win, [255 0 0], [x y x+100 y+100]);
return;

function WaitKey()
	while KbCheck
	end;
	KbWait;
return;