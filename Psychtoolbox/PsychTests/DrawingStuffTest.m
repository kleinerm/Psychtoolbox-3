% DrawingStuffTest
% 
% Exercises FrameRect, DrawLine,  FillPoly and FramePoly on OS X.  

% 7/24/03   awi     Wrote it. 

s=max(Screen('Screens'));
w=Screen('OpenWindow', s,[],[],[],[],[],0);
Screen('FillRect', w, 0);

% Test drawing some frame rects
r1=[0 0 100 100];
r2=OffsetRect(r1, 200,200);
r3=OffsetRect(r1, 250,250);
r4=OffsetRect(r1, 300,300);
Screen('FrameRect',w, [255 255 0], r2, 1);
Screen('FrameRect',w, [255 0 0], r3, 4);
Screen('FrameRect',w, [0 255 0], r4, 9);
Screen('FrameRect',w, [255 0 255], r4, 1);

% Test drawing a line
Screen('DrawLine', w, 255, 100,100, 400,400, 3);

% Test drawing filled and framed polygons.  Note that polygons must be convex.  This is a
% requirement made by GL.  Choosing vertices on the perimeter of a  circle
% is guaranteed to specify a convex polygon.  
polyCenterX=500;
polyCenterY=300;
numPoints=15;
polyRadius=50;
ClockRandSeed;
angles=sort(rand(1,numPoints) * 2 * pi);
polyPoints=round([ [cos(angles)*polyRadius+polyCenterX]', [sin(angles)*polyRadius+polyCenterY]']);
Screen('FillPoly', w, [0 0 255], polyPoints);
polyCenterY=150;
polyPoints=round([ [cos(angles)*polyRadius+polyCenterX]', [sin(angles)*polyRadius+polyCenterY]']);
Screen('FramePoly', w, [0 0 255], polyPoints,2);
Screen('Flip', w);
KbWait;
Screen('CloseAll');
