function BeampositionTest(n, screenid, synced)

AssertOpenGL;

if nargin < 1 || isempty(n)
    n = 10000;
end

if nargin < 2 || isempty(screenid)
    screenid = max(Screen('Screens'));
end
fprintf('Testing screen #: %g\n',screenid);

if nargin < 3 || isempty(synced)
    synced = 0;
end

w=Screen('OpenWindow', screenid, 0);
Screen('Flip',w);
beampos = zeros(n, 1);
t = zeros(n, 1);
for i=1:n
    if synced
        Screen('Flip', w);
    end
    beampos(i) = Screen('GetWindowInfo', w, 1);
    t(i) = GetSecs;
end

while Screen('GetWindowInfo', w, 1) < 500; end;
ta=GetSecs;
while Screen('GetWindowInfo', w, 1) < 515; end;
te=GetSecs;

tel = (te - ta) * 1000

Screen('CloseAll');

figure;
subplot(2,2,1)
plot(beampos,'k-');
axis tight
title(['BeamPosition: Screen ' num2str(screenid)]); ylabel('Scanlines'); xlabel('Loop #')
subplot(2,2,2)
plot(diff(t)*1000,'k.');
axis tight
title('TimeStamp Deltas'); ylabel('Time (ms)'); xlabel('Loop #')
subplot(2,2,3)
hist(beampos, 100);
axis tight
title('Histogram of BeamPosition'); ylabel('Number'); xlabel('Scanline #')
subplot(2,2,4)
plot(t, beampos,'k-');
axis tight
title('BeamPosition against Timestamp'); ylabel('Scanlines'); xlabel('Time (s)')
return;
