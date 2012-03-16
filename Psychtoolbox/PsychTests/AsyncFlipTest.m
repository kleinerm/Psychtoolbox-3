function AsyncFlipTest(screenid, workload, pollingwait)
% AsyncFlipTest - Stress-Test & Benchmark async flips.
%
% This exercises the async flip implementation in some simulated
% workloads, to test its reliability on a given GPU + driver + operaring
% system combo, and to benchmark its timing behaviour.
%
% The reliability and performance of async flips depends strongly
% on the quality of the underlying graphics hardware (GPU), its
% device drivers and the operating system. When doing concurrent
% async flipping and drawing, the load is even higher, as is
% dependency on a specific system setup.
%
% This demo first runs a red flicker stimulus for basic testing,
% until you press any key. Then it runs more complex stimuli
% until you press any key or 10 seconds worth of stimulation have
% elapsed. Then various timing parameters are plotted.
%
% In the first part you should see homogeneous regular red
% flickering.
%
% In the second part you should see a monochrome (workload = 1),
% colorful (workload = 2) random dot image. For workload = 0
% you should see a black background, and in the center of the
% screen a rectangle with a flickering green background and a
% yellow text "Hallo ;-)". If you see something else, your
% operating system or graphics driver has a bug. If you see
% irregular flicker then your system is not up to the task at
% the current video refresh rate, ie., it skips many presentation
% deadlines due to some overload of the GPU or system.
%
% Usage:
%
% AsyncFlipTest2([screenid=max][, workload=0][, pollingwait=0]);
%
% screenid = Which screen to run on. Default = max screen.
%
% workload = Which drawing workload to simulate in test part II [0, 1 2].
%            See code for workloads. 0 Simulates drawing into offscreen
%            windows, then blitting those into onscreen window.
%            1 simulates large luminance texture workload.
%            2 simulate the kind of texture creation, upload and
%            drawing workload you would have when playing back 1920x1080p
%            HD video in color.
%
% pollingwait = Use 'AsyncFlipCheckEnd' instead of 'AsyncFlipEnd'
%               in 1st test?
%

% History:
% 6.01.2012  mk  Written.

AssertOpenGL;

if nargin < 1 || isempty(screenid)
    screenid = max(Screen('Screens'));
end

if nargin < 2 || isempty(workload)
    workload = 0;
end

if nargin < 3 || isempty(pollingwait)
    pollingwait = 0;
end

% Some random test images for texture testing:
img  = uint8(rand(1080, 1920) * 255);
img3 = uint8(rand(1080, 1920, 3) * 255);

% Open onscreen window with imaging pipeline enabled:
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'UseVirtualFramebuffer');
w = PsychImaging('OpenWindow', screenid, 0);

% Contrastmodulation for flickerstim is 80% of range:
cm=0.8;

KbReleaseWait;

ifi = Screen('GetFlipInterval', w);

% Sync us to retrace with a conventional flip:
Screen('Flip', w);

% Until keypress, test the simple way: Don't draw while flip
% is pending, but wait for flip completion before drawing:
while ~KbCheck
    % Wait for flip to complete, collect timestamps:
    if pollingwait
        tv1=0;
        while tv1 == 0
            tv1 = Screen('AsyncFlipCheckEnd', w);
            WaitSecs('YieldSecs', 0.001);
        end
    else
        tv1 = Screen('AsyncFlipEnd', w);
    end

    % Draw something:
    cm = 1 - cm;
    Screen('FillRect', w, [255*cm 0   0]);

    % Schedule new flip:
    Screen('AsyncFlipBegin', w, tv1 + ifi/2);
end

% Ready for part II:
KbReleaseWait;

% Some sync flips, and clearing the two buffers to black
% and green:
Screen('Flip', w);
Screen('FillRect', w, [0 255 0]);
Screen('Flip', w, 0, 2);
Screen('FillRect', w, [0 0 0]);
Screen('Flip', w, 0, 2);

n = Screen('NominalFramerate', w) * 10;
if n == 0
    n = 60 * 10;
end

t = zeros(6, n + 2);
fc = 1;

try
    % Test offscreen window interaction as well:
    woff = Screen('OpenOffscreenwindow', w, 128, [0 0 500 100]);

    % Do the parallel async flip and draw stuff until keypress:
    while ~KbCheck && (fc < n)
        % Drawing simulation:
        cm = 1 - cm;
        if ismember(0, workload)
            % Simulate drawing into offscreen windows:
            Screen('FillRect', woff, [0, 255*cm, 0]);
            DrawFormattedText(woff, 'Hallo :-)', 'center', 'center', [255 255 0]);

            % Draw final offscreen window to onscreen window:
            Screen('DrawTexture', w, woff);
        end

        if ismember(1, workload)
            % Simulate luminance texture blitting:
            tex = Screen('MakeTexture', w, img);
            Screen('DrawTexture', w, tex, [], [], [], [], [], [0 0 255*cm]);
            Screen('Close', tex);
        end

        if ismember(2, workload)
            % Simulate RGB texture blitting as in HD video playback:
            tex = Screen('MakeTexture', w, img3, [], [], [], 3);
            Screen('DrawTexture', w, tex, [], [], [], [], [], 255 * cm * [1 1 1]);
            Screen('Close', tex);
        end

        % Measure completion of draw-command submission to GPU driver:
        t(4,fc) = GetSecs;

        % Finish it off: We do a sync flip, so we can measure how long
        % the whole stimulus rendering takes:
        Screen('Drawingfinished', w, 2, 1);
        t(5,fc) = GetSecs;

        % Update framecounter:
        fc = fc + 1;
        t(6,fc) = GetSecs;

        % Wait for last AsyncFlip to complete, get its onset timestamp:
        t(1, fc) = Screen('AsyncFlipEnd', w);

        % How long did async flip post-completion code run? This includes
        % operating system scheduling delays and the completion timestamping.
        t(2, fc) = GetSecs;

        % Schedule next async flip:
        Screen('AsyncFlipBegin', w, t(1, fc) + ifi/2, 2);

        % How long did preparing the async flip take? This includes running the
        % PTB imaging pipeline to prepare the final framebuffer image:
        t(3, fc) = GetSecs;

        % Draw next image...
    end

    % Final flip to avoid shutdown warning:
    Screen('Flip', w);
    Screen('CloseAll');

    % Truncate to valid range, convert to msec's:
    t = 1000 * t(:, 2:fc-2);

    close all ;

    figure;
    plot(t(2,:) - t(1,:));
    title('Delay stimulus onset -> flip completion [msecs]:');

    figure;
    plot(t(3,:) - t(2,:));
    title('Delay flip completion -> Next flip scheduled [msecs]:');

    figure;
    plot(t(4,:) - t(3,:));
    title('Delay flip scheduled -> Drawing for next stimulus scheduled [msecs]:');

    figure;
    plot(t(5,:) - t(3,:));
    title('Delay flip scheduled -> Drawing of next stim finished [msecs]:');

    figure;
    plot(t(1,:) - t(6,:));
    title('Remaining headroom from drawing completion of next stim to onset of previous stim [msecs]:');
catch
    sca;
end
