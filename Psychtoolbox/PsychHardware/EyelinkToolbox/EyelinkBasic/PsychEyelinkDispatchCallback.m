function rc = PsychEyelinkDispatchCallback(callArgs, msg)
% Retrieve live eye-image from Eyelink, show it in onscreen window.
%
% This function is normally called from within the Eyelink() file.
% Normal user code only calls it once to supply the eyelink defaults struct.
%
% To define which onscreen window the eye image should be
% drawn to, call it with the return value from EyelinkInitDefaults, e.g.,
% w=Screen('OpenWindow', ...);
% el=EyelinkInitDefaults(w);
% PsychEyelinkDispatchCallback(el); 
%
% to actually receive and display the images, register this function as eyelink's callback:
% if Eyelink('Initialize', 'PsychEyelinkDispatchCallback') ~=0
% 	error('eyelink failed init')
% end
% result = Eyelink('StartSetup',1) %put the tracker into a mode capable of sending images
% %then you must hit 'return' on the PTB computer, this key command will be sent to the tracker host to initiate sending of images.
%
% This function fetches the most recent live image from the Eylink eye
% camera and displays it in the previously assigned onscreen window.
%

% History:
% 15.3.2009 Derived from MemoryBuffer2TextureDemo.m (MK).
% 4.4.2009  updated to use EyelinkGetKey (edf).

% Cached texture handle for eyelink texture:
persistent eyelinktex;

% Cached window handle for target onscreen window:
persistent win;
persistent calxy;
persistent imgtitle;

% Cached eyelink stucture containing keycodes
persistent el

% Cached constant definitions:
persistent GL_RGBA;
persistent GL_RGBA8;
persistent GL_UNSIGNED_BYTE;

if isempty(eyelinktex)
    % Define the two OpenGL constants we actually need. No point in
    % initializing the whole PTB OpenGL mode for just two constants:
    GL_RGBA = 6408;
    GL_RGBA8 = 32856;
    GL_UNSIGNED_BYTE = 5121;
end

% Preinit return code to zero:
rc = 0;

if nargin < 2
    msg = [];
end

if nargin < 1
    callArgs = [];
end

if isempty(callArgs)
    error('You must provide some valid "callArgs" variable as 1st argument!');
end

if (~isnumeric(callArgs) | ~isvector(callArgs)) & ~isstruct(callArgs) %#ok<AND2,OR2>
    error('"callArgs" argument must be a EyelinkInitDefaults struct or double vector!');
end

if isscalar(callArgs) && isstruct(callArgs) && isfield(callArgs,'window') && isscalar(callArgs.window)
    if Screen('WindowKind', callArgs.window) ~= 1
        error('argument didn''t contain a valid handle of an open onscreen window!  pass in result of EyelinkInitDefaults(previouslyOpenedPTBWindowPtr).');
    end
    
    % Ok, valid handle. Assign it and return:
    win = callArgs.window;

	% assume rest of el structure is valid
	el = callArgs;
	
    return;
end

% Not an eyelink struct.  Either a 4 component vector from Eyelink(), or something wrong:
if length(callArgs) ~= 4
    error('Invalid "callArgs" received from Eyelink() Not a 4 component double vector as expected!');
end

% Extract command code:
eyecmd = callArgs(1);

if isempty(win)
    warning('Got called as callback function from Eyelink() but usercode has not set a valid target onscreen window handle yet! Aborted.'); %#ok<WNTAG>
    return;
end

switch eyecmd
    case 1,
        % Nothing to do here. See code below for eye image display...
        
    case 2,
        % Eyelink Keyboard query:

		[rc, el]=EyelinkGetKey(el);
        
    case 3,
        % Alert message:
        fprintf('Eyelink ALERT: %s.\n', msg);
        
    case 4,
        % Image title:
        fprintf('Eyelink image title is %s.\n', msg);
        imgtitle = msg;
        
    case 5,
        % Draw calibration target:
        calxy = callArgs(2:3);
        Screen('DrawDots', win, calxy, 5, [255 255 0]);
        
    case {6 , 7}
        % Setup calibration display: Do nothing except clear screen:
        Screen('Flip', win);
        
    otherwise
        % Unknown command:
        return;
end

if eyecmd ~= 1
    return;
end

% Video callback from Eyelink: We have a 'eyewidth' by 'eyeheight' pixels
% live eye image from the Eyelink system. Each pixel is encoded as a 4 byte
% RGBA pixel with alpha channel set to a constant value of 255 and the RGB
% channels encoding a 1-Byte per channel R, G or B color value. The
% given 'eyeimgptr' as a a specially encoded memory pointer to the memory
% buffer inside Eyelink() that encodes the image.
eyeimgptr = callArgs(2);
eyewidth  = callArgs(3);
eyeheight = callArgs(4);

% Create a new PTB texture of proper format and size and inject the 4
% channel RGBA color image from the Eyelink memory buffer into the texture.
% Return a standard PTB texture handle to it. If such a texture already
% exists from a previous invocation of this routiene, just recycle it for
% slightly higher efficiency:
upsidedown = 0;
eyelinktex = Screen('SetOpenGLTextureFromMemPointer', win, eyelinktex, eyeimgptr, eyewidth, eyeheight, 4, upsidedown, [], GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);

% Draw texture centered in window:
Screen('DrawTexture', win, eyelinktex);

% Draw calibration target:
if ~isempty(calxy)
    Screen('DrawDots', win, calxy, 5, [255 255 0]);
end

% Draw title:
if ~isempty(imgtitle)
    Screen('DrawText', win, imgtitle, eyewidth / 2, 10, [255 0 0]); 
end

% Show it:
Screen('Flip', win);

% Done.
return;
