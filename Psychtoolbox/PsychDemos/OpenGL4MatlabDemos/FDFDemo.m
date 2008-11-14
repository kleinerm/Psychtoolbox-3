function FDFDemo(dotDensity, dotLifetime)
% FDFDemo(dotDensity, dotLifetime) - Demo of "formless dot field" stimulus
% via moglFDF.
%
% This demo generates a simple "formless dot field" random dot motion
% stimulus to create "structure from motion" percept by use of the moglFDF
% function for formless dot field rendering. See "help moglFDF" for more
% details.
%
% The demo shows a simple spinning 3D sphere, rendered as random dot
% stimulus.
%
% The following optional parameters can be provided to FDFDemo:
%
% dotDensity = Number of dots to use for both, the background- and
%              foreground distribution. Defaults to 10000.
%
% dotLifetime = Lifetime of dots in frames. Defaults to 10 frames.
%
%
% Control keys:
%
% ESCape key finishes the demo.
%
% SPACE key toggles between a slowly rotating sphere and a static sphere.
%
% 'd' toggles the display between the formless dot field stimulus and some
% debug visualization.
%
%

% History:
% 05/02/08  Written (MK).
% 11/03/08  Documentation update, preparation for public release (MK).

% Setup default settings:
if nargin < 1 || isempty(dotDensity)
    % 10000 dots by default:
    dotDensity = 10000;
end

if nargin < 2 || isempty(dotLifetime)
    % 10 frames lifetime per dot by default:
    dotLifetime = 10;
end

debug = 0;

% Is the script running in OpenGL Psychtoolbox? Abort, if not.
AssertOpenGL;

% Setup unified keyboard mapping:
KbName('UnifyKeyNames');
escape = KbName('ESCAPE');
space = KbName('space');
dKey = KbName('d');

% Find the screen to use for display:
screenid=max(Screen('Screens'));

% Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
% mogl OpenGL for Matlab wrapper:
InitializeMatlabOpenGL([], 0);

% Open a double-buffered full-screen window on the main displays screen,
% with fast Offscreen window support enabled and black background clear
% color. Fast Offscreen windows support is needed for moglFDF to work.
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'UseFastOffscreenWindows');
[win , winRect] = PsychImaging('OpenWindow', screenid, 0);

try
    % Prepare texture to by applied to the sphere: Load & create it from an image file:
    myimg = imread([PsychtoolboxRoot 'PsychDemos/OpenGL4MatlabDemos/earth_512by256.jpg']);

    % Make a special power-of-two texture from the image by setting the enforcepot - flag to 1
    % when calling 'MakeTexture'. GL_TEXTURE_2D textures (==power of two textures) are
    % especially easy to handle in OpenGL. If you use the enforcepot flag, it is important
    % that the texture image 'myimg' has a width and a height that is exactly a power of two,
    % otherwise this command will fail: Allowed values for image width and height are, e.g.,
    % 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 and on some high-end gfx cards
    % 4096 pixels. Our example image has a size of 512 by 256 pixels...
    mytex = Screen('MakeTexture', win, myimg, [], 1);

    % Retrieve OpenGL handles to the PTB texture. These are needed to use the texture
    % from "normal" OpenGL code:
    [gltex, gltextarget] = Screen('GetOpenGLTexture', win, mytex);

    % Debug output settings for moglFDF: Most useful are -1 for real object
    % render and 1 for silhouette render, as well as 0 for normal op.
    moglFDF('DebugFlag', 0);

    % Stimulus parameters:

    % Size of the final output window to be drawn to:
    rect = Screen('Rect', win);
    
    % Texture coordinates on the surface of our demo object are in the
    % range 0.0 to 1.0 in both x- and y- direction:
    texCoordMin   = [0.0 , 0.0];
    texCoordMax   = [1.0 , 1.0];
    
    % Resolve motion with 512 x 512 resolution:
    texResolution = [256 , 256];
    
    % Probability with which a randomly located dot within the silhouette
    % is drawn -- Kind of "Signal to noise" ratio within the objects
    % silhouette, if the "object-induced dot motion" is considered the
    % signal and the noise is considered the noise.
    % Values between 0 - 1 are meaningful:
    BGSilhouetteAcceptanceProbability = 0.5;
    
    % Increase dotDensity if needed, to make sure it is an integral multiple
    % of dotLifetime as requested by moglFDFs current implementation:
    dotDensity = ceil(dotDensity / dotLifetime) * dotLifetime;
    
    % Use max 'dotDensity' foreground dots for sampling the objects
    % surface: In the current moglFDF implementation, maxFGDots must be an
    % integral multiple of the dotLifetime!
    maxFGDots = max(ceil(((1 - BGSilhouetteAcceptanceProbability) * dotDensity) / dotLifetime) * dotLifetime, dotLifetime);

    % Use max 'dotDensity' dots for background distribution:
    maxBGDots = dotDensity;
    
    % Use occlusion culling: Dots that would stick to the occluded part of
    % the 3D objects surface are not drawn. By default - if this parameter
    % is omitted or set > 1 - all dots are drawn, even "occluded" ones.
    zThreshold = 0.0001;
    
    % Setup 'fdf' context with all stimulus parameters as defined above:
    fdf = moglFDF('CreateContext', win, rect, texCoordMin, texCoordMax, texResolution, maxFGDots, maxBGDots, dotLifetime, zThreshold, BGSilhouetteAcceptanceProbability);

    % Define actual string of commands that renders the 3D object or scene:
    % This command sequence will first glRotate the object by 0.1 degrees
    % (if toggle is set to 1), then draw our sphere 'mysphere' at that
    % orientation. See setup code below for the definition of 'toggle' and
    % 'mysphere'.
    callbackEvalString = 'glRotatef(toggle * 0.1, 0, 0, 1); gluSphere(mysphere, 0.7, 100, 100);';
    fdf = moglFDF('SetRenderCallback', fdf, callbackEvalString);

    % Setup the OpenGL rendering context of the onscreen window for use by
    % OpenGL wrapper. After this command, all following OpenGL commands will
    % draw into the onscreen window 'win':
    Screen('BeginOpenGL', win);

    % Get the aspect ratio of the screen:
    ar=winRect(4)/winRect(3);

    % Setup default drawing color to white (R,G,B)=(1,1,1):
    glColor3f(1,1,1);

    % Enable proper occlusion handling via depth tests:
    glEnable(GL.DEPTH_TEST);

    % Set projection matrix: This defines a perspective projection,
    % corresponding to the model of a pin-hole camera - which is a good
    % approximation of the human eye and of standard real world cameras --
    % well, the best aproximation one can do with 3 lines of code ;-)
    glMatrixMode(GL.PROJECTION);
    glLoadIdentity;

    % Field of view is 25 degrees from line of sight. Objects closer than
    % 0.1 distance units or farther away than 100 distance units get clipped
    % away, aspect ratio is adapted to the monitors aspect ratio:
    gluPerspective(25,1/ar,0.1,100);

    % Setup modelview matrix: This defines the position, orientation and
    % looking direction of the virtual camera:
    glMatrixMode(GL.MODELVIEW);
    glLoadIdentity;

    % Reposition camera: Sitting at (x,y,z)==(0,0,5), looking at the origin
    % (0,0,0), in an upright (0,1,0) orientation:
    gluLookAt(0,0,5,0,0,0,0,1,0);

    % Set background clear color to 'black' (R,G,B,A)=(0,0,0,0):
    glClearColor(0,0,0,0);

    % Clear out the backbuffer: This also cleans the depth-buffer for
    % proper occlusion handling: You need to glClear the depth buffer whenever
    % you redraw your scene, e.g., in an animation loop. Otherwise occlusion
    % handling will screw up in funny ways...
    glClear;

    % Enable texture mapping for this type of textures...
    glEnable(gltextarget);

    % Bind our texture, so it gets applied to all following objects:
    % Btw. this whole texture setup is pretty futile for random dot field
    % rendering as we don't apply any textures, we only use the texture
    % coordinates! We apply a texture anyway, so the users sees something
    % nice if one of the debug modes is enabled where the object is
    % renderered in a "normal way" for illustrative purposes.
    glBindTexture(gltextarget, gltex);

    % Textures color texel values shall modulate the color computed by lighting model:
    glTexEnvfv(GL.TEXTURE_ENV,GL.TEXTURE_ENV_MODE,GL.MODULATE);

    % Clamping behaviour shall be a cyclic repeat:
    glTexParameteri(gltextarget, GL.TEXTURE_WRAP_S, GL.REPEAT);
    glTexParameteri(gltextarget, GL.TEXTURE_WRAP_T, GL.REPEAT);

    % Set up minification and magnification filters. This is crucial for the thing to work!
    glTexParameteri(gltextarget, GL.TEXTURE_MIN_FILTER, GL.LINEAR);
    glTexParameteri(gltextarget, GL.TEXTURE_MAG_FILTER, GL.LINEAR);

    % Create the sphere as a quadric object. This is needed because the simple glutSolidSphere
    % command does not automatically assign texture coordinates for texture mapping onto a sphere:
    % mysphere is a handle that you need to pass to all quadric functions:
    mysphere = gluNewQuadric;

    % Enable automatic generation of texture coordinates for our quadric object:
    gluQuadricTexture(mysphere, GL.TRUE);

    % Apply some static rotation to the object to have a nice view onto it:
    % This basically rotates our spinning earth into an orientation that
    % roughly matches the real orientation in space...
    % First -90 degrees around its x-axis...
    glRotatef(-90, 1,0,0);

    % ...then 18 degrees around its new (rotated) y-axis...
    glRotatef(18,0,1,0);

    % OpenGL setup done: Switch back to 2D mode:
    Screen('EndOpenGL', win);

    % Init the rotation 'toggle' flag (see above) to "rotation enabled":
    toggle = 1;

    % Our framecounter, we love stats ;-)
    fcount = 0;
    
    % Initial Flip to have a nice black display, and to record the 'tstart'
    % timestamp of this demo animation:
    tstart = Screen('Flip', win);
    
    % Now for our little animation loop. This loop will run until a key is pressed.
    % It rotates the object by a few degrees (actually: Applies a rotation transformation
    % to all objects to be drawn) and then redraws it at its new
    % orientation:
    while 1
        % Perform object-render, update & recompute cycle for set of dots.
        % moglFDF will compute the new dot distribution, based on the
        % current 3D scene appearance, but it won't draw the new dot
        % distribution for the next frame yet:
        fdf = moglFDF('Update', fdf);

        % Wanna have nice looking random dots?
        if 1
            % Enable alpha blending and smooth dots for nice looking
            % anti-aliased dots:
            glBlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA);
            glEnable(GL.BLEND);
            glPointSize(3.0);
            glEnable(GL.POINT_SMOOTH);
        end
        
        % Render 2D dot set in yellow:
        glColor3f(1,1,0);
        
        % This performs the actual high-speed drawing of the dot field into
        % window 'win':
        fdf = moglFDF('Render', fdf, win, [1 1]);

        % Set to 1 for readback of dots and "manual" visualization:
        if 0
            % This is a slow alternative to moglFDF('Render'):
            % The final dot distribution is read back from the GPU into a
            % Matlab dot matrix 'xydots'...
            xydots = moglFDF('GetResults', fdf);

            % For the fun of it, some stats of the xydots are computed and
            % printed -- here the minimum and maximum 2D dot locations...
            minx=min(xydots(1,:)) %#ok<NOPRT,NASGU>
            miny=min(xydots(2,:)) %#ok<NOPRT,NASGU>
            maxx=max(xydots(1,:)) %#ok<NOPRT,NASGU>
            maxy=max(xydots(2,:)) %#ok<NOPRT,NASGU>

            % And good'ol Screen('DrawDots') is used to draw the 'xydots'
            % vector of dots:
            Screen('DrawDots', win, xydots, 2, [255 0 0], [], 1);
        end

        % Done with drawing, disable alpha blending again:
        glDisable(GL.BLEND);
        
        % Check for and handle keyboard input:
        [pressed secs keyCode] = KbCheck;
        if pressed
            % Pressing ESCape finishes the demo by breaking out of the
            % animation loop:
            if keyCode(escape)
                break;
            end
            
            % Pressing SPACE toggles the 3D rotation of the sphere. As soon
            % as rotation stops and motion information therefore gets lost,
            % the perception of the sphere will quickly degrade into random
            % dot blinking:
            if keyCode(space)
                KbReleaseWait;
                toggle = 1 - toggle;
            end
            
            if keyCode(dKey)
                KbReleaseWait;
                debug = mod(debug+1, 3);
                switch debug
                    case 0,
                        moglFDF('DebugFlag', 0);
                    case 1,
                        moglFDF('DebugFlag', 1);
                    case 2,
                        moglFDF('DebugFlag', -1);
                end
            end
        end
        
        % Show new image at next retrace:
        Screen('Flip', win);

        % Update framecounter:
        fcount = fcount + 1;

        % Ready for next draw loop iteration...
    end;
    
    % End of animation loop: Take end-timestamp:
    tend = Screen('Flip', win);

    % Compute and show average framerate:
    avgfps = fcount / (tend - tstart) %#ok<NOPRT>
    avgdur = 1000/avgfps %#ok<NASGU,NOPRT>

    % Enable OpenGL context for cleanup work:
    Screen('BeginOpenGL', win);

    % Delete our sphere object:
    gluDeleteQuadric(mysphere);

    % Unselect our texture...
    glBindTexture(gltextarget, 0);

    % ... and disable texture mapping:
    glDisable(gltextarget);

    % End of OpenGL rendering...
    Screen('EndOpenGL', win);

    % Destroy FDF context, release all ressources:
    moglFDF('DestroyContext', fdf);

    % Close onscreen window and release all other ressources:
    Screen('CloseAll');
catch
    % In case of error, a Screen('CloseAll') will also do a good
    % post-mortem cleanup job:
    Screen('CloseAll');
    
    psychrethrow(psychlasterror);
end

% Well done!
return
