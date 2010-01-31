function OSSchedulingAccuracyTest(configFile, duration)
% OSSchedulingAccuracyTest(configFile [, duration=60])

if nargin < 1
    error('Must provide configFile name!');
end

% Build filename for result file: Based on configname, machinename, date
% and time:
res.configFile = configFile;

% Get basepath of config file: Will use it as basepath for result file:
[basepath configBaseFile] = fileparts(configFile);
basepath = [ PsychtoolboxRoot 'PsychTests' ]; 

res.OSName = OSName;
comp = Screen('Computer');
res.computer = comp;
if isfield(comp, 'system') 
    res.OSSystem = comp.system;
else
    res.OSSystem = 'Linux2.6';
end

if isfield(comp, 'machineName')
    res.machineName = comp.machineName;
else
    res.machineName = input('What is the machines name? ', 's');
end

res.PrecisionWait    = str2double(input('Precisionwait? [1/0]', 's'));
res.Priority         = str2double(input('Priority level? ', 's'));
res.secondaryCPULoad = str2double(input('How much secondary CPU load? [%] ', 's'));
res.secondaryGPULoad = str2double(input('How much secondary GPU load? [%] ', 's'));
res.isMultiDisplay   = input('Is this a multi-display setup? [y/n] ', 's');
res.withSound        = input('With sound? [y/n] ', 's');
res.Comments         = input('Any comments to add? ', 's');

res.mydate = datestr(clock);
res.mydate(isspace(res.mydate)) = '_';
res.mydate(res.mydate == ':') = '-';
res.outFilename = sprintf('%s/TimingtestResults/Schedlat_%s_On_%s_at_%s.mat', basepath, configBaseFile, res.machineName, res.mydate);
[foo, res.outFile] = fileparts(res.outFilename);
fprintf('Will write results to file "%s" [path: %s]\n', res.outFile, res.outFilename);

if nargin < 2
    duration = [];
end

if isempty(duration)
    duration = 60;
end

res.duration   = duration;

Priority(res.Priority);

secs = zeros(1, duration * 10000);
t = secs;
i = 0;

fprintf('Running for %f seconds at Priority %f ...\n', duration, res.Priority);

try
    % Preheat these:
    WaitSecs(0);
    GetSecs;
    
    % Stop after time tstopit:
    tstopit = GetSecs + duration;

    while 1
        % Increment trial counter:
        i = i + 1;
        
        % Pick random wait duration between 0 and 10 msecs:
        secs(i) = rand * 0.010;

        % Try to wait that long, log real wait duration:
        if res.PrecisionWait > 0
            tref = GetSecs;
            tnow = WaitSecs(secs(i));
        else
            tref = GetSecs;
            tnow = WaitSecs('YieldSecs', secs(i));
        end
        t(i) = tnow - tref;
        
        % Abort on timeout:
        if tnow > tstopit
            break;
        end
    end
    
    % Done. Switch to normal scheduling:
    Priority(0);
catch
    Priority(0);
    psychrethrow(psychlasterror);
end

% Cut arrays down to real samplecount:
t = t(1:i);
secs = secs(1:i);

% Log results:
res.nSamples = i;
res.realDelay = t;
res.wantedDelay = secs;

% Save results:
save(res.outFilename, 'res', '-V6');

% Calculate scheduling jitter, ie., per-trial error, in msecs:
jitter = 1000 * abs(t - secs);
fprintf('After %i trials: Mean delay %f msecs, stddev = %f msecs, range = %f msecs.\n\n', i, mean(jitter), std(jitter), range(jitter));
figure;
hist(jitter, 100);
title(sprintf('Scheduling latency histogram: Latency [msecs] vs. counts:\n(N=%i, Prio=%i)', i, res.Priority));
figure;
plot(jitter);

return;
