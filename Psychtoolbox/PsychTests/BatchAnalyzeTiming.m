function BatchAnalyzeTiming(sdir)
    global res;

    if nargin < 1
        sdir = [];
    end
    
    if isempty(sdir)
        sdir = pwd;
    end
    
    % Recursively get filenames of all result files in 'sdir' directory tree:
    [fdir, fnames] = subdir(sdir);

    [gnrTotal, gnrSkipped, gnrCorrupt, gnrDisagree, gnrDelayed, meanDiff, stdDiff, rawmeanDiff, rawstdDiff, rawrangeDiff] = deal(0);
    sc = 0;
    rangeDiff = zeros(3,0);
    rawrangeDiff = zeros(3,0);
    
    % Check each for valid and matching datafile:
    for fi = 1:length(fnames)
        fname = char(fnames{fi});

        % Only interested in Priority 1 runs with sync-flip and medium load:
%        if ~isempty(strfind(fname, 'Res_flipconfig_prio1_syncflip')) && ~isempty(strfind(fname,'_medium_')) %&& ~isempty(strfind(fname,'noload'))
        if ~isempty(strfind(fname, 'Res_flipconfig_'))

            % Load result file: Will create struct 'res' with data:
            load(fname);
            
            if (res.measurementType == 'p') && res.VBLTimestampingMode == 1 && isfield(res, 'onsetFlipTime') && res.onsetFlipTime(end) > 0
            %if (res.measurementType == 'd') && res.VBLTimestampingMode == 1 && isfield(res, 'onsetFlipTime') && res.onsetFlipTime(end) > 0
                sc = sc + 1;
                fprintf('%s\n', fname);
                [nrTotal, nrSkipped, nrCorrupt, nrDisagree, nrDelayed, meanDiff(sc), stdDiff(sc), rangeDiff(:, sc), rawmeanDiff(sc), rawstdDiff(sc), rawrangeDiff(:, sc)] = AnalyzeThis;

                gnrTotal = gnrTotal + nrTotal;
                gnrSkipped = gnrSkipped + nrSkipped;
                gnrCorrupt = gnrCorrupt + nrCorrupt;
                gnrDisagree = gnrDisagree + nrDisagree;
                gnrDelayed = gnrDelayed + nrDelayed;
            end
        end
    end
    
    fprintf('\n\n\n');
    fprintf('Total     frames: %i\n', gnrTotal);
    fprintf('Skipped   frames: %i\n', gnrSkipped);
    fprintf('Corrupted frames: %i\n', gnrCorrupt);
    fprintf('Disagreed frames: %i\n', gnrDisagree);
    fprintf('Delayed   frames: %i\n', gnrDelayed);
    
    close all;
    %     figure;
    %     hold on;
    %     errorbar(meanDiff, rangeDiff(1,:),'b*');
    %     title('Mean disagreement photodiode - PrecisionFlipOnsettime:');
    %     xlabel('Session Nr.');
    %     ylabel('Milliseconds');
    %
    %     errorbar(rawmeanDiff, rawrangeDiff(1,:),'r*');
    %     title('Mean disagreement photodiode - RawFlipOnsettime:');
    %     xlabel('Session Nr.');
    %     ylabel('Milliseconds');
    %     hold off;

    if sc > 0
        figure;
        hold on;
        errorbar(1:sc, meanDiff, meanDiff - rangeDiff(2,:), rangeDiff(3,:) - meanDiff,'b*');
        title('Mean disagreement photodiode - Screen(''Flip'') Time: [blue = high precision, red = raw]');
        xlabel('Session Nr.');
        ylabel('Milliseconds');
        errorbar(1:sc, rawmeanDiff, rawmeanDiff - rawrangeDiff(2,:), rawrangeDiff(3,:) - rawmeanDiff,'r*');
        hold off;
        
        fprintf('Typical difference: %f msecs, Typical stddev: %f msecs, Typical range: %f msecs\n', mean(meanDiff), mean(stdDiff), mean(rangeDiff(1,:)))
    else
        fprintf('No measurement data available for analysis!\n');
    end
    
    return;
end

function [nrTotal, nrSkipped, nrCorrupt, nrDisagree, nrDelayed, meanDiff, stdDiff, rangeDiff, rawmeanDiff, rawstdDiff, rawrangeDiff] = AnalyzeThis
    global valids;
    global res;
    global deltas;
    global vdeltas;
    global vrawdeltas;
    global skippedones;

    
    agrthresh = 0.0016;
    
    % Discard 1st frame by marking it as invalid:
    res.failFlag(1) = -1;
    valids = find(res.failFlag == 0);
%    skippedones = find(abs(res.onsetFlipTime - res.predictedOnset) > 0.75 * res.ifi);
    skippedones = find(abs(res.measuredTime - res.predictedOnset) > 0.75 * res.ifi);
    skippedones = intersect(skippedones, valids);
    nrSkipped = length(skippedones);
    nrCorrupt = length(find(res.failFlag > 0));
    deltas = res.measuredTime - res.onsetFlipTime;
    nrDisagree = length(find(abs(deltas(valids)) > agrthresh));
    nrDelayed = length(find(deltas(valids) < -0.66 * res.ifi));

    agreevalids = intersect(valids, find(abs(deltas) <= agrthresh));
    
    % Only take deltas of valid frames into account:
    vdeltas = 1000 * deltas(agreevalids); %#ok<FNDSB>
    meanDiff  = mean(vdeltas);
    stdDiff   = std(vdeltas);
    rangeDiff(1) = range(vdeltas);
    rangeDiff(2) = min(vdeltas);
    rangeDiff(3) = max(vdeltas);

    vrawdeltas = 1000 * (res.measuredTime(agreevalids) - res.rawFlipTime(agreevalids));
    rawmeanDiff  = mean(vrawdeltas);
    rawstdDiff   = std(vrawdeltas);
    rawrangeDiff(1) = range(vrawdeltas);
    rawrangeDiff(2) = min(vrawdeltas);
    rawrangeDiff(3) = max(vrawdeltas);
    
    nrTotal = max(find(res.onsetFlipTime > 0)); %#ok<MXFND>
    
    fprintf('Comments: %s\n', res.Comments);
    if (nrCorrupt > 0) || (nrDisagree > 0) || (nrSkipped > 0)
        fprintf('Number of skipped frames: %i\n', nrSkipped);
        fprintf('Number of corrupted frames: %i\n', nrCorrupt);
        fprintf('Number of > 1.6 msecs disagreement meas vs. flip: %i\n', nrDisagree);
        fprintf('Number of flip >= 0.66 ifi later than meas. (Excessive wakeup-delay): %i\n', nrDelayed);
    end
    fprintf('Valid high-precision frames: Mean difference %f msecs, stddev %f msecs, range %f msecs.\n', meanDiff, stdDiff, rangeDiff(1));
    fprintf('Valid raw-precision  frames: Mean difference %f msecs, stddev %f msecs, range %f msecs.\n', rawmeanDiff, rawstdDiff, rawrangeDiff(1));

    %     fprintf('Scheduling / wakeup delay vbl onset (swap) to wakeup: Mean %f msecs, Max %f msecs.\n', 1000 * mean(res.rawFlipTime(valids) - res.vblFlipTime(valids)), 1000 * max(res.rawFlipTime(valids) - res.vblFlipTime(valids)));
    %     if isfield(res, 'swapRequestSubmissionTime')
    %         fprintf('Headroom for swaprequest submission, predictedonset - swapreqtime: Mean %f msecs, Max %f msecs.\n', 1000 * mean(res.predictedOnset(valids) - res.swapRequestSubmissionTime(valids)), 1000 * max(res.predictedOnset(valids) - res.swapRequestSubmissionTime(valids)));
    %     end
    %
        for i=1:10
            if ~isempty(find(res.waitFramesSched(valids) == i))
                skipcount(i) = length(find(res.waitFramesSched(skippedones) == i));
                skipratio(i) = skipcount(i) / length(find(res.waitFramesSched(valids) == i));
            else
                skipcount(i) = nan;
                skipratio(i) = nan;
            end
        end
    %
    %     if length(skippedones) < 25
    %         skippedOnes = skippedones
    %     end
    %

    fprintf('\n');
    fprintf('Skipcount for 2 frames sched: %i, Skipcount for > 2 frame: %i,  Ratio: %f %%\n', skipcount(2), length(skippedones) - skipcount(2), 100 * skipcount(2) / length(skippedones));

    if length(res.waitFramesSched) > 2*4972
        firstHalfSkips = length(find(skippedones <= 4972));
        secondHalfSkips = length(find(skippedones > 4972));
        fprintf('Skipped during regular intervals (1st half): %i\n', firstHalfSkips);
        fprintf('Skipped during randomized intervals (2nd half): %i\n', secondHalfSkips);
        
        if ~isempty(find(skippedones > 9944))
            fprintf('LONG TIME SKIPS!\n');
            disp(skippedones(find(skippedones > 9944)));
            if res.Priority > 0
                error('FOOOH');
            end
        end
    end
    
    
    %     close all;
    %
    %     figure;
    %     plot(skipratio);
    %     title('Ratio of skipped frames vs. total frames per waitFramesSched:');
    %     figure;
    %     plot(skipcount);
    %     title('Number of skipped frames per waitFramesSched:');
    %
    %     if isfield(res, 'TimeAtSwapBuffers')
    %         waitedswap = intersect(skippedones, find((res.swapRequestSubmissionTime - res.targetWhentime) < 0));
    %         %waitedswap = 1:9953;
    %         if ~isempty(waitedswap)
    %             figure ; plot(1000 * (res.TimeAtSwapBuffers(waitedswap) - res.targetWhentime(waitedswap)));
    %             title('Wakeup delay from timed wait (scheduling delay) before SwapBuffers() call: [msecs]:');
    %         end
    %
    %         if ~isempty(skippedones)
    %             figure ; plot(1000 * (res.swapRequestSubmissionTime(skippedones) - res.targetWhentime(skippedones)));
    %             title('Invocation time of Screen(''Flip'') relative to when deadline (positive == delayed): [msecs]:');
    %
    %             figure ; plot(1000 * (res.targetWhentime(skippedones) + 0.5 * res.ifi - res.TimeAtSwapBuffers(skippedones)));
    %             title('Headroom between VSYNC deadline and time of SwapBuffers (positive = good): [msecs]:');
    %
    %             figure; scatter(skippedones,repmat(1,1,length(skippedones)))
    %             title('Placement of skipped frames: x-position == frameId');
    %
    %             %figure ; plot(diff(res.targetWhentime(skippedones) + 0.5 * res.ifi));
    %             %title('VSYNC deadline of skipped frames: [secs]:');
    %         end
    %     end
    %
    %     % Plot distribution of raw flip timestamps wrt. high-precision timestamps:
    %     figure;
    %     hist(1000 * (res.rawFlipTime(2:end) - res.vblFlipTime(2:end)), 100)
    %     title('Histogram: Delay of rawtimestamp wrt. real vbltime of bufferswap');
    %     xlabel('Delay [msecs]');
    %     ylabel('Count');

    fprintf('\n\n\n');
    
    return;
end
