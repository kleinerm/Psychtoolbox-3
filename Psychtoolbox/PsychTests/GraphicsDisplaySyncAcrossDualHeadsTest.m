function GraphicsDisplaySyncAcrossDualHeadsTest(screenids, nrtrials)
% GraphicsDisplaySyncAcrossDualHeadsTest([screenids] [, nrtrials=6000])
%
% Apple MacOS/X only for now!
%
% Test synchronizity between the scanout/refresh cycles of two different
% display heads 'screenids(1)' and 'screenids(2)'. If 'screenids' is
% omitted, we test the two heads with maximal id. 'nrtrial' sample passes
% with a sampling interval of roughly 1 msecs are conducted. 'nrtrials'
% defaults to 6000 samples.
%
% Each sample consists of querying the current rasterbeam position and
% timestamp of last VBL interrupt on each display head. At the end, the
% samples of both heads are plotted against each other and compared.
%
% The help text in the Matlab prompt tells you how to interpret the plots.

% History:
% 12/11/07 Written (MK).

AssertOpenGL;

if ~IsOSX
    error('Sorry, this test is only supported on MacOS/X.');
end

if nargin < 1
    screenids = [];
end

if isempty(screenids)
    screenids = Screen('Screens');
    if length(screenids)<2
        % Only single head - Nothing to test?!?
        error('Only one display head attached to graphics hardware. How am i supposed to test sync across heads?!?');
    end
    
    if length(screenids)>2
        % If more than two heads available, choose the two heads with
        % maximum id:
        screenids = screenids(end-1:end);
    end
end

if length(screenids) > 2
    error('"screenids" parameter contains indices of more than two screens?!? This is a dual-head sync test!');
end

if nargin < 2
    nrtrials = [];
end

if isempty(nrtrials)
    nrtrials = 6000;
end

% On OS/X we can also test the VBL Interrupt based method, on Windows this
% doesn't exist:
if IsOSX
    testvblirqs = 1;
else
    testvblirqs = 0;
end

% Open windows:
w(1) = Screen('OpenWindow', screenids(1), 0, [0 0 10 10]);
w(2) = Screen('OpenWindow', screenids(2), 0, [0 0 10 10]);

% Some beauty...
Screen('Flip', w(1), 0, 2);
Screen('Flip', w(2), 0, 2);
Screen('FillRect', w(1), 255);
Screen('FillRect', w(2), 255);
Screen('Flip', w(1), 0, 2);
Screen('Flip', w(2), 0, 2);

timinginfo = zeros(2, 2, nrtrials);

% Test loop one:
for i=1:nrtrials
    % Sample in roughly 1 msec intervals:
    WaitSecs(0.001);
    
    % Sample in close sync:
    for j=1:2
        winfo(j) = Screen('GetWindowInfo', w(j));
    end
    
    for j=1:2
        timinginfo(1, j,i) = winfo(j).Beamposition;
        timinginfo(2, j,i) = winfo(j).LastVBLTime;
    end
end

Screen('CloseAll');

fprintf('What you should see in the following plots for well synchronized displays:\n');
fprintf('The "drift" plots should either show a nice zero flat line or a flat line with\n');
fprintf('a small constant offset. Of course a bit of kind of gaussion noisyness has to be\n');
fprintf('expected in the system. What you should not see is some clear trend in one direction\n');
fprintf('or a sawtooth-like pattern. That would indicate drift between the display heads due\n');
fprintf('to missing sync across the heads.\n\n');

% Some plotting:
close all;
if winfo(1).VBLEndline~=-1
    plot(1:nrtrials, squeeze(timinginfo(1,1,:)), 'r', 1:nrtrials, squeeze(timinginfo(1,2,:)), 'b');
    title('Beampositions for both displays: Red = Head 1, Blue=Head 2');

    figure;
    plot(1:nrtrials, squeeze(timinginfo(1,1,:)) - squeeze(timinginfo(1,2,:)));
    title('Beamposition drift between displays:');
end

figure;
plot(1:nrtrials, squeeze(timinginfo(2,1,:)), 'r', 1:nrtrials, squeeze(timinginfo(2,2,:)), 'b');
title('VBL Interrupt timestamps for both displays: Red = Head 1, Blue=Head 2');

figure;
plot(1:nrtrials, squeeze(timinginfo(2,1,:)) - squeeze(timinginfo(2,2,:)));
title('VBL Interrupt timestamp drift between displays:');
