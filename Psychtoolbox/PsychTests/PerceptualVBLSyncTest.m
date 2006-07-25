function PerceptualVBLSyncTest(stereomode, fullscreen, doublebuffer)
% PerceptualVBLSyncTest(stereomode, fullscreen, doublebuffer)
%
% Perceptual synchronization test for synchronization of Screen('Flip') and
% Screen('WaitBlanking') to the vertical retrace.
%
% After starting this test, you should see a flickering greyish background
% that flickers in a homogenous way - without cracks or weird moving patterns
% in the flickering area. If you see an imhogenous flicker, this means that
% synchronization of stimulus onset to the vertical retrace doesn't work due
% to some serious bug or limitation of your graphics hardware or its driver.
% If you don't know what this means, you can test this script with parameter
% doublebuffer == 0 to artificially create a synchronization failure.
%
% On OS-X you should also see some emerging pattern of yellow horizontal lines.
% These lines should be tightly concentrated/clustered in the topmost area of
% the screen. Lots of yellow lines in the middle or bottom area or even
% randomly distributed lines indicate some bug in the driver of your graphics
% hardware. This is a common problem of all ATI graphics adapters on MacOS-X
% versions earlier than OS-X 10.4.3 when running a dual-display setup...
%
% A second reason for distributed yellow lines could be bad timing on your
% machine, e.g., due to background activity by virus scanners or the Spotlight
% indexing service on OS-X. Turn these off for conducting your studies!
%

% History:
% 01/28/06 mk Written. Replaces the built-in flickertest of Screen('OpenWindow')
%             on multi-display setups. That test wasn't well received :(

% Check for presence of OpenGL PTB.
AssertOpenGL;

if nargin < 3
   % Use double-buffered windows by default: Single-buffered ones can't sync to
   % retrace and are discouraged anyway. Setting doublebuffer=0 is an easy way
   % to reproduce the visual pattern created by a complete sync-failure though.
   doublebuffer=1;
end;
doublebuffer=doublebuffer+1;

if nargin < 1
   % Use non-stereo display by default. 
   stereomode=0;
end;

if nargin < 2
   fullscreen=1;
end;

try
   screen=max(Screen('Screens'));
   if fullscreen
      rect=[];
   else
      rect=[0,0,800,600];
   end;
   
   help PerceptualVBLSyncTest;
   fprintf('Press ENTER key to start the test. The test will stop after 10 seconds\n');
   fprintf('or any keypress...\n');
   pause;
   
   [win , winRect]=Screen('OpenWindow', screen, 0, rect, 32, doublebuffer, stereomode);
   flickerRect = InsetRect(winRect, 100, 0);
   color = 0;
   deadline = GetSecs + 10;
   beampos=0;
   
   while (~KbCheck) & (GetSecs < deadline)
      % Draw left eye view (if stereo enabled):
      Screen('SelectStereoDrawBuffer', win, 0);
      % Draw alternating black/white rectangle:
      Screen('FillRect', win, color, flickerRect);
      % If beamposition is available (on OS-X), visualize it via yellow horizontal line:
      if (beampos>=0) Screen('DrawLine', win, [255 255 0], 0, beampos, winRect(3), beampos); end;
      % Same for right-eye view...
      Screen('SelectStereoDrawBuffer', win, 1);
      Screen('FillRect', win, color, flickerRect);
      if (beampos>=0) Screen('DrawLine', win, [255 255 0], 0, beampos, winRect(3), beampos); end;
      
      % Alternate drawing color from white -> black, or black -> white
      color=255 - color;
      
      if doublebuffer>1
         % Flip buffer on next vertical retrace, query rasterbeam position on flip, if available:
         [VBLTimestamp, StimulusOnsetTime, FlipTimestamp, Missed, beampos] = Screen('Flip', win, 0, 2);
      	%beampos=400 + 100 * sin(GetSecs);   
      else
         % Just wait a bit in non-buffered case:
         pause(0.001);
      end;
   end;
   
   Screen('CloseAll');
   return;   
catch
   Screen('CloseAll');   
end;
