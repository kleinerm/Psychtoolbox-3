function GetSecsTest(n)

% Set default number of samples to 10000:
if nargin < 1
    n = [];
end

if isempty(n)
    n = 10000;
end

% Close all plot figure windows:
close all;

% Perform cold init of GetSecs:
GetSecs;

% Test 1: Increment of low-precision timeGetTime() timer:
% Expected outcome. All deltas between ticks() are either zero or one
% milliseconds, with very occassional outliers that are higher...
ticks = zeros(1,n);
raw = zeros(1,n);
cooked = zeros(1,n);
for i=1:n
    [cooked(i) ticks(i) raw(i)] = GetSecs(-1);
end

tickinc = diff(ticks);
plot(tickinc * 1000);
figure;
hist(tickinc * 1000);

% Test 2: Sample raw high-res time and raw low-res time, plot them against
% each other:
figure;
plot(ticks, raw);

figure;
plot((raw - ticks)*1000);
avglag = mean(raw-ticks)*1000
lag1 = (raw(1) - ticks(1)) * 1000;
lagn = (raw(n) - ticks(n)) * 1000;
ratio = (raw(n)-raw(1))/(ticks(n)-ticks(1))
fprintf('lag1 = %15.6f , lag%i = %15.6f, Ratio of elapsed time in high-res vs. low-res clock: %15.10f\n', lag1, n, lagn, ratio);

if abs(ratio - 1.0) > 0.05
    fprintf('Timers disagree by more than 5% --> This could indicate flaky timers!!\n');
end

% Test 3: Let CPU go to sleep for a few seconds and see if there's drift:
[cooked1 lowres1 highres1] = GetSecs(-1);
WaitSecs(10);
[cooked2 lowres2 highres2] = GetSecs(-1);

fprintf('Elapsed time should be 10 seconds: lowres timer says %f secs, highres timer says %f secs.\n', lowres2-lowres1, highres2-highres1);

% Test 4: Multicore hopping stress test:
told = GetSecs;
for i=1:n
    tnew = GetSecs(mod(i,2)+1);
    if tnew < told
        fprintf('Time warps (time going backwards!) detected when switching between CPU cores!!! (delta = %f secs)\n', tnew - told);
    end
    told = tnew;
end



return;
