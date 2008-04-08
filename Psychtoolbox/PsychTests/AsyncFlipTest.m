function AsyncFlipTest(screenid)

AssertOpenGL;

if IsWin
    error('Sorry, asynchronous display flipping is only supported on well designed operating systems.');
end

if nargin < 1
    screenid = [];
end

if isempty(screenid) 
    screenid(1) = max(Screen('Screens'));
    screenid(2) = max(Screen('Screens'));
end

w(1)=Screen('OpenWindow', screenid(1), 0);
%w(1)=Screen('OpenWindow', screenid(1), 0, [0 0 100 100]);
%w(2)=Screen('OpenWindow', screenid(1), 0, [101 0 201 100]);
cm=0.6;

i1 = Screen('GetFlipInterval', w(1)) * 0.5;
i2 = Screen('GetFlipInterval', w(1)) * 0.5;

%tv2=Screen('Flip', w(2));
tv1=Screen('Flip', w(1));

while ~KbCheck
    Screen('FillRect', w(1), [255*cm 0   0]);
 %   Screen('FillRect', w(2), [0   255*cm 0]);
    
    tbase=GetSecs
    Screen('AsyncFlipBegin', w(1), tv1); % + i1);
 %   Screen('AsyncFlipBegin', w(2), tv2 + i2);
    
 %   fprintf('Initiated flip for w1.... waiting\n');

    [tref1 vref1 cref1] = Screen('WaitUntilFlipCertain', w(1))
 
    tv1 = Screen('AsyncFlipEnd', w(1));
 %   tv2 = Screen('AsyncFlipEnd', w(2));
        
    tref1
    tv1
    delta = tv1 - tref1
    cm = 1-cm;
end

Screen('CloseAll');
return;
