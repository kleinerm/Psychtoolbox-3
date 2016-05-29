function DatapixxGPUDitherpatternTest(fullscreen)
% DatapixxGPUDitherpatternTest([fullscreen=1])
%
% Low level diagnostic for GPU dithering via Datapixx.
%
% Steps through all 256 grayscale levels and uses
% Datapixx et al. scanline readback to check what
% the GPU actually outputs. Plots it and prints
% the first 10 pixels of the topmost output scanline.
%
% This is meant to facilitate low-level diagnosis
% of GPU dithering bugs if our regular test script
% BitsPlusIdentityClutTest.m reports dithering trouble
% and can't fix the problem automatically.
%
% 'fullscreen' Defaults to 1 for fullscreen windows.
% 0 = Use a windowed window at top of screen for
% display to also diagnose possible compositor
% interference.
%

% History:
% 13-Aug-2015  mk  Wrote it.

PsychDefaultSetup(0);
Datapixx('Open');

if nargin < 1 || isempty(fullscreen)
  fullscreen = 1;
end

screenid = max(Screen('Screens'));

if fullscreen
  rect = [];
else
  rect = [0, 0, Screen('WindowSize', screenid), 90];
end

w = Screen('Openwindow', screenid, 0, rect);
LoadIdentityClut(w);

even = zeros(1,256);
odd  = even;
trouble = 0;
fprintf('\n\n\n\n');

for i = 0:255
  Screen('FillRect', w, i);
  Screen('Flip', w);
  Datapixx('RegWrRdVideoSync');
  Datapixx('RegWrRdVideoSync');
  Datapixx('RegWrRdVideoSync');
  pixels = Datapixx('GetVideoLine');
  pixels = pixels(1,1:10);
  fprintf('Ref %i: ', i);
  fprintf('%i ', pixels);
  if (range(pixels) ~= 0) || (pixels(1,1) ~= i)
    fprintf(' --> TROUBLE!');
    trouble = trouble + 1;
  end
  fprintf('\n');
  even(i+1) = pixels(1,1);
  odd(i+1) = pixels(1, 2);
end

Datapixx('Close');
sca;

if trouble > 0
  fprintf('\n\nFAILURE! Many wrong pixels detected! Trouble for %i separate levels!!!\n', trouble);
  close all;
  plot(0:255, even, 'r', 0:255, odd, 'g');
else
  fprintf('\n\nALL GOOD :)\n');
end

return;
