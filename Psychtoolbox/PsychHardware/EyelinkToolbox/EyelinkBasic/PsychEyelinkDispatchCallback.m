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
%  4.4.2009 Updated to use EyelinkGetKey + fixed eyelinktex persistence crash (edf).
% 11.4.2009 Cleaned up. Should be ready for 1st release, although still
%           pretty alpha quality. (MK).

% Cached texture handle for eyelink texture:
persistent eyelinktex;

% Cached window handle for target onscreen window:
persistent win;
persistent calxy;
persistent imgtitle;
persistent eyewidth;
persistent eyeheight;

% Cached eyelink stucture containing keycodes
persistent el;
persistent instructionsDrawn;
persistent lastImageTime;
persistent drawcount;

% Cached constant definitions:
persistent GL_RGBA;
persistent GL_RGBA8;
persistent hostDataFormat;

if 0 == Screen('WindowKind', eyelinktex)
	eyelinktex = []; %got persisted from a previous ptb window which has now been closed; needs to be recreated
end

if isempty(eyelinktex)
	% Define the two OpenGL constants we actually need. No point in
	% initializing the whole PTB OpenGL mode for just two constants:
	GL_RGBA = 6408;
	GL_RGBA8 = 32856;
	GL_UNSIGNED_BYTE = 5121; %#ok<NASGU>
    GL_UNSIGNED_INT_8_8_8_8 = 32821; %#ok<NASGU>
	GL_UNSIGNED_INT_8_8_8_8_REV = 33639;
    hostDataFormat = GL_UNSIGNED_INT_8_8_8_8_REV;
    drawcount = 0;
    lastImageTime = GetSecs;
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

if ~isnumeric(callArgs) & ~isstruct(callArgs) %#ok<AND2,OR2>
	error('"callArgs" argument must be a EyelinkInitDefaults struct or double vector!');
end

% Eyelink el struct provided?
if isstruct(callArgs) & isfield(callArgs,'window') %#ok<AND2>
    % Check if el.window subfield references a valid window:
	if Screen('WindowKind', callArgs.window) ~= 1
		error('argument didn''t contain a valid handle of an open onscreen window!  pass in result of EyelinkInitDefaults(previouslyOpenedPTBWindowPtr).');
	end

	% Ok, valid handle. Assign it and return:
	win = callArgs.window;

	% Assume rest of el structure is valid:
	el = callArgs;
    
    instructionsDrawn=0;
    eyelinktex=[];
    lastImageTime=GetSecs;

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

if ~instructionsDrawn
    instructionsDrawn=1;
    helpTxt='Hit return (on either PTB computer or tracker host computer) to toggle camera image, esc to quit';
    Screen('DrawText',win,helpTxt,0,0,[0 0 0]);
    Screen('Flip',win);
end

% Flag that tells if a new camera image was received and our camimage
% texture needs update:
newcamimage = 0;
needsupdate = 0;

switch eyecmd
	case 1,
		% New videoframe received. See code below for actual processing.
        newcamimage = 1;
        needsupdate = 1;
	case 2,
		% Eyelink Keyboard query:
		[rc, el] = EyelinkGetKey(el);

	case 3,
		% Alert message:
		fprintf('Eyelink ALERT: %s.\n', msg);
        needsupdate = 1;
        % TODO FIXME: Implement some reasonable behaviour...
	case 4,
		% Image title of camera image transmitted from Eyelink:
		fprintf('Eyelink image title is %s. [Threshold = %f]\n', msg, callArgs(2));
        if callArgs(2) ~= -1
            imgtitle = sprintf('Title: %s [Threshold = %f]', msg, callArgs(2));
        else
            imgtitle = msg;
        end
        needsupdate = 1;

        % TODO FIXME: Rebuild text slide with new title...
        
	case 5,
		% Define calibration target and enable its drawing:
		calxy = callArgs(2:3);
        needsupdate = 1;

    case 6,
        % Clear calibration display:
		fprintf('clear_cal_display.\n');
        needsupdate = 1;

        % TODO FIXME: Implement some response, if needed.
        
	case 7,
		% Setup calibration display:
        fprintf('setup_cal_display.\n');
		Screen('Flip', win);
        instructionsDrawn=0;

        drawcount = 0;
        lastImageTime = GetSecs;
        needsupdate = 1;

    case 8,
        % Setup image display:
        eyewidth  = callArgs(2);
        eyeheight = callArgs(3);
		fprintf('setup_image_display for %i x %i pixels.\n', eyewidth, eyeheight);
        
        drawcount = 0;
        lastImageTime = GetSecs;
        needsupdate = 1;

    case 9,
        % Exit image display:
		fprintf('exit_image_display.\n');
        fprintf('AVG FPS = %f Hz\n', drawcount / (GetSecs - lastImageTime));
        needsupdate = 1;

    case 10,
        % Erase current calibration target:
        fprintf('erase_cal_target.\n');
		calxy = [];
        needsupdate = 1;

    case 11,
        fprintf('exit_cal_display.\n');
        fprintf('AVG FPS = %f Hz\n', drawcount / (GetSecs - lastImageTime));
        needsupdate = 1;
        
    case 12,
        % New calibration target:
        fprintf('cal_target_beep_hook.\n');
        % TODO FIXME: Callout into proper subroutines in 'el' struct,
        % instead of hard-coding a beep signal here!
        Beeper(1000, 0.8, 0.05);
        
    case 13,
        % New drift correction target:
        fprintf('dc_target_beep_hook.\n');
        % TODO FIXME: Callout into proper subroutines in 'el' struct,
        % instead of hard-coding a beep signal here!
        Beeper(1200, 0.8, 0.05);
        
    case 14,
        errc = callArgs(2);
        fprintf('cal_done_beep_hook: %i\n', errc);
        % TODO FIXME: Callout into proper subroutines in 'el' struct,
        % instead of hard-coding a beep signal here!

        if errc > 0
            % Calibration failed:
            Beeper(400, 0.8, 0.25);
        else
            % Calibration success:
            Beeper(800, 0.8, 0.25);
        end

    case 15,
        errc = callArgs(2);
        fprintf('dc_done_beep_hook: %i\n', errc);
        % TODO FIXME: Callout into proper subroutines in 'el' struct,
        % instead of hard-coding a beep signal here!

        if errc > 0
            % Drift correction failed:
            Beeper(300, 0.8, 0.25);
        else
            % Drift correction success:
            Beeper(700, 0.8, 0.25);
        end

	otherwise
		% Unknown command:
		return;
end

% Display redraw and update needed?
if ~needsupdate
    % Nope. Return from callback:
	return;
end

% Need to rebuild/redraw and flip the display:

% New video data from eyelink?
if newcamimage 
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
    eyelinktex = Screen('SetOpenGLTextureFromMemPointer', win, eyelinktex, eyeimgptr, eyewidth, eyeheight, 4, 0, [], GL_RGBA8, GL_RGBA, hostDataFormat);
end

% Draw eye camera image texture centered in window, if any such texture exists:
if ~isempty(eyelinktex)
    Screen('DrawTexture', win, eyelinktex);
end

% Draw calibration target, if any:
if ~isempty(calxy)
	Screen('DrawDots', win, calxy, 5, [255 255 0]);
end

% Draw title:
if ~isempty(imgtitle)
    % TODO FIXME: Would be better to just draw any kind of text (in the
    % switch-case statement above) into an offscreen window that acts as a
    % slide of text, then just 'DrawTexture' that text slide offscreen
    % window here as kind of an overlay. That would be more efficient,
    % given that 'DrawText' is a rather slow operation and text updates are
    % infrequent...
	Screen('DrawText', win, imgtitle, eyewidth / 2, 10, [255 0 0]);
end

% TODO FIXME: Merge this with the drawtext code above: One slide for all
% currently active text messages...
if exist('helpTxt', 'var')
	Screen('DrawText',win,helpTxt,0,0,[0 0 0]);
end

% Show it: We disable synchronization of Matlab to the vertical retrace.
% This way, display update itself is still synced and tear-free, but we
% don't waste time waiting for swap completion. Potentially higher
% performance for calibration displays and eye camera image updates...
Screen('Flip', win, 0, 0, 1);

% Some counter, just to measure udpate rate:
drawcount = drawcount + 1;

% Done. Return from callback:
return;
