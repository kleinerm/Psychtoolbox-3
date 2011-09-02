function TextureSharingTest
% TextureSharingTest -- Test OpenGL context resource sharing.
%
% Create a texture for one onscreen window. Open a 2nd one.
% Draw texture in both. Should work. If not -> GPU driver bug.

w = Screen('openwindow', 0, [255 0 0], [0 0 400 400]);
t = Screen('MakeTexture', w, uint8(rand(300,300,1) * 255))
Screen('DrawTexture', w, t);
Screen('flip', w)
w2=Screen('Openwindow', 0, [0 255 0], [800 0 1200 400])
Screen('DrawTexture', w2, t);
Screen('Flip', w2)
KbStrokeWait;
sca;

return;
