function SimpleTimingTest(withUpdateRunning)
t=zeros(1, 200 * 64);

% Switch to realtime scheduling:
Priority(9);

if withUpdateRunning > 0
    StartUpdateProcess;
end;
running = IsUpdateRunning
WaitSecs(2);

tstart = GetSecs;
tend = tstart + 62;
count = 1;
t(1)=GetSecs;
while t(count) < tend
    count = count + 1;
    t(count)=GetSecs;
    WaitSecs(0.005);
end;

Priority(0);

plot(t(2:count) - t(1:count-1));

return;
