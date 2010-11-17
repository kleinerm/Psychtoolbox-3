function PosterBatchAnalyzeTimestamps
% PosterBatchAnalyzeTimestamps
%
% Iterate over all timingtest result files ('Res_flipconfig_xxxx') which
% contain valid high precision flip timestamps, either based on
% 'p'hotodiode measurements or 'd'atapixx measurements.
%
% measMethod must be 'p' or 'd' for photo-diode measurement via RTBox, or
% Datapixx measurement.
%
% The recursive search for matching result files starts at /includes the
% current working directory.
%
% The script iterates over each matching file and adds all collected raw
% and high precision timestamps to a large array of timestamps. At the end
% of iteration, the pooled dataset is analyzed for mean, max and min
% deviation between Flip onset timestamps and measured timestamps. The
% total range of deviation and the standard deviation from the mean (==
% timestamp variability) is computed as well. This is done on the raw
% timestamps and high precision timestamps. End results are printed and
% scatterplots of all individual samples are shown.
%
% This script was used to compute the values of the "Stimulus timestamp
% precision" result table of Mario Kleiner's ECVP-2010 poster "Visual
% stimulus timing precision in Psychtoolbox-3: Tests, pitfalls &
% solutions". The same data is used in the corresponding table of his PhD
% Dissertation.
%
    clear all;
    
    global res;
    
    global gRawDeltas;
    global gPrecDeltas;
    
    includer = [];
    keyboard;

    measMethod = input('Measurement method p or d for photodiode or datapixx?', 's');

    if isempty(measMethod) || ~ismember(measMethod, ['p', 'd'])
        error('Invalid measurement method provided!');
    end
        
    sdir = pwd;
    
    % Recursively get filenames of all result files in 'sdir' directory tree:
    [fdir, fnames] = subdir(sdir);

    [gnrTotal, gnrSkipped, gnrCorrupt, gnrDisagree, gnrDelayed, meanDiff, stdDiff, rawmeanDiff, rawstdDiff, rawrangeDiff] = deal(0);
    sc = 0;
    count = 0;
    rangeDiff = zeros(3,0);
    rawrangeDiff = zeros(3,0);
    
    % Check each for valid and matching datafile:
    for fi = 1:length(fnames)
        fname = char(fnames{fi});

        % Only interested in Priority 1 runs with sync-flip and medium load:
        if ~isempty(strfind(fname, 'Res_flipconfig_'))
            %if ~isempty(strfind(fname, 'Res_flipconfig_prio1_syncflip')) && ~isempty(strfind(fname,'_medium_')) %&& ~isempty(strfind(fname,'noload'))

            % Load result file: Will create struct 'res' with data:
            load(fname);

            if (res.measurementType == measMethod) && ismember(res.VBLTimestampingMode, [1,4]) && isfield(res, 'onsetFlipTime') && res.onsetFlipTime(end) > 0
                count = count + 1
                if isempty(includer) || ismember(count, includer)
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
    end
    
    fprintf('\n\n\n');
    fprintf('Total     frames: %i\n', gnrTotal);
    fprintf('Skipped   frames: %i\n', gnrSkipped);
    fprintf('Corrupted frames: %i\n', gnrCorrupt);
    fprintf('Disagreed frames: %i\n', gnrDisagree);
    fprintf('Delayed   frames: %i\n', gnrDelayed);
    
    if ~isempty(gPrecDeltas)
        meanDiff  = mean(gPrecDeltas);
        stdDiff   = std(gPrecDeltas);
        rangeDiff(1) = range(gPrecDeltas);
        rangeDiff(2) = min(gPrecDeltas);
        rangeDiff(3) = max(gPrecDeltas);

        rawmeanDiff  = mean(gRawDeltas);
        rawstdDiff   = std(gRawDeltas);
        rawrangeDiff(1) = range(gRawDeltas);
        rawrangeDiff(2) = min(gRawDeltas);
        rawrangeDiff(3) = max(gRawDeltas);

        fprintf('Precision timestamps:\n');
        fprintf('Mean error: %f\n', meanDiff);
        fprintf('Stddev.   : %f\n', stdDiff);
        fprintf('Range     : %f\n', rangeDiff(1));
        fprintf('Min Error : %f\n', rangeDiff(2));
        fprintf('Max Error : %f\n', rangeDiff(3));
        fprintf('\n\n');
        fprintf('Raw timestamps:\n');
        fprintf('Mean error: %f\n', rawmeanDiff);
        fprintf('Stddev.   : %f\n', rawstdDiff);
        fprintf('Range     : %f\n', rawrangeDiff(1));
        fprintf('Min Error : %f\n', rawrangeDiff(2));
        fprintf('Max Error : %f\n', rawrangeDiff(3));

    
        close all;

        hold on;
        scatter(0,0,'g');
        scatter(ones(1,length(gPrecDeltas)), gPrecDeltas, 'b');
        scatter(2*ones(1,length(gRawDeltas)), gRawDeltas, 'r');
        hold off;
        drawnow;
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

    global gRawDeltas;
    global gPrecDeltas;
    
    
    agrthresh = 0.0016;
    
    % Discard 1st frame by marking it as invalid:
    res.failFlag(1) = -1;
    valids = find(res.failFlag == 0);
    skippedones = find(abs(res.measuredTime - res.predictedOnset) > 0.75 * res.ifi);
    skippedones = intersect(skippedones, valids);
    nrSkipped = length(skippedones);
    nrCorrupt = length(find(res.failFlag > 0));
    deltas = res.measuredTime - res.onsetFlipTime;
    nrDisagree = length(find(abs(deltas(valids)) > agrthresh));
    nrDelayed = length(find(deltas(valids) < -0.66 * res.ifi));

    % Compute final indices of valid samples in this set:
    agreevalids = intersect(valids, find(abs(deltas) <= agrthresh));
    
    % Only take deltas of valid frames into account:
    vdeltas = 1000 * deltas(agreevalids); %#ok<FNDSB>
    
    
    meanDiff  = mean(vdeltas);
    stdDiff   = std(vdeltas);
    rangeDiff(1) = range(vdeltas);
    rangeDiff(2) = min(vdeltas);
    rangeDiff(3) = max(vdeltas);

    % Only take rawdeltas of valid frames into account:
    rawdeltas = res.measuredTime - res.rawFlipTime;
    vrawdeltas = 1000 * rawdeltas(agreevalids);

    % Only add to global sample pools if no corrupt measurements:
    if nrCorrupt == 0
        % Add to global pool of precsion deltas:
        gPrecDeltas = [gPrecDeltas , vdeltas];

        % Add to global pool of raw deltas:
        gRawDeltas = [gRawDeltas , vrawdeltas];
    end
    
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
    fprintf('\n\n\n');
    
    return;
end
