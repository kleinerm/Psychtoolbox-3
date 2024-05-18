function success=EyelinkDoDriftCorrection(el, x, y, draw, allowsetup)
% success=EyelinkDoDriftCorrection(el [, x][, y][, draw][, allowsetup])
%
% DO PRE-TRIAL DRIFT CORRECTION
% We repeat if ESC key pressed to do setup.
% Setup might also have erased any pre-drawn graphics.
%
% Note that EyelinkDoDriftCorrection() internally may use Snd() to play
% auditory feedback tones if el.targetbeep=1 or el.feedbackbeep=1 and the
% el.callback function is set to the default PsychEyelinkDispatchCallback().
%
% If you want to use PsychPortAudio in a script that also calls EyelinkDoDriftCorrection,
% then read "help Snd" for instructions on how to provide proper interoperation
% between PsychPortAudio and the feedback sounds created by Eyelink. The demos
% referenced under "help SR-ResearchDemos" show an even better approach than the
% one described in "help Snd".
%
global eyelinkanimationtarget

success=1;

% if no x and y are supplied, set x,y to center coordinates
if ~exist('x', 'var') || isempty(x) || ~exist('y', 'var') || isempty(y)
    [x,y] = WindowCenter(el.window); % convenience routine part of eyelink toolbox
end

if ~exist('draw', 'var') || isempty(draw)
    draw=1;
end

if ~exist('allowsetup', 'var') || isempty(allowsetup)
    allowsetup=1;
end

while 1
    if Eyelink('IsConnected')==el.notconnected   % Check link often so we don't lock up if tracker lost
        success=0;
        return;
    end
    
    if ~isempty(el.callback) % if we have a callback set, we call it.
        if isempty(eyelinkanimationtarget)
            eyelinkanimationtarget = initmoviestruct();
        end

        global inDoTrackerSetup;
        if isempty(inDoTrackerSetup)
            inDoTrackerSetup = false;
        end

        global inDoDriftCorrection;
        if isempty(inDoDriftCorrection)
            inDoDriftCorrection = true;
        end

        if strcmpi(el.calTargetType, 'video') && ~eyelinkanimationtarget.init
            eyelinkanimationtarget = loadanimationmovie(el, eyelinkanimationtarget);
        end
        result = Eyelink('DriftCorrStart', x, y, 1, draw, allowsetup);

    else
        % else we continue with the old version
        result = EyelinkDoDriftCorrect(el, x, y, draw, allowsetup);
    end
    
    if result==el.TERMINATE_KEY
        success=0;
        return;
    end
    
    % repeat if ESC was pressed to access Setup menu
    if(result~=el.ESC_KEY)
        break;
    end
    
end % while

% fprintf('~isempty(el.callback): %d, el.calTargetType: %s, ~inDoTrackerSetup: %d, eyelinkanimationtarget.init: %d\n', ~isempty(el.callback), el.calTargetType, ~inDoTrackerSetup, eyelinkanimationtarget.init)
if ~isempty(el.callback) && strcmpi(el.calTargetType, 'video') && ~inDoTrackerSetup && eyelinkanimationtarget.init
    eyelinkanimationtarget = cleanupmovie(el, eyelinkanimationtarget);
    inDoDriftCorrection = false;
end

return


    function eyelinkanimationtarget = initmoviestruct()
        eyelinkanimationtarget.init = false;
        eyelinkanimationtarget.movie = [];
        eyelinkanimationtarget.movieduration = [];
        eyelinkanimationtarget.fps = [];
        eyelinkanimationtarget.imgw = [];
        eyelinkanimationtarget.imgh = [];
        eyelinkanimationtarget.calxy = [];
    end

    function eyelinkanimationtarget = loadanimationmovie(el, eyelinkanimationtarget)
        [movie, movieduration, fps, imgw, imgh] = Screen('OpenMovie', el.window, el.calAnimationTargetFilename, el.calAnimationOpenAsync, el.calAnimationOpenPreloadSecs, el.calAnimationOpenSpecialFlags1);
        eyelinkanimationtarget.init = true;
        eyelinkanimationtarget.movie = movie;
        eyelinkanimationtarget.movieduration = movieduration;
        eyelinkanimationtarget.fps = fps;
        eyelinkanimationtarget.imgw = imgw;
        eyelinkanimationtarget.imgh = imgh;
    end

    function eyelinkanimationtarget = cleanupmovie(el, eyelinkanimationtarget)
        texkill = Screen('GetMovieImage', el.window, eyelinkanimationtarget.movie, el.calAnimationWaitTexClose);
        Screen('PlayMovie', eyelinkanimationtarget.movie, 0, el.calAnimationLoopParam);
        if texkill > 0
            Screen('Close', texkill);
        end
        Screen('CloseMovie', eyelinkanimationtarget.movie);
        eyelinkanimationtarget = initmoviestruct();
    end

end
