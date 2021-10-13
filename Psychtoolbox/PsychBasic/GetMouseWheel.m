function wheelDelta = GetMouseWheel(mouseIndex)
% wheelDelta = GetMouseWheel([mouseIndex])
%
% Return change of mouse wheel position of a wheel mouse (in "wheel clicks")
% since last query. 'mouseIndex' is the device index of the wheel mouse to
% query. The argument is optional: If left out, the first detected real wheel
% mouse is queried.
%
% OS X: _____________________________________________________________________
%
% Uses PsychHID for low-level access to mice with mouse wheels. If wheel
% state is not queried frequent enough, the internal queue may overflow and
% some mouse wheel movements may get lost, resulting in a smaller reported
% 'wheelDelta' than the real delta since last query. On OS X 10.4.11 the
% operating system can store at most 10 discrete wheel movements before it
% discards movement events. This uses low-level code which may not work on
% all wheel mice.
%
% Linux: ____________________________________________________________________
%
% Uses GetMouse() extra valuators to check if one of the valuators represents
% a mouse wheel, then translates the valuators absolute wheel position into
% wheel delta by keeping track of old values.
%
% MS-Windows: _______________________________________________________________
%
% This function is not supported and will fail with an error.
%
% ___________________________________________________________________________
% See also: GetClicks, GetMouseIndices, GetMouse, SetMouse, ShowCursor,
% HideCursor
%

% History:
% 05/31/08  mk  Initial implementation.
% 05/14/12  mk  Tweaks for more mice.
% 02/21/17  mk  Support Linux by wrapping around GetMouse() valuator functionality.
% 11/22/17  mk  Fix potential OSX bug. Untested on OSX so far.
% 08/16/21  mk  Adapt to naming convention with libinput driver on X11.

% Cache the detected index of the first "real" wheel mouse to allow for lower
% execution times:
persistent oldWheelAbsolute;
persistent wheelMouseIndex;

if isempty(oldWheelAbsolute)
    oldWheelAbsolute = nan(max(GetMouseIndices)+1, 1);
end

if isempty(wheelMouseIndex) && ((nargin < 1) || isempty(mouseIndex))
    % Find first mouse with a mouse wheel:
    if IsLinux
        mousedices = GetMouseIndices('slavePointer');
    else
        mousedices = GetMouseIndices;
    end
    numMice = length(mousedices);
    if numMice == 0
        error('GetMouseWheel could not find any mice connected to your computer');
    end

    if IsOSX
        allHidDevices=PsychHID('Devices');
        for i=1:numMice
            b=allHidDevices(mousedices(i)).wheels;
            if ~IsOSX
                % On Non-OS/X we can't detect .wheels yet, so fake
                % 1 wheel for each detected mouse and hope for the best:
                b = 1;
            end
            
            if any(b > 0) && isempty(strfind(lower(allHidDevices(mousedices(i)).product), 'trackpad'))
                wheelMouseIndex = mousedices(i);
                break;
            end
        end
    end

    if IsLinux
        for i=mousedices
            [~,~,~,~,~,valinfo] = GetMouse([], i);
            for j=1:length(valinfo)
                if strcmp(valinfo(j).label, 'Rel Vert Wheel')  || strcmp(valinfo(j).label, 'Rel Vert Scroll')
                    wheelMouseIndex = i;
                    break;
                end
            end
            if ~isempty(wheelMouseIndex)
                break;
            end
        end
    end

    if isempty(wheelMouseIndex)
        error('GetMouseWheel could not find any mice with mouse wheels connected to your computer');
    end
end;

% Override mouse index provided?
if nargin < 1 || isempty(mouseIndex)
    % Nope: Assign default detected wheel-mouse index:
    mouseIndex = wheelMouseIndex;
end

if IsLinux
    [~,~,~,~,valuators,valinfo] = GetMouse([], mouseIndex);
    for j=1:length(valinfo)
        if strcmp(valinfo(j).label, 'Rel Vert Wheel') || strcmp(valinfo(j).label, 'Rel Vert Scroll')
            wheelAbsolute = valuators(j);
            if isnan(oldWheelAbsolute(mouseIndex+1))
                wheelDelta = 0;
            else
                wheelDelta = wheelAbsolute - oldWheelAbsolute(mouseIndex+1);
            end
            oldWheelAbsolute(mouseIndex+1) = wheelAbsolute;
            return;
        end
    end
    error('Given mouse does not have a wheel.');
end

% Use low-level access to get wheel state report: Refetch until empty
% report is returned:
wheelDelta = 0;
rep = PsychHID('GetReport', mouseIndex, 1, 0, 10);
while ~isempty(rep)
    wheely = rep(end);
    switch wheely
        case 1,
            wheelDelta = wheelDelta + 1;
        case 255,
            wheelDelta = wheelDelta - 1;
    end
    rep = PsychHID('GetReport', mouseIndex, 1, 0, 10);
end

return;
