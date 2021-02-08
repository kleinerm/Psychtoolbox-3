function DatapixxGPUDitherpatternTest(fullscreen, useVulkan)
% DatapixxGPUDitherpatternTest([fullscreen=1][, useVulkan=0])
%
% Low level diagnostic for GPU dithering via a VPixx
% devices like Datapixx/ViewPixx/Propixx, or a CRS
% device like the Bits# - but *not* the original Bits+.
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
% 'useVulkan' Defaults to 0. If set to 1, use Vulkan display
% backend, instead of standard OpenGL backend.
%

% History:
% 13-Aug-2015  mk  Wrote it.
% 03-Jul-2016  mk  Add support for CRS Bits#.
% 01-Sep-2020  mk  Add support for Vulkan.

PsychDefaultSetup(0);

if nargin < 1 || isempty(fullscreen)
  fullscreen = 1;
end

if nargin < 2 || isempty(useVulkan)
    useVulkan = 0;
end

screenid = max(Screen('Screens'));

oldsynclevel = Screen('Preference', 'SkipSynctests', 1);

if fullscreen
  rect = [];
else
  rect = [0, 0, Screen('WindowSize', screenid), 90];
end

if BitsPlusPlus('OpenBits#')
  BitsPlusPlus('SwitchToStatusScreen');
  WaitSecs(4);
  bitssharp = 1;
else
  Datapixx('Open');
  bitssharp = 0;
end

if useVulkan
    PsychImaging('PrepareConfiguration');
    PsychImaging('AddTask', 'General', 'UseVulkanDisplay');
    w = PsychImaging('Openwindow', screenid, 0, rect);
else
    w = Screen('Openwindow', screenid, 0, rect);
end
LoadIdentityClut(w);
%lut = Screen('Readnormalizedgammatable', screenid);
%foo1 = isequal(lut(:,1), lut(:,2))
%foo2 = isequal(lut(:,1), lut(:,3))
%close all;
%plot(lut);

even = zeros(1,256);
odd  = even;
trouble = 0;
fprintf('\n\n\n\n');

KbReleaseWait(-1);

for i = 0:255
  Screen('FillRect', w, i);
  Screen('Flip', w);
  if bitssharp
    WaitSecs('YieldSecs', 3 * Screen('GetFlipInterval', w));
    % Readback the two topmost scanlines from Bits# :
    pixels = BitsPlusPlus('GetVideoLine', 256, 1);
    pixels = [pixels, BitsPlusPlus('GetVideoLine', 256, 2)]; %#ok<AGROW>
  else
    Datapixx('RegWrRdVideoSync');
    Datapixx('RegWrRdVideoSync');
    Datapixx('RegWrRdVideoSync');
    pixels = Datapixx('GetVideoLine');
  end
  
  fprintf('Ref %i: RGB ', i);
  fprintf('%i ', pixels(:,[1:10, floor(length(pixels)/2):floor(length(pixels)/2)+9]));
  if any(psychrange(pixels) ~= 0) || (pixels(1,1) ~= i)
    fprintf(' --> TROUBLE!');
    trouble = trouble + 1;
    if (pixels(1,1) ~= i)
        fprintf(' WRONG VALUE');
    else
        fprintf(' DITHERING OR CROSS CHANNEL/CROSS ROW MISMATCH:\n');
        disp(pixels(:,[1:10, floor(length(pixels)/2):floor(length(pixels)/2)+9]));
    end
  end
  fprintf('\n');
  even(i+1) = pixels(1, 1);
  odd(i+1) = pixels(1, 2);

  if KbCheck(-1)
      break;
  end
end

if bitssharp
  BitsPlusPlus('SwitchToBits++');
  BitsPlusPlus('Close');
else
  Datapixx('Close');
end

sca;

if trouble > 0
  fprintf('\n\nFAILURE! Many wrong pixels detected! Trouble for %i separate levels!!!\n', trouble);
  close all;
  plot(0:255, even, 'r', 0:255, odd, 'g');
elseif i < 255
  fprintf('\n\nEARLY USER ABORT BEFORE ALL 255 SAMPLES taken - INCONCLUSIVE!)\n');  
else
  fprintf('\n\nALL GOOD :)\n');
end

Screen('Preference', 'SkipSynctests', oldsynclevel);

return;
