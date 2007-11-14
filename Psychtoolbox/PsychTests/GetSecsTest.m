function GetSecsTest(n)

if ~IsWin
    error('GetSecsTest is currently only supported on Microsoft Windows.');
end

if nargin < 1
    n = [];
end

if isempty(n)
    n = 10000;
end

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
fprintf('lag1 = %15.6f , lag%i = %15.6f\n', lag1, n, lagn);
ratio = (raw(n)-raw(1))/(ticks(n)-ticks(1))
return;
