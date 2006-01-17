function PerceptualVBLSyncTest(doublebuffer, stereomode, fullscreen)

AssertOpenGL;

if nargin < 1
   doublebuffer=1;
end;
doublebuffer=doublebuffer+1;

if nargin < 2
   stereomode=0;
end;

if nargin < 3
   fullscreen=1;
end;

try
   screen=max(Screen('Screens'));
   if fullscreen
      rect=[];
   else
      rect=[0,0,800,600];
   end;
   
   win=Screen('OpenWindow', screen, 0, rect, 32, doublebuffer, stereomode);
   color = 0;
   deadline = GetSecs + 10;
   
   while (~KbCheck) & (GetSecs < deadline)
      Screen('SelectStereoDrawBuffer', win, 0);
      Screen('FillRect', win, color);
      Screen('SelectStereoDrawBuffer', win, 1);
      Screen('FillRect', win, color);
      
      color=255 - color;
      
      if doublebuffer>1
         Screen('Flip', win);
      else
         pause(0.001);
      end;
   end;
   
   Screen('CloseAll');
   return;   
catch
   Screen('CloseAll');   
end;
