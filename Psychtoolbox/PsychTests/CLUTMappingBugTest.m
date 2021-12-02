function CLUTMappingBugTest(CLUTMapLength)
% CLUTMappingBugTest([CLUTMapLength=[256, 2048]])
%
% CLUTMapLength = Length of CLUT to test. Reasonable values would be
% 256, ..., 2048. Values up to 8192 would work on modern GPU's, values
% up to 16384 might work on the latest gen hardware from 2017.
% If omitted, the test will run itself with CLUTMapLength 256 and 2048.
%
% This test is derived from CLUTMappingBug.m, written by Denis Pelli,
% which demonstrated the following bug in the May 2017 PTB release, which
% is now fixed, as this test should verify:
%
% This program demonstrates a bug in the software CLUT mapping that
% appeared in the May 2017 release of Psychtoolbox. It seems that the
% rounding rule has changed for conversion of the floating point color
% (argument to FillRect) to selection of the index in the software CLUT. My
% programs use the software CLUT. They were reliably displaying my stimuli.
% Since the new release, my stimuli are distorted as though my floating
% point colors are mapped through the wrong CLUT entries.
%
% MacBook Pro (Retina, 15-inch, Mid 2015). MATLAB 8.6.0.267246 (R2015b),
% Psychtoolbox 3.0.14 beta.
%
% This program loads the CLUT with a gray ramp, and then sets two CLUT
% entries (rectEntry and screenEntry) to green. It then uses FillRect to
% fill the whole screen with the color of screenEntry, and then a small
% rect with the color of rectEntry. If everything works, then both color
% values should display the same green. This works fine when the clut
% length is short (e.g. 256) and and the entry numbers are small (under
% 100). This fails when the clut length is large or the entry number is
% large. When the CLUT size is 2048, this fails even for entry 1. Very
% likely we're missing the right CLUT entry by just one, but that is a very
% obvious fail here because only two CLUT entries are green. Off by one is
% terrible for my psychophysical programs as well.
%
% If you set CLUTMapLength=256 and rectEntry=20, then everything works
% fine, and you get the same green over the whole screen, as you should. If
% you then set rectEntry=128, then the rect will be gray, not green. If you
% increase CLUTMapLength to 2048 then you'll get no green, as the rect
% and the screen background are black.
%
% This seems to be a problem in the rounding rule used to convert the
% floating point color argument of FillRect to an index in the CLUT.
%
% I am very surprised to find that the bug disappears if I turn on
% EnableNative10BitFramebuffer while running on my MacBook Pro. In my
% psychophysical software, I always have that on, yet I'm encountering the
% rounding bug. I'm not sure what might be different between this demo and
% my psychophysical software. I wrote this program to isolate the problem I
% was having in that software.
%
% denis.pelli@nyu.edu
% May 30, 2017

if nargin < 1 || isempty(CLUTMapLength)
    CLUTMappingBugTest(256);
    CLUTMappingBugTest(2048);
    return;
end

PsychDefaultSetup(0);

skip = Screen('Preference','SkipSyncTests', 2);

rectEntry = round(CLUTMapLength * 0.8);
screenEntry=1;
try
   maxEntry=CLUTMapLength-1;

   PsychImaging('PrepareConfiguration');
   % Need 32 bit floating point framebuffer for > 256 slots:
   if CLUTMapLength > 256
      PsychImaging('AddTask','General','FloatingPoint32Bit');
   end

   PsychImaging('AddTask','General','NormalizedHighresColorRange',1);
   PsychImaging('AddTask','AllViews','EnableCLUTMapping',CLUTMapLength,1); % clutSize, high res
   PsychImaging('AddTask','General','UseRetinaResolution');
   window=PsychImaging('OpenWindow',0,1.0);

   rect=[0 0 600 300];
   gamma=repmat(((0:maxEntry)/maxEntry)',1,3); % gray ramp
   gamma(1+screenEntry,:)=[0 1 0]; % green
   gamma(1+rectEntry,:)=[0 1 0]; % green
   Screen('LoadNormalizedGammaTable',window,gamma,2);
   Screen('FillRect',window,screenEntry/maxEntry);
   Screen('FillRect',window,rectEntry/maxEntry,rect);
   Screen('TextSize',window,36);
   Screen('DrawText',window, sprintf('LUT size %i: You should see green only. Click to quit.', CLUTMapLength),100,80,0,1,1);
   Screen('Flip',window);
   GetClicks;
   sca; % screen close all
   Screen('Preference','SkipSyncTests', skip);
catch
   sca; % screen close all
   Screen('Preference','SkipSyncTests', skip);
   psychrethrow(psychlasterror);
end
