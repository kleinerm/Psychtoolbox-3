function AsyncFlipTest2(screenid)

AssertOpenGL;

if nargin < 1
    screenid = [];
end

if isempty(screenid) 
    screenid(1) = max(Screen('Screens'));
    screenid(2) = max(Screen('Screens'));
end

w(1)=Screen('OpenWindow', screenid(1), 0, [], [], [], [], [], kPsychNeedFastBackingStore);
cm=0.8;

t=[];
t(end+1)=Screen('Flip', w(1));

while ~KbCheck

    % Wait for flip to complete, collect timestamps:
    if 0
        tv1=0;
        while tv1 == 0
            tv1 = Screen('AsyncFlipCheckEnd', w(1));
            WaitSecs('YieldSecs', 0.001);
        end
    else
        tv1 = Screen('AsyncFlipEnd', w(1));
    end

    cm = 1 - cm;
    
    % Draw something:
    Screen('FillRect', w(1), [255*cm 0   0]);
    
    t(end+1) = tv1;

    % Schedule new flip:
    Screen('AsyncFlipBegin', w(1), tv1+0.005); % + i1);
end

KbReleaseWait;

Screen('Flip', w(1));
Screen('FillRect', w(1), [0 255 0]);
Screen('Flip', w(1), 0, 2);
Screen('FillRect', w(1), [0 0 0]);
Screen('Flip', w(1), 0, 2);

woff = Screen('Openoffscreenwindow', w(1), 128, [0 0 500 100]);
while ~KbCheck
    %Screen('AsyncFlipEnd', w(1));
    % Draw something:
    cm = 1 - cm;
    Screen('FillRect', woff, [0, 255*cm, 0]);
    DrawFormattedText(woff, 'Hallo :-)', 'center', 'center', [255 255 0]);
    Screen('DrawTexture', w(1), woff);
    Screen('Drawingfinished', w(1), 2);
    %img = Screen('GetImage', woff);
    t(end+1) = Screen('AsyncFlipBegin', w(1), t(end) + 0.018, [2]);
end

% Final flip to avoid shutdown warning:
t(end+1) = Screen('Flip', w(1));
Screen('CloseAll');

close all ; plot(diff(1000 * t))
%figure; imshow(img);
