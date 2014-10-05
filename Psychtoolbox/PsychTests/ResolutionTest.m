function ResolutionTest(screenNumbers)
% ResolutionTest prints screen resolutions reported by Screen 'Resolution'
% and 'Resolutions'.
%
% Usage: ResolutionTest([screenNumbers=all]);
%
% On Linux, if a Psychtoolbox screen (=X-Screen) has multiple video outputs
% attached, this function will report both, per output settings and modes and
% the unified resolution of a screen - consisting of virtual resolutions and
% settings formed by all attached outputs. Per-Output settings on Linux can
% be made via Screen('ConfigureDisplay', 'Scanout', screenNumber, outputNumber, ...);
% Screen('Resolution', screenNumber, ...); only affects or reports the unified
% "virtual resolution" of a whole X-Screen, not of its individual outputs.
%
% Also see SetResolution, NearestResolution, and Screen Resolution and Resolutions.
%

% 1/27/00 dgp Wrote it.
% 9/17/01 dgp Added "recommended" to the report.
% 4/24/02 awi Exit on PC with message.
% 10/7/12 mk  Clean it up. Output refresh rates an pixelsize as well.
% 04/15/14 mk Handle multiple video outputs per screen on Linux.

if nargin < 1 || isempty(screenNumbers)
    screenNumbers = Screen('Screens');
end

% Test every screen
for screenNumber = screenNumbers
    % Reject non-existent screens:
    if ~ismember(screenNumber, Screen('Screens'))
        continue;
    end
    
    % Get number of video outputs per screen. Always 1 on non-Linux:
    numOutputs = Screen('ConfigureDisplay', 'NumberOutputs', screenNumber);
    
    fprintf('\nSCREEN %i: CURRENT COMBINED RESOLUTION:\n', screenNumber);
    res = Screen('Resolution', screenNumber);
    disp(res);
    
    if IsLinux
        for outputId = 0:numOutputs-1
            fprintf('\nSCREEN %i - Output %i: CURRENT RESOLUTION:\n', screenNumber, outputId);
            disp(Screen('ConfigureDisplay', 'Scanout', screenNumber, outputId));
        end
        fprintf('\n');
    end
    
    fprintf('\nSCREEN %i: AVAILABLE COMBINED RESOLUTIONS:\n', screenNumber);
    res = Screen('Resolutions', screenNumber);
    oldres = '';
    
    for i=1:length(res)
        resname = sprintf('%dx%d ', res(i).width, res(i).height);
        if isempty(strfind(oldres, resname))
            oldres = [oldres resname]; %#ok<*AGROW>
            fprintf('%d x %d\n', res(i).width, res(i).height);
        end
    end
    
    fprintf('\nSCREEN %i: AVAILABLE COMBINED DETAILED RESOLUTIONS:\n', screenNumber);
    res = Screen('Resolutions', screenNumber);
    for i=1:length(res)
        fprintf('%4d x %4d   %3.0f Hz %d ',res(i).width,res(i).height,res(i).hz, res(i).pixelSize);
        fprintf('bits\n');
    end
    fprintf('\n')
    
    if IsLinux
        for outputId = 0:numOutputs-1
            fprintf('\nSCREEN %i - OUTPUT %i: AVAILABLE PER OUTPUT DETAILED RESOLUTIONS:\n', screenNumber, outputId);
            res = Screen('Resolutions', screenNumber, outputId);
            for i=1:length(res)
                fprintf('%4d x %4d   %3.0f Hz %d ',res(i).width,res(i).height,res(i).hz, res(i).pixelSize);
                fprintf('bits\n');
            end
            fprintf('\n');
        end
        fprintf('\n');
    end
end
