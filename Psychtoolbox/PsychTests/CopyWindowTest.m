function CopyWindowTest(uselegacy, sf, sd)
% CopyWindowTest([uselegacy=0][, sf=1][, sd=1])
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
%
% Since the year 2012, Psychtoolbox uses fast offscreen window support from
% the imaging pipeline by default, instead of the old legacy path, which was
% less robust, slower, less flexible and more limited in its functionality (e.g.,
% no support for floating point framebuffers, multi-sample anti-aliasing or
% 3D rendering with correct occlusion testing). The legacy path is only enabled
% if your graphics card + driver does not support fast offscreen windows, or if
% you manually force use of the legacy path by including the flag 2^24 as a
% ConsereVRAMSetting, e.g., Screen('Preference', 'ConserveVRAM', 2^24);
% See "help ConserveVRAMSettings", the section titled "kPsychDontAutoEnableImagingPipeline".
%
% The only sensible reason to use the legacy path is if your graphics drivers
% should have some bugs in handling of fast offscreen windows and a driver
% update doesn't fix the problem. This CopyWindowTest() script should expose
% such bugs. It also allows you to use test the legacy path with the optional
% flag 'uselegacy' set to 1.
%

% History:
% 01/30/06 mk Wrote it.
% 01/01/12 mk Update with info about fast path and how to opt-out of it.

AssertOpenGL;

if nargin < 1 || isempty(uselegacy)
   uselegacy = 0;
end

% Source rectangle scaling factor:
if nargin < 2 || isempty(sf)
   sf=1;
end;
sf

% Target rectangle scaling factor:
if nargin < 3 || isemtpy(sd)
   sd=1;
end;
sd

% Enable legacy Offscreen window support if requested:
ov = Screen('Preference', 'ConserveVRAM');
if uselegacy
   Screen('Preference', 'ConserveVRAM', bitor(ov, 2^24));
end

% Open onscreen window:
w=Screen('OpenWindow', 0, 0);

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
[woff1, srcRect] = Screen('OpenOffscreenWindow', w, [0 255 0], [0 0 100 100], [], [], 0);
srcRect=srcRect*sf;

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
woff2 = Screen('OpenOffscreenWindow', w, [], [], [], [], 0);
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
srcRect=[100 100 100 + 100*sf 100 + 100*sf];
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
woff3 = Screen('OpenOffscreenWindow', w, [], [], [], [], 0);

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

Screen('Preference', 'ConserveVRAM', ov);

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
