function SetStereoSideBySideParameters(win, leftOffset, leftScale, rightOffset, rightScale, shaders, offsetUnit)
% Change parameters for side-by-side stereo display modes (4 and 5).
%
% SetStereoSideBySideParameters(win [, leftOffset][, leftScale][, rightOffset][, rightScale][, shaders][, offsetUnit='windowsizes'])
%
% Call this function after the win = PsychImaging('OpenWindow',...); call on an
% onscreen window in side-by-side stereo mode to change the parameters
% of drawing the stereo views.
%
% All parameters except the onscreen 'win'dowhandle are optional and have
% reasonable builtin defaults:
%
% 'leftOffset' = Top-Left [x,y] offset of left eye framebuffer in relative
% coordinates [0,0] == top-left of framebuffer, [1,0] == 1 stereo window
% width to the right, [2,0] == 2 stereo window width to the right etc. This
% is the case if the optional parameter 'offsetUnit' is omitted, empty, or
% set to 'windowsizes'. Otherwise the unit for 'leftOffset' and 'rightOffset'
% is selected by the 'offsetUnit' parameter.
%
% 'leftScale' = Scaling of left eye image buffer. E.g., [1,1] == Don't
% scale. [0.75, 0.5] scale to 75% of original width, 50% of original
% height.
%
% 'rightOffset', 'rightScale' == Ditto for right eye image.
%
% 'shaders' a vector containing the GLSL handles of a new pair of shaders to replace
% the standard builtin side-by-side compositing shaders. shaders(1) for left eye
% view, shaders(2) for right eye view. The old shaders are deleted if new shaders
% are assigned.
%
% 'offsetUnit' is a string, specifying the unit used for specifying 'leftOffset'
% and 'rightOffset'. If omitted, by default it is 'windowsizes'. Following values
% are currently supported:
%
% 'windowsizes' - 1 Unit for x or y is one width or height of the onscreen window,
%                 as described above for 'leftOffset'.
% 'pixels'      - 1 Unit for x or y is one pixel, so you can specify offsets in
%                 absolute pixel coordinates. The pixels are likely framebuffer
%                 pixels, ie. after potential application of Retina/HiDPI scaling
%                 or the panelfitter, but other correction tasks like 'GeometryCorrection'
%                 may affect which 'pixels' are meant, if such tasks change
%                 imaging geometry further.
%

% History:
% 03-Dec-2012 mk   Written.
% 22-Jul-2021 mk   Add optional override 'shaders' parameter.
% 01-Oct-2022 mk   Add optional 'offsetUnit' parameter to allow offsets in pixels.
% 02-Oct-2022 mk   Add RemapMouse() support for 'LeftView' and 'RightView'.

global ptb_geometry_inverseWarpMap;
persistent initial_inverseWarpMap;

% Test if a windowhandle is provided...
if nargin < 1
    error('You must provide the windowhandle for the onscreen window as 1st parameter!');
end

% ... and if it is a valid onscreen window in frame-sequential stereo mode:
if Screen('WindowKind', win) ~= 1
    error('Provided windowhandle is not a valid and open onscreen window!');
end

winfo = Screen('GetWindowInfo', win);
if ~ismember(winfo.StereoMode, [4,5])
    % No side-by-side mode -> No operation.
    fprintf('SetStereoSideBySideParameters: Info: Provided onscreen window is not switched to side-by-side stereo mode. Call ignored.\n');
    return;
end

% Query size of onscreen window in pixels w x h:
[w, h] = Screen('WindowSize', win);

% Parse other arguments, assign defaults if none passed:
if nargin < 2 || isempty(leftOffset)
    leftOffset = [0, 0];
end

if nargin < 3 || isempty(leftScale)
    leftScale = [1, 1];
end

if nargin < 4 || isempty(rightOffset)
    rightOffset = [1, 0];
end

if nargin < 5 || isempty(rightScale)
    rightScale = [1, 1];
end

if nargin < 6
    shaders = [];
end

if nargin < 7 || isempty(offsetUnit)
    offsetUnit = 'windowsizes';
else
    if ~ischar(offsetUnit)
        error('Optional parameter offsetUnit is not a character string, as required.');
    end
end

switch (lower(offsetUnit))
    case {'windowsizes'}
        sw = w;
        sh = h;

    case {'pixels'}
        sw = 1;
        sh = 1;

    otherwise
        error('Invalid value ''%s'' for parameter offsetUnit specified.', offsetUnit);
end

% Query full specification of processing slot for left eye view shader:
% 'slot' is position in processing chain, others are parameters for the
% operation:
[slot shaderid blittercfg voidptr glsl] = Screen('HookFunction', win, 'Query', 'StereoCompositingBlit', 'StereoCompositingShaderDualViewLeft'); %#ok<ASGLU>
if slot == -1
    error('Could not find processing slot for left-eye view!');
end

% Delete old processing slot from pipeline:
Screen('HookFunction', win, 'Remove', 'StereoCompositingBlit' , slot);

if ~isempty(shaders)
    glDeleteProgram(glsl);
    glsl = shaders(1);
end

% Define new blitter configuration for changed parameters:
leftOffset(1) = leftOffset(1) * sw;
leftOffset(2) = leftOffset(2) * sh;
blittercfg = sprintf('Builtin:IdentityBlit:Offset:%i:%i:Scaling:%f:%f', floor(leftOffset(1)), floor(leftOffset(2)), leftScale(1), leftScale(2));

% Insert modified processing function at old position (slot) in the
% pipeline, effectively replacing the slot:
posstring = sprintf('InsertAt%iShader', slot);
Screen('Hookfunction', win, posstring, 'StereoCompositingBlit', shaderid, glsl, blittercfg);

% Query full specification of processing slot for right eye view shader:
% 'slot' is position in processing chain, others are parameters for the
% operation:
[slot shaderid blittercfg voidptr glsl] = Screen('HookFunction', win, 'Query', 'StereoCompositingBlit', 'StereoCompositingShaderDualViewRight'); %#ok<ASGLU>
if slot == -1
    error('Could not find processing slot for right-eye view!');
end

% Delete old processing slot from pipeline:
Screen('HookFunction', win, 'Remove', 'StereoCompositingBlit' , slot);

if ~isempty(shaders)
    glDeleteProgram(glsl);
    glsl = shaders(2);
end

% Define new blitter configuration for changed parameters:
rightOffset(1) = rightOffset(1) * sw;
rightOffset(2) = rightOffset(2) * sh;
blittercfg = sprintf('Builtin:IdentityBlit:Offset:%i:%i:Scaling:%f:%f', floor(rightOffset(1)), floor(rightOffset(2)), rightScale(1), rightScale(2));

% Insert modified processing function at old position (slot) in the
% pipeline, effectively replacing the slot:
posstring = sprintf('InsertAt%iShader', slot);
Screen('Hookfunction', win, posstring, 'StereoCompositingBlit', shaderid, glsl, blittercfg);

% Now we need to define suitable inverse mappings for RemapMouse():

% If we don't have a backup copy of the initial ptb_geometry_inverseWarpMap for
% this win'dow - after PsychImaging('OpenWindow') and before our first invocation,
% then create one as a baseline, so successive invocations of this function always
% start with the same baseline, instead of compounding results from multiple calls:
if isempty(initial_inverseWarpMap) || isempty(initial_inverseWarpMap{win})
    % Initial call, make internal backup for successive calls:
    initial_inverseWarpMap{win} = ptb_geometry_inverseWarpMap{win};
end

% Get true framebuffer size of window and build identity mapping:
[wr, hr] = Screen('WindowSize', win, 1);
[xg, yg] = meshgrid(0:wr-1, 0:hr-1);

% Reset window global modulo parameter:
ptb_geometry_inverseWarpMap{win}.mx = wr;

% Assign left view inverse mapping of mouse position:
curmap(:,:,1) = (xg - leftOffset(1)) * 1 / leftScale(1);
curmap(:,:,2) = (yg - leftOffset(2)) * 1 / leftScale(2);
if ~isfield(initial_inverseWarpMap{win}, 'LeftView')
    ptb_geometry_inverseWarpMap{win}.('LeftView') = int16(curmap);
else
    prevmap = initial_inverseWarpMap{win}.('LeftView');
    ptb_geometry_inverseWarpMap{win}.('LeftView') = int16(remap(prevmap, curmap));
end

% Assign right view inverse mapping of mouse position:
curmap(:,:,1) = (xg - rightOffset(1)) * 1 / rightScale(1);
curmap(:,:,2) = (yg - rightOffset(2)) * 1 / rightScale(2);
if ~isfield(initial_inverseWarpMap{win}, 'RightView')
    ptb_geometry_inverseWarpMap{win}.('RightView') = int16(curmap);
else
    prevmap = initial_inverseWarpMap{win}.('RightView');
    ptb_geometry_inverseWarpMap{win}.('RightView') = int16(remap(prevmap, curmap));
end

end

function newmap = remap(prevmap, curmap)
    newmap = nan(size(curmap));
    curmap = round(curmap + 1);
    curmap = max(curmap, 1);
    curmap(:,:,1) = min(curmap(:,:,1), size(prevmap, 2));
    curmap(:,:,2) = min(curmap(:,:,2), size(prevmap, 1));
    ym = size(curmap, 1);
    xm = size(curmap, 2);
    for y = 1:ym
        for x = 1:xm
            xl = curmap(y, x, 1);
            yl = curmap(y, x, 2);
            newmap(y, x, 1) = prevmap(yl, xl, 1);
            newmap(y, x, 2) = prevmap(yl, xl, 2);
        end
    end
end
