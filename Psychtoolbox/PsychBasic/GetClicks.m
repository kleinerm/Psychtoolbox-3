function [clicks,x,y,whichButton,clickSecs] = GetClicks(w, interClickSecs, mouseDev, untilTime)
% [clicks,x,y,whichButton,clickSecs] = GetClicks([windowPtrOrScreenNumber][, interclickSecs][, mouseDev][, untilTime=inf])
%
% Wait for the user to click the mouse, and then count the number of clicks
% that occur within a inter-click interval of each other. Return the number
% of 'clicks' and the mouse location 'x', 'y', as well as the indices of the
% pressed buttons in vector 'whichButton', and a vector 'clickSecs' with
% the timestamps of when each click was detected.
%
% The x,y location is the location at the downstroke of the first mouse
% click. The mouse position (x,y) is "local", i.e. relative to the origin of
% the window or screen, if supplied; otherwise it's "global", i.e. relative
% to the origin of the desktop.
%
% The allowed inter-click interval can be adjusted by setting the Matlab
% global variable "ptb_mouseclick_timeout" to a value in seconds. E.g.,
% global ptb_mouseclick_timeout; ptb_mouseclick_timeout = 1; would set the
% inter-click interval to 1 second. By default, the interval is 500 msecs.
%
% You can also specify an override interval in the optional argument
% 'interClickSecs' for a given call to GetClicks. A setting of zero would
% disable multi-click detection, ie., only wait for first mouse-click or
% press, then return immediately.
%
% If the optional parameter 'untilTime' is provided, GetClicks will only wait
% for the first click until that time and then return, regardless if the 1st click
% happened or not. On such a timeout, all return arguments will be zero. E.g.,
% [clicks,x,y,whichButton,clickSecs] = GetClicks(window, 0.2, [], GetSecs + 5);
% would wait up to 5 seconds in total for the first click to happen, and then
% another 0.2 seconds for a 2nd click, then 0.2 seconds for a 3rd click...
%
% The optional 'mouseDev' parameter allows to select a specific mouse or
% pointer device to query if your system has multiple pointer devices.
% Currently Linux only, silently ignored on other operating systems.
%
% See Also: GetMouse, SetMouse, GetMouseIndices

% 5/12/96  dgp  Wrote it.
% 5/16/96  dhb  Wrote as MEX-file, updated help.
% 6/7/96   dhb  Modified as per Pelli suggestion to respond to cmd-.
%          dhb  Modified as per Pelli suggestion to stop any playing sound.
% 8/23/96  dhb  Added support for windowPtr argument.
% 3/10/97  dgp  windowPtrOrScreenNumber
% 6/10/02  awi  Added See Also.
% 2/28/06  mk   Completely rewritten as a wrapper around GetMouse, based on
%          mk   the semantic and description of OS-9 GetClicks().
% 6/17/06  mk   We also pass the windowPtr - argument on Windows now, because
%          mk   now GetMouse.m is able to accept this argument.
% 02/08/09 mk   Report id of pressed button, allow for variable interclick,
%               as suggested by Diederick Niehorster. Reduce rtwait to 2
%               msecs but use WaitSecs('YieldSecs') waits to prevent
%               overload.
% 07/29/11 mk   Allow specification of 'mouseDev' mouse device index.
% 02/28/22 mk   Add 1st click timeout 'untilTime', and 'clickSecs' timestamps.

% Inter-click interval (in secs.) for multiple mouse-clicks.
global ptb_mouseclick_timeout;

% Setup default click timeout if no one set:
if isempty(ptb_mouseclick_timeout)
    ptb_mouseclick_timeout = 0.5; % Default timeout for multi-clicks is 500 msecs.
end

if nargin < 2
    interClickSecs = [];
end

if isempty(interClickSecs)
    interClickSecs = ptb_mouseclick_timeout;
end

if nargin < 3
    mouseDev = [];
end

if nargin < 4 || isempty(untilTime)
    untilTime = inf;
end

% Are we in nice mode?
nice = 1;

% Amount of secs to wait in nice-mode between each poll to avoid overloading
% the system. Now that WaitSecs('YieldSecs') is able to do a non-precise
% wait where it yields the cpu for at least the given amount of time, we
% can use rather strict/short wait intervals without the danger of
% overloading the system, so no need to differentiate between OS:
rtwait = 0.001; % 1 msecs yielding, ie., could take a bit longer.

% Wait for release of buttons if some already pressed:
buttons = 1;
while any(buttons) && (GetSecs < untilTime)
    [x,y,buttons] = GetMouse([], mouseDev);
    if ((nice > 0) && any(buttons)), WaitSecs('YieldSecs', rtwait); end
end

if any(buttons)
    % Timed out:
    [clicks, x, y, whichButton, clickSecs] = deal(0);
    return;
end

% Wait for first mouse button press:
while ~any(buttons) && (GetSecs < untilTime)
    if nargin < 1
        % Don't pass windowPtr argument if none supplied.
        [x,y,buttons] = GetMouse;
    else
        % Pass windowPtr argument to GetMouse for proper remapping.
        [x,y,buttons] = GetMouse(w, mouseDev);
    end
    if ((nice > 0) && ~any(buttons)), WaitSecs('YieldSecs', rtwait); end
end

if ~any(buttons)
    % Timed out:
    [clicks, x, y, whichButton, clickSecs] = deal(0);
    return;
end

% Timestamp 1st click:
clickSecs = GetSecs;

% First mouse click done. (x,y) is our returned mouse position. Assign
% button number(s) of clicked button(s) as well:
whichButton = find(buttons);

% Wait for further click in the timeout interval.
clicks = 1;
tend = clickSecs + interClickSecs;

while GetSecs < tend
    % If already down, wait for release...
    while any(buttons) && GetSecs < tend
        [xd,yd,buttons] = GetMouse([], mouseDev);
        if ((nice > 0) && any(buttons)), WaitSecs('YieldSecs', rtwait); end
    end

    % Wait for a press or timeout:
    while ~any(buttons) && GetSecs < tend
        [xd,yd,buttons] = GetMouse([], mouseDev);
        if ((nice > 0) && ~any(buttons)), WaitSecs('YieldSecs', rtwait); end
    end

    % Mouse click or timeout?
    if any(buttons) && GetSecs < tend
        % Mouse click. Count it.
        clicks = clicks + 1;

        % Timestamp and extend timeout for the next mouse click:
        clickSecs(end+1) = GetSecs;
        tend = clickSecs(end) + interClickSecs;
    end
end

% At this point, (x,y) contains the mouse-position of the first click
% and clicks should contain the total number of distinctive mouse clicks.
return;
