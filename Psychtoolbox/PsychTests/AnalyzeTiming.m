function AnalyzeTiming(fname)

global valids;
global res;
global deltas;
global vdeltas;
global vrawdeltas;
global skippedones;

% Load result file: Will create struct 'res' with data:
load(fname);
disp(res);

if ~isempty(findstr(res.OSName, 'OSX'))
    res.failFlag(res.measuredTime > 1e6) = -1;
    res.measuredTime(res.measuredTime > 1e6) = nan;
end

% Discard 1st frame by marking it as invalid:
res.failFlag(1) = -1;
valids = find(res.failFlag == 0);
%skippedones = find(abs(res.onsetFlipTime - res.predictedOnset) > 0.9 * res.ifi);
skippedones = find(abs(res.measuredTime - res.predictedOnset) > 0.75 * res.ifi);

skippedones = intersect(skippedones, valids);

fprintf('\n\n\n');
fprintf('Comments: %s\n', res.Comments);
fprintf('Number of skipped frames: %i\n', length(skippedones));
fprintf('Number of corrupted frames: %i\n', length(find(res.failFlag > 0)));
deltas = res.measuredTime - res.onsetFlipTime;
fprintf('Number of > 1.6 msecs disagreement meas vs. flip: %i\n', length(find(abs(deltas(valids)) > 0.0016)));
fprintf('Number of flip >= 0.66 ifi later than meas. (Excessive wakeup-delay): %i\n', length(find(deltas(valids) < -0.66 * res.ifi)));

% Only take deltas of valid frames into account:
vdeltas = 1000 * deltas(valids); %#ok<FNDSB>

fprintf('Valid high-precision frames: Mean difference %f msecs, stddev %f msecs, range %f msecs.\n', mean(vdeltas), std(vdeltas), range(vdeltas));
vrawdeltas = 1000 * (res.measuredTime(valids) - res.rawFlipTime(valids));
fprintf('Valid raw-precision  frames: Mean difference %f msecs, stddev %f msecs, range %f msecs.\n', mean(vrawdeltas), std(vrawdeltas), range(vrawdeltas));

fprintf('Scheduling / wakeup delay vbl onset (swap) to wakeup: Mean %f msecs, Max %f msecs.\n', 1000 * mean(res.rawFlipTime(valids) - res.vblFlipTime(valids)), 1000 * max(res.rawFlipTime(valids) - res.vblFlipTime(valids)));
if isfield(res, 'swapRequestSubmissionTime')
    fprintf('Headroom for swaprequest submission, predictedonset - swapreqtime: Mean %f msecs, Max %f msecs.\n', 1000 * mean(res.predictedOnset(valids) - res.swapRequestSubmissionTime(valids)), 1000 * max(res.predictedOnset(valids) - res.swapRequestSubmissionTime(valids)));
end

for i=1:10
    if ~isempty(find(res.waitFramesSched(valids) == i))
        skipcount(i) = length(find(res.waitFramesSched(skippedones) == i));
        skipratio(i) = skipcount(i) / length(find(res.waitFramesSched(valids) == i));
    else
        skipcount(i) = nan;
        skipratio(i) = nan;
    end
end

if length(skippedones) < 25
    skippedOnes = skippedones
end

fprintf('\n');
fprintf('Skipcount for 2 frames sched: %i, Skipcount for > 2 frame: %i,  Ratio: %f %%\n', skipcount(2), length(skippedones) - skipcount(2), 100 * skipcount(2) / length(skippedones));

if length(res.waitFramesSched) > 2*4972
    firstHalfSkips = length(find(skippedones <= 4972));
    secondHalfSkips = length(find(skippedones > 4972));
    fprintf('Skipped during regular intervals (1st half): %i\n', firstHalfSkips);
    fprintf('Skipped during randomized intervals (2nd half): %i\n', secondHalfSkips);
end

close all;

figure;
plot(skipratio);
title('Ratio of skipped frames vs. total frames per waitFramesSched:');
figure;
plot(skipcount);
title('Number of skipped frames per waitFramesSched:');

if isfield(res, 'TimeAtSwapBuffers')
    waitedswap = intersect(skippedones, find((res.swapRequestSubmissionTime - res.targetWhentime) < 0));
    %waitedswap = 1:9953;
    if ~isempty(waitedswap)
        figure ; plot(1000 * (res.TimeAtSwapBuffers(waitedswap) - res.targetWhentime(waitedswap)));
        title('Wakeup delay from timed wait (scheduling delay) before SwapBuffers() call: [msecs]:');
    end

    if ~isempty(skippedones)
        figure ; plot(1000 * (res.swapRequestSubmissionTime(skippedones) - res.targetWhentime(skippedones)));
        title('Invocation time of Screen(''Flip'') relative to when deadline (positive == delayed): [msecs]:');

        figure ; plot(1000 * (res.targetWhentime(skippedones) + 0.5 * res.ifi - res.TimeAtSwapBuffers(skippedones)));
        title('Headroom between VSYNC deadline and time of SwapBuffers (positive = good): [msecs]:');

        figure; scatter(skippedones,repmat(1,1,length(skippedones)))
        title('Placement of skipped frames: x-position == frameId');
        
        %figure ; plot(diff(res.targetWhentime(skippedones) + 0.5 * res.ifi));
        %title('VSYNC deadline of skipped frames: [secs]:');
    end 
end

% Plot distribution of raw flip timestamps wrt. high-precision timestamps:
figure;
hist(1000 * (res.rawFlipTime(2:end) - res.vblFlipTime(2:end)), 100)
title('Histogram: Delay of rawtimestamp wrt. real vbltime of bufferswap');
xlabel('Delay [msecs]');
ylabel('Count');


return;
