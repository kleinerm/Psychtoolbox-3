function VulkanInteropDebug(flags)
% VulkanInteropDebug([flags=0])
%

close all;
Screen('Preference', 'Verbosity', 5);
PsychVulkan('Verbosity', 5);

fs = 0; % EDIT ME!
n = 1000;
numifis = 0;
loadjitter = 0;
stereo = 0;
flushpipe = 0;
synchronous = 0;
usedpixx = 0;
usevulkan = 1;
screenNumber = [];
clearmode = 2;

if nargin < 1 || isempty(flags)
    flags = 0;
end

PsychVulkan('OverrideFlags', flags);
overrideFlags = PsychVulkan('OverrideFlags')

try
    PsychDefaultSetup(1);
    RestrictKeysForKbCheck(KbName('ESCAPE'));

    % Get the list of Screens and choose the one with the highest screen number.
    % Screen 0 is, by definition, the display with the menu bar. Often when 
    % two monitors are connected the one without the menu bar is used as 
    % the stimulus display.  Chosing the display with the highest display number is 
    % a best guess about where you want the stimulus displayed.  
    screens=Screen('Screens');
    if isempty(screenNumber)
        screenNumber=max(screens);
    end
    screensize=Screen('Rect', screenNumber);

    % Query size of screen:
    screenheight=screensize(4);

    PsychImaging('PrepareConfiguration');
    if usevulkan
        % Use PsychVulkan display backend instead of standard OpenGL:
        PsychImaging('AddTask', 'General', 'UseVulkanDisplay');
    end
    
    if fs
        rect = [];
    else
        rect = InsetRect(screensize, 64, 0);
    end
    w=PsychImaging('OpenWindow',screenNumber, [0 255 0], rect,[],[], stereo);
    
    % Query effective stereo mode, as Screen() could have changed it behind our
    % back, e.g., if we asked for mode 1 but Screen() had to fallback to
    % mode 11:
    winfo = Screen('GetWindowInfo', w);
    stereo = winfo.StereoMode;
        
    % Query nominal framerate as returned by Operating system:
    % If OS returns 0, then we assume that we run on a flat-panel with
    % fixed 60 Hz refresh interval.
    framerate=Screen('NominalFramerate', w);
    if (framerate==0)
        framerate=60;
    end

    ifinominal=1 / framerate;
    fprintf('The refresh interval reported by the operating system is %2.5f ms.\n', ifinominal*1000);
        
    if (stereo>0)
        % Show something for the right eye as well in stereo mode:
        Screen('SelectStereoDrawBuffer', w, 1);
        Screen('FillRect', w, 0);
        Screen('DrawText', w, 'Stereo yeah!!!', 10, 40, 255);
    end
    
    % Measure monitor refresh interval again, just for fun...
    % This will trigger a calibration loop of minimum 100 valid samples and return the
    % estimated ifi in 'ifi': We require an accuracy of 0.1 ms == 0.0001
    % secs. If this level of accuracy can't be reached, we time out after
    % 20 seconds...
    %[ ifi nvalid stddev ]= Screen('GetFlipInterval', w, 100, 0.0001, 5);
    [ ifi, nvalid, stddev ]= Screen('GetFlipInterval', w);
    fprintf('Measured refresh interval, as reported by "GetFlipInterval" is %2.5f ms. (nsamples = %i, stddev = %2.5f ms)\n', ifi*1000, nvalid, stddev*1000);
    
    % Init data-collection arrays for collection of n samples:
    ts=zeros(1,n);
    beampos=ts;
    missest=ts;
    flipfin=ts;
    dpixxdelay=ts;
    td=ts;
    so=ts;
    tSecondary = ts;
    sodpixx = ts;
    boxTime = ts;
    
    % Compute random load distribution for provided loadjitter value:
    wt=rand(1,n)*(loadjitter*ifi);

    % Perform some initial Flip to get us in sync with retrace:
    % tvbl is the timestamp (system time in seconds) when the retrace
    % started. We need it as a reference value for our WaitBlanking
    % emulation:
    tvbl=Screen('Flip', w, [], clearmode);
    
    % Test-loop: Collects n samples.
    for i=1:n
        % Presentation time calculation for waiting 'numifis' monitor refresh
        % intervals before flipping front- and backbuffer:
        % This formula emulates the old PTB-MacOS9 Screen('WaitBlanking', numifis)
        % behaviour as closely as possible.
        % The 'Flip' command takes a presentation timestamp 'tdeadline' as
        % optional argument: If tdeadline == 0 or is left out, Flip will
        % flip at the next possible retrace (PTB 1.0.5 and earlier behaviour).
        % If tdeadline is > 0, then Flip will wait until the system time
        % 'tdeadline' is reached and then flip the buffers
        % at the next possible VBL. This allows to specify absolute points in time
        % at which flip should occur. If you want the old behaviour of
        % Screen('WaitBlanking', w, numifis) back, then just calculate a
        % proper presentation timestamp "tdeadline" relative to the time of last flip
        % 'tvbl', as demonstrated here:        
        tdeadline=tvbl + numifis * ifi - 0.5 * ifi;
        
        % If numifis == 0, flip on next retrace. This should be the same
        % as numifis == 1, but might make a difference in robustness if
        % the stimulus is **very** complex and the load for the system is at
        % the limit that it can handle in a single video-refresh interval.
        if numifis==0
            % If user supplied numifis=0, we force tdeadline=0, so Flip
            % will actually ignore the deadline and just Flip at the next
            % possible retrace...
            tdeadline=0;
        end
        
        if usedpixx
            % Ask for a Datapixx onset timestamp for next 'Flip':
            PsychDataPixx('LogOnsetTimestamps', 1);
        end
        
        % Flip: The clearmode argument specifies if flip should clear the
        % drawing buffer after flip (=0 - default), keep it "as is"
        % for incremental drawing/updating of stims (=1) or don't do
        % anything to the framebuffer at all (=2).
        % We return the timestamp, when VBL starts in tvbl: This is when
        % the front- and back drawing surfaces get exchanged and it is the
        % crucial reference value for computing the 'tdeadline'
        % presentation deadline for the next 'Flip' command.
        % The rasterbeam-position (scanline) when the measurement was taken is returned in beampos(i),
        % the time when flip returned to Matlab is returned in flipfin(i),
        % estimated stimulus onset time aka end of VBL is returned in so(i).
        %
        % The first value "tvbl" is needed for tdeadline calculation if
        % one wants to emulate WaitBlanking of old PTB - see formula above.
        % beampos > screen height means that flip returned during the VBL
        % interval. Small values << screen height are also ok,
        % they just indicate either a slower machine or some types of flat-panels...
        pos=mod(i, screenheight);
        xm = pos + 50;
        ym = pos + 50;

        mouseposrgb = Screen('GetImage', w, OffsetRect([0 0 1 1], xm, ym), 'drawBuffer');
        fprintf('PRE-RGB at cursor position (%f, %f): (%f, %f, %f)\n', xm, ym, mouseposrgb);

        [ tvbl, so(i), flipfin(i), missest(i), beampos(i)]=Screen('Flip', w, tdeadline, clearmode);
        mouseposrgb = Screen('GetImage', w, OffsetRect([0 0 1 1], xm, ym), 'drawBuffer');
        fprintf('POST-RGB at cursor position (%f, %f): (%f, %f, %f)\n', xm, ym, mouseposrgb);
        
        if usedpixx
            % Ask for a Datapixx onset timestamp from last 'Flip':
            [boxTime(i), sodpixx(i)] = PsychDataPixx('GetLastOnsetTimestamp'); %#ok<ASGLU>
            dpixxdelay(i) = GetSecs;
        end
        
        % Special code for DWM debugging: Disabled by default - Not for pure
        % mortals!
        tSecondary(i) = 0;
        if IsWin && 0
            while 1
                WaitSecs('YieldSecs', 0.001);
                wdminfo = Screen('GetWindowInfo', w, 2);

                if ~isstruct(wdminfo)
                    break;
                end
                
                if wdminfo.cDXPresentConfirmed == wdminfo.cDXPresentSubmitted
                    tSecondary(i) = wdminfo.qpcVBlank - ((wdminfo.cDXRefresh - wdminfo.cDXRefreshConfirmed) * wdminfo.qpcRefreshPeriod);
                    tvbl = tSecondary(i);
                    so(i) = tSecondary(i);
                    break;
                end
            end
        end
        
        % Record timestamp for later use:
        ts(i) = tvbl;
        
        % Draw some simple stim for next frame of animation: We draw a
        % simple flashing rectangle that moves over the screen. The same
        % rectangle is drawn with some offset for the right-eye if stereo
        % display is requested:
        Screen('SelectStereoDrawBuffer', w, 0);
        
        Screen('FillRect', w, mod(i, 255), [pos+20 pos+20 pos+400 pos+400]);
        % Screen('FillRect', w, mod(i, 2)*255);
        if (stereo>0)
            % Show something for the right eye as well in stereo mode:
            Screen('SelectStereoDrawBuffer', w, 1);
            Screen('FillRect', w, mod(i, 255), [pos+40 pos+20 pos+420 pos+400]);
        end

        if flushpipe==1
            % Give a hint to PTB that no further drawing commands will
            % follow before the next Flip-command. This can be used by PTB
            % to optimize drawing of very demanding stimuli in order to decrease the
            % chance of deadline misses due to overload. The "clearmode"
            % argument should be the same as the one passed to Flip. It is
            % another hint. If synchronous is set == 1, then
            % DrawingFinished will return an estimate of the time needed by
            % the graphics hardware to draw your stimulus. This is useful
            % for finding the cause of skipped frames. If the value is
            % close to the video refresh interval of your monitor, then you
            % are drawing too much/too complex stims for your graphics
            % hardware -> Reduce complexity, reduce monitor refresh rate or
            % buy faster graphics hardware. Don't set synchronous == 1 for
            % real experiments as it will significantly degrade
            % performance and can *cause* deadline misses.
            td(i)=Screen('DrawingFinished', w, clearmode, synchronous);
        end

        % Sleep a random amount of time, just to simulate some work being
        % done in the Matlab loop:
        WaitSecs(wt(i));
        % And give user a chance to abort the test by pressing any key:
        if KbCheck
            break;
        end
    end % Draw next frame...

    % calculate clock skew corrected Datapixx onset timestamps
    if usedpixx>1
        sodpixx = PsychDataPixx('BoxsecsToGetsecs', boxTime);
    end

    RestrictKeysForKbCheck([]);

    % Shutdown realtime scheduling:
    Priority(0)

    % Close display: If we skipped/missed any presentation deadline during
    % Flip, Psychtoolbox will automatically display some warning message on the Matlab
    % console:
    sca;
    PsychVulkan('OverrideFlags', []);

    % Restrict to actual number of collected samples:
    n = i;
    ts = ts(1:n);
    so = so(1:n);
    flipfin = flipfin(1:n);
    missest = missest(1:n);
    beampos = beampos(1:n);
    td = td(1:n);
    dpixxdelay = dpixxdelay(1:n);
    tSecondary = tSecondary(1:n);
    sodpixx = sodpixx(1:n);
    boxTime = boxTime(1:n);

    % Count and output number of missed flip on VBL deadlines:
    numbermisses=0;
    numberearly=0;

    if numifis > 0
        if (stereo == 11) && (numifis == 1)
            % Special case: Stereomode 11 can't do better than one swap
            % every two refresh cycles, so take this into account:
            ifi = ifi * 2;
        end

        for i=2:n
            if (ts(i)-ts(i-1) > ifi*(numifis+0.5))
                numbermisses=numbermisses+1;
            end

            if (ts(i)-ts(i-1) < ifi*(numifis-0.5))
                numberearly=numberearly+1;
            end
        end
    else
        if stereo == 11
            % Special case: Stereomode 11 can't do better than one swap
            % every two refresh cycles at best, so take this into account:
            ifi = ifi * 2;
        end

        for i=2:n
            if (ts(i)-ts(i-1) > ifi*1.5)
                numbermisses=numbermisses+1;
            end

            if (ts(i)-ts(i-1) < ifi*(numifis-0.5))
                numberearly=numberearly+1;
            end
        end
    end

    % Plot all our measurement results:

    % Figure 1 shows time deltas between successive flips in milliseconds:
    % This should equal the product numifis * ifi:
    figure
    hold on
    plot((ts(2:n) - ts(1:n-1)) * 1000);
    ni = numifis;
    if numifis < 1
        ni = 1;
    end
    
    if (ni < 2 ) && (stereo == 11)
        % Special case: Stereomode 11 can't do better than one swap
        % every two refresh cycles, so take this into account:
        ni = 2;
    end
    
    plot(ones(1,n)*ifi*ni*1000);
    title('Delta between successive Flips in milliseconds:');
    hold off
    
    % Figure 2 shows the recorded beam positions:
    if winfo.VBLEndline > -1
        hasbeampos = 1;
    else
        hasbeampos = 0;
    end

    if hasbeampos
        figure
        plot(beampos);
        title('Rasterbeam position when timestamp was taken (in scanlines):');
    end

    if isequal(ts, so)
        % Same info in vbltime and stimulus onset time. Only
        % do one plot and label it in a less confusing manner:
        figure
        plot((flipfin - so)*1000);

        if IsLinux && (Screen('Preference', 'VBLTimestampingmode') == 4)
            % Linux mode 4: OpenML or Wayland presentation feedback, so is stimulus onset:
            title('Time delta between stimulus onset and return of Flip in milliseconds:');
        elseif (IsLinux || IsOSX) && ismember(Screen('Preference', 'VBLTimestampingmode'), [1, 3]) && (winfo.VBLCount > 0)
            % Linux or OSX, vbl timestamping requested and apparently working. so is vblank time:
            title('Time delta between start of VBL and return of Flip in milliseconds:');
        else
            % Windows or other os'es without working high precision timestamping: so is raw timestamp:
            title('Time delta between return from swap completion and return of Flip in milliseconds:');
        end
    else
        % Figure 4 shows difference in ms between finish of Flip and estimated
        % start of VBL time:
        figure
        plot((flipfin - ts)*1000);
        title('Time delta between start of VBL and return of Flip in milliseconds:');

        % Figure 5 shows difference in ms between finish of Flip and estimated
        % stimulus-onset:
        figure
        plot((flipfin - so)*1000);
        title('Time delta between stimulus onset and return of Flip in milliseconds:');
    end

    % Figure 6 shows duration of drawing commands when calling
    % "DrawingFinished" in synchronous mode.
    if synchronous==1
        figure
        plot(td*1000);
        title('Total duration of all drawing commands in milliseconds:');
    end
    
    if IsWin && (tSecondary(1)>0 && tSecondary(2)>0)
        figure;
        plot((tSecondary - so) * 1000);
        title('Time delta in milliseconds between stimulus onset according to DWM and stimulus onset according to Flip:');
        fprintf('Average discrepancy between DWM and beamposition timestamping is %f msecs, stddev = %f msecs.\n', mean((tSecondary - so) * 1000), std((tSecondary - so) * 1000));
    end
    
    if usedpixx
        figure;
        plot((so - sodpixx) * 1000);
        title('Time delta in msecs onset according to Flip - onset according to DataPixx:');
        fprintf('Average discrepancy between Flip timestamping and DataPixx is %f msecs, stddev = %f msecs.\n', mean((so - sodpixx) * 1000), std((so - sodpixx) * 1000));

        figure;
        plot((dpixxdelay - so) * 1000);
        title('Time delta between stimulus onset and return of Datapixx timestamping in milliseconds:');
    end

    % Output some summary and say goodbye...
    fprintf('PTB missed %i out of %i stimulus presentation deadlines.\n', numbermisses, n);
    fprintf('One missed deadline is ok and an artifact of the measurement.\n');
    
    fprintf('PTB completed %i stimulus presentations before the requested target time.\n', numberearly);
    if numberearly > 0
        fprintf('CAUTION: Completing flips too early should *never ever happen*! Your system has\n');
        fprintf('CAUTION: a serious bug or misconfiguration in its graphics driver!!!\n');
    end
    fprintf('Have a look at the plots for more details...\n');
    
    % Done.
catch %#ok<*CTCH>
    % This "catch" section executes in case of an error in the "try" section
    % above. Importantly, it closes the onscreen window if its open and
    % shuts down realtime-scheduling of Matlab:
    RestrictKeysForKbCheck([]);

    sca;
    PsychVulkan('OverrideFlags', []);
    
    % Disable realtime-priority in case of errors.
    Priority(0);
    psychrethrow(psychlasterror);
end %try..catch..

return
