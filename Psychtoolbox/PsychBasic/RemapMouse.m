function [xo, yo] = RemapMouse(win, viewId, xm, ym)
%   [xo, yo] = RemapMouse(win, viewId, xm, ym); -- Map mouse position to stimulus position.
%
%   Certain operations, e.g., PsychImaging('AddTask', ..., 'GeometryCorrection', ...);
%   to perform geometric display correction, will break the 1:1
%   correspondence between stimulus pixel locations (xo,yo) and the mouse
%   cursor position, ie. a mouse cursor positioned at display position
%   (xm,ym) will be no longer pointing to stimulus pixel (xm,ym). If you
%   want to know which pixel in your original stimulus image corresponds to
%   a specific physical display pixel (or mouse cursor position), use this
%   function to perform the necessary coordinate transformation.
%
%   'win' is the window handle of the onscreen window to remap.
%
%   'viewId' is the view id to remap, the same name you specified in
%   PsychImaging() to setup a geometry correction, e.g., 'AllViews'.
%
%   (xm,ym) is the 2D position in display device space, e.g., as returned
%   from GetMouse() for a mouse cursor position query. Please make sure
%   that you pass in the 'win'dow handle to GetMouse as well, ie.
%   GetMouse(win), so GetMouse() can correct its returned mouse position
%   for your specific display arrangement and onscreen window placement.
%   This function only corrects for distortions inside the onscreen window
%   'win', ie. relative to its origin.
%
%   The returned values (xo, yo) are the remapped stimulus pixel locations,
%   over which a mouse cursor at location (xm,ym) is currently hovering.
%
%   If you pass in a (xm,ym) position for which there doesn't exist any
%   corresponding stimulus pixel position, values outside the window bounds
%   will be returned, e.g., greater than window width or height, or negative.
%
%   If you call this function on a window or view without active geometry
%   manipulations, it will do nothing and simply return the passed in (xm,
%   ym) position, ie. it is safe to use this function all the time.
%
%   Limitations: The function currently only corrects for distortions
%   introduced by the tasks implemented in the Psychtoolbox image
%   processing pipeline via some of the functions available via
%   PsychImaging() function. It does not take other transformations into
%   account, e.g., mirroring, arranging displays of a multi-display setup
%   in an unusual way etc. You may need to add your own code to take such
%   transformations into account.
%

% History:
% 26.12.2011  mk  Written.
% 12.01.2013  mk  Added panelfitter support.
% 04.11.2014  mk  Handle Retina modes on OSX and rotation via panelfitter.
% 03.10.2022  mk  Handle 'AllViews' in absence of 'AllViews' map -> Fallback to 'LeftView'.
% 19.08.2025  mk  Handle Retina scaling on Linux + Wayland.

% This global array is setup by PsychImaging() when setting up geometric
% display correction:
global ptb_geometry_inverseWarpMap;

if nargin < 4
    error('At least one of the required parameters is missing!');
end

if ~isnumeric(win) || ~isscalar(win) || (Screen('WindowKind', win) ~= 1)
    error('Window handle invalid. Does not correspond to an onscreen window!');
end

if ~ischar(viewId)
    error('viewId parameter is not a name string, as required!');
end

% Handling of Retina displays on macOS and Linux+Wayland:
if IsOSX || IsWayland
    winfo = Screen('GetWindowInfo', win);
    isf = winfo.ExternalMouseMultFactor;
    xm = xm * isf;
    ym = ym * isf;
end

if isempty(ptb_geometry_inverseWarpMap) || isempty(ptb_geometry_inverseWarpMap{win})
    % Window does not use geometry correction. Just pass-through our input values:
    xo = xm;
    yo = ym;
else
    % Apply gains and modulo transform to input (xm,ym):
    xm = mod(xm * ptb_geometry_inverseWarpMap{win}.gx, ptb_geometry_inverseWarpMap{win}.mx);
    ym = mod(ym * ptb_geometry_inverseWarpMap{win}.gy, ptb_geometry_inverseWarpMap{win}.my);

    % Default if map is missing:
    xo = xm;
    yo = ym;

    if isfield(ptb_geometry_inverseWarpMap{win}, viewId)
        % Retrieve proper inverse mapping matrix for given viewId:
        map = ptb_geometry_inverseWarpMap{win}.(viewId);
    elseif strcmpi(viewId, 'AllViews') && isfield(ptb_geometry_inverseWarpMap{win}, 'LeftView')
        % No map for 'AllViews' but map for 'LeftView' exists. Use as fallback:
        map = ptb_geometry_inverseWarpMap{win}.('LeftView');
    else
        map = [];
    end

    % Apply inverse map if one exists:
    if ~isempty(map)
        % Round and map (xm,ym) input mouse position to indices in inverse map:
        xm = min(max(round(xm) + 1, 1), size(map, 2));
        ym = min(max(round(ym) + 1, 1), size(map, 1));

        % Lookup corresponding unmapped position from matrix:
        xo = double(map(ym, xm, 1));
        yo = double(map(ym, xm, 2));
    end
end

% At this point (xo, yo) contain mouse position relative to the imaging
% pipelines image buffer for the requested viewId. If remapping wrt. some
% internal buffer or final buffer was requested, we're done. For the most
% common case of remapping to the stimulus input buffer, we need one more
% step:
if strcmpi(viewId, 'AllViews') || strcmpi(viewId, 'LeftView') || strcmpi(viewId, 'RightView')
    % Want mouse position mapped to original stimulus image. The current
    % (xo, yo) contains mouse position wrt. inputBufferFBO for viewId. If
    % our gpu panel scaler is active then we need to undo the rescaling
    % done by the scaler to get the position in the drawBufferFBO which
    % corresponds to our effective stimulus position. Query panel fitter
    % parameters into p:
    p = Screen('PanelFitter', win);

    % Panelfitter active aka p non-zero?
    if any(p)
        % Remap:

        % Non-Zero rotation angle?
        if p(9) ~= 0
            % Yes, need some extra rotation inversion transforms:
            xoff = p(5); yoff = p(6);
            cx = p(10); cy = p(11);
            angle = p(9);

            xo = xo - (xoff + cx);
            yo = yo - (yoff + cy);
            rot = atan2(yo, xo) - (angle * pi / 180);
            rad = norm([yo, xo]);
            xo = rad * cos(rot);
            yo = rad * sin(rot);
            xo = xo + (xoff + cx);
            yo = yo + (yoff + cy);
        end

        % Invert scaling and offsets:
        xo = ((xo - p(5)) / (p(7) - p(5)) * (p(3) - p(1))) + p(1);
        yo = ((yo - p(6)) / (p(8) - p(6)) * (p(4) - p(2))) + p(2);
    end
end

return;
