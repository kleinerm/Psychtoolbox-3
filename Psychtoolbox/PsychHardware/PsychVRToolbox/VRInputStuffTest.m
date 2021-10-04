function res = VRInputStuffTest
% VRInputStuffTest - Test input functionality related to VR devices.
%
% Tries to enumerate available controllers and other properties related to
% input. After any key press or controller button press, reports live state of
% buttons, sensors, triggers etc. of connected controllers. Also exercises the
% haptic feedback functionality if any of left and right touch controllers if
% the A/B or X/Y buttons are pressed for low/high frequency rumble on right or
% left touch controller. After a keypress or Home button press on the controller,
% visualizes tracked hand position and orientation of hand controllers and allows
% to do some nice visual effects based on trigger / grip button presses, thumbsticks
% movement etc.
%
% Tested with XBox controller, Oculus remote, and the two Oculus touch controllers
% of the Rift CV-1 on Windows-10 and Linux.

% Constants for use in VR applications:
global OVR;

% GL data structure needed for all OpenGL demos:
global GL;

canary = onCleanup(@sca);

% Setup unified keymapping and unit color range:
PsychDefaultSetup(2);

% Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
% mogl OpenGL for Matlab/Octave wrapper:
InitializeMatlabOpenGL;

% Select screen with highest id as Oculus output display:
screenid = max(Screen('Screens'));

% Open our fullscreen onscreen window with black background clear color:
PsychImaging('PrepareConfiguration');

hmd = PsychVRHMD('AutoSetupHMD', 'Tracked3DVR', 'DebugDisplay');
if isempty(hmd)
    fprintf('No VR HMDs connected. Game over!\n');
    return;
end

PsychDebugWindowConfiguration;
[win, winRect] = PsychImaging('OpenWindow', screenid, [0 0 1]);
ifi = Screen('GetFlipInterval', win);
hmdinfo = PsychVRHMD('GetInfo', hmd);

clc;

if strcmpi(hmdinfo.subtype, 'Oculus-1')
    fprintf('Properties of our subject:\n\n');
    fprintf('Player height: %f\n', PsychOculusVR1('FloatProperty', hmd, OVR.KEY_PLAYER_HEIGHT));
    fprintf('Player eye height: %f\n', PsychOculusVR1('FloatProperty', hmd, OVR.KEY_EYE_HEIGHT));
    fprintf('Player neck-to-eye: %f\n', PsychOculusVR1('FloatsProperty', hmd, OVR.KEY_NECK_TO_EYE_DISTANCE));
    fprintf('Player eye-to-nose: %f\n', PsychOculusVR1('FloatsProperty', hmd, OVR.KEY_EYE_TO_NOSE_DISTANCE));
    % Suppress error output during username query, as these properties are often undefined and
    % would spill pointless error messages:
    oldverbosity = PsychOculusVR1('Verbosity', 0);
    fprintf('User name: %s\n', PsychOculusVR1('StringProperty', hmd, OVR.KEY_USER, 'Hans'));
    fprintf('Player name: %s\n', PsychOculusVR1('StringProperty', hmd, OVR.KEY_NAME, 'Mueller'));
    PsychOculusVR1('Verbosity', oldverbosity);
    fprintf('Player gender: %s\n', PsychOculusVR1('StringProperty', hmd, OVR.KEY_GENDER, OVR.KEY_DEFAULT_GENDER));
    fprintf('\n\n');
end

if hmdinfo.VRControllersSupported
    fprintf('\n\nConnected controllers:\n\n');
    controllerTypes = PsychVRHMD('Controllers', hmd);
    if bitand(controllerTypes, OVR.ControllerType_Remote)
        fprintf('Remote control connected.\n');
    end
    if bitand(controllerTypes, OVR.ControllerType_XBox)
        fprintf('XBox controller connected.\n');
    end
    if bitand(controllerTypes, OVR.ControllerType_LTouch)
        fprintf('Left hand controller connected.\n');
    end
    if bitand(controllerTypes, OVR.ControllerType_RTouch)
        fprintf('Right hand controller connected.\n');
    end
    fprintf('\n\n');
end

% Fetch play area / guardian boundaries:
[isVisible, playboundsxyz, outerboundsxyz] = PsychVRHMD('VRAreaBoundary', hmd);

fprintf('\n\nPress any key or controller button to continue. Will continue in 10 seconds automatically.\n');
DrawFormattedText(win, 'Press any key or controller button to continue. Will continue in 10 seconds automatically.', 'center', 'center', [1 1 0], 20);
Screen('Flip', win);

timeout = GetSecs + 10;
while 1
    down = KbCheck(-1);
    istate = PsychVRHMD('GetInputState', hmd, OVR.ControllerType_Active);
    if any(istate.Buttons) || down || GetSecs > timeout
        break;
    end
end

KbReleaseWait(-1);
while any(istate.Buttons)
    istate = PsychVRHMD('GetInputState', hmd, OVR.ControllerType_Active);
end

pulseEnd = 0;

while 1
    WaitSecs('YieldSecs', 0.1);
    clc;

    % Show instructions in HMD on how to continue:
    DrawFormattedText(win, 'Press BackSpace key on keyboard, or Enter-Button or\nBack-Button on controller', 'center', 'center', [1 1 0]);
    Screen('Flip', win);

    % Query and display all input state:
    istate = PsychVRHMD('GetInputState', hmd, OVR.ControllerType_Active);

    if ~istate.Valid
        fprintf('No VR controller input available, possibly due to VR input focus loss.\n');
        fprintf('Make sure our application has input focus and possibly that the HMD is on the users head.\n');
        continue;
    end

    fprintf('Press Back button on remote control or other controllers, or Enter-Button on controller, or backspace key, to finish.\n\n');
    disp(istate);

    if istate.Buttons(OVR.Button_A)
        fprintf('Button_A ');
    end

    if istate.Buttons(OVR.Button_B)
        fprintf('Button_B ');
    end

    if istate.Buttons(OVR.Button_X)
        fprintf('Button_X ');
    end

    if istate.Buttons(OVR.Button_Y)
        fprintf('Button_Y ');
    end

    if istate.Buttons(OVR.Button_RThumb)
        fprintf('Button_RThumb ');
    end

    if istate.Buttons(OVR.Button_RShoulder)
        fprintf('Button_RShoulder ');
    end

    if istate.Buttons(OVR.Button_LThumb)
        fprintf('Button_LThumb ');
    end

    if istate.Buttons(OVR.Button_LShoulder)
        fprintf('Button_LShoulder ');
    end

    if istate.Buttons(OVR.Button_Up)
        fprintf('Button_Up ');
    end

    if istate.Buttons(OVR.Button_Down)
        fprintf('Button_Down ');
    end

    if istate.Buttons(OVR.Button_Left)
        fprintf('Button_Left ');
    end

    if istate.Buttons(OVR.Button_Right)
        fprintf('Button_Right ');
    end

    if istate.Buttons(OVR.Button_Enter)
        fprintf('Button_Enter ');
    end

    if istate.Buttons(OVR.Button_Back)
        fprintf('Button_Back ');
    end

    if istate.Buttons(OVR.Button_VolUp)
        fprintf('Button_VolUp ');
    end

    if istate.Buttons(OVR.Button_VolDown)
        fprintf('Button_VolDown ');
    end

    if istate.Buttons(OVR.Button_Home)
        fprintf('Button_Home ');
    end

    if any(istate.Buttons(OVR.Button_RMask))
        fprintf('Button_RMask ');
    end

    if any(istate.Buttons(OVR.Button_LMask))
        fprintf('Button_LMask ');
    end

    if any(istate.Buttons(OVR.Button_Private))
        fprintf('Button_Private ');
    end

    [~, ~, keycode] = KbCheck(-1);
    if istate.Buttons(OVR.Button_Back) || istate.Buttons(OVR.Button_Enter) || keycode(KbName('BackSpace'))
        break;
    end

    fprintf('\n');

    % Oculus VR 1.11+ controlled VR setup?
    if strcmpi(hmdinfo.subtype, 'Oculus-1')
        % Get info about collisions of tracked devices with the VR play area boundaries:
        [isTriggering, closestDistance, closestPointxyz, surfaceNormal] = PsychOculusVR1('TestVRBoundary', hmd, OVR.TrackedDevice_All, 0);
        if isTriggering
            fprintf('VR collision: closestDistance %f m, point [%f,%f,%f], normal [%f,%f,%f]\n', closestDistance, closestPointxyz(1), closestPointxyz(2), ...
                    closestPointxyz(3), surfaceNormal(1), surfaceNormal(2), surfaceNormal(3));
        end

        % Get info about collision of test point with the VR play area boundaries:
        mypoint = [1, 2, 3];
        [isTriggering, closestDistance, closestPointxyz, surfaceNormal] = PsychOculusVR1('TestVRBoundaryPoint', hmd, mypoint, 0);
        if isTriggering
            fprintf('VR mypoint collision: closestDistance %f m, point [%f,%f,%f], normal [%f,%f,%f]\n', closestDistance, closestPointxyz(1), closestPointxyz(2), ...
                    closestPointxyz(3), surfaceNormal(1), surfaceNormal(2), surfaceNormal(3));
        end
    end

    if (istate.Buttons(OVR.Button_A) || istate.Buttons(OVR.Button_B)) && hmdinfo.hapticFeedbackSupported
        if GetSecs < pulseEnd
            % Pressed A before end of previously initiated pulse. Abort current pulse:
            pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_RTouch, [], 0, 0);
        else
            % Initiate new pulse: 0.75 seconds, 25% or 100% frequency, 0.8 amplitude:
            if istate.Buttons(OVR.Button_B)
                pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_RTouch, 0.75, 1.0, 0.8);
            else
                pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_RTouch, 0.75, 0.25, 0.8);
            end
            % Debounce button:
            WaitSecs('YieldSecs', 0.1);
        end
    end

    if (istate.Buttons(OVR.Button_X) || istate.Buttons(OVR.Button_Y)) && hmdinfo.hapticFeedbackSupported
        if GetSecs < pulseEnd
            % Pressed A before end of previously initiated pulse. Abort current pulse:
            pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_LTouch, [], 0, 0);
        else
            % Initiate new pulse: 0.75 seconds, 25% or 100% frequency, 0.8 amplitude:
            if istate.Buttons(OVR.Button_Y)
                pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_LTouch, 0.75, 1.0, 0.8);
            else
                pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_LTouch, 0.75, 0.25, 0.8);
            end
            % Debounce button:
            WaitSecs('YieldSecs', 0.1);
        end
    end

    if (istate.Buttons(OVR.Button_Up) || istate.Buttons(OVR.Button_Down)) && hmdinfo.hapticFeedbackSupported
        if GetSecs < pulseEnd
            % Pressed A before end of previously initiated pulse. Abort current pulse:
            pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_XBox, [], 0, 0);
        else
            % Initiate new pulse: 0.75 seconds, 25% or 100% frequency, 0.8 amplitude:
            if istate.Buttons(OVR.Button_Up)
                pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_XBox, 0.75, 1.0, 0.8);
            else
                pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_XBox, 0.75, 0.25, 0.8);
            end
            % Debounce button:
            WaitSecs('YieldSecs', 0.1);
        end
    end
end

while any(istate.Buttons)
    istate = PsychVRHMD('GetInputState', hmd, OVR.ControllerType_Active);
end

% Part 3: Actual hand tracking and visualisation:
if hmdinfo.handTrackingSupported
  % Number of fountain particles whose positions are computed on the GPU:
  nparticles = 10000;

  % Diameter of particles in pixels:
  particleSize = 5;

  % 'StartPosition' is the 3D position where all particles originate. It is
  % faked to a position, so that the particles seem to originate from the
  % teapots "nozzle":

  % Lifetime for each simulated particle, is chosen so that there seems to be
  % an infinite stream of particles, although the same particles are recycled
  % over and over:
  particlelifetime = 2;

  % Amount of "flow": A value of 1 will create a continuous stream, whereas
  % smaller value create bursts of particles:
  flowfactor = 1;

  % Load and setup the vertex shader for particle fountain animation:
  shaderpath = [PsychtoolboxRoot 'PsychDemos/OpenGL4MatlabDemos/GLSLDemoShaders/ParticleSimple'];
  glsl = LoadGLSLProgramFromFiles(shaderpath,1);

  % Bind shader so it can be setup:
  glUseProgram(glsl);

  % Assign static 3D startposition for fountain at tip of nozzles:
  StartPosition = [0, 0, -0.4];
  glUniform3f(glGetUniformLocation(glsl, 'StartPosition'), StartPosition(1), StartPosition(2), StartPosition(3));

  % Assign lifetime:
  glUniform1f(glGetUniformLocation(glsl, 'LifeTime'), particlelifetime);

  % Assign simulated gravity constant 'g' for proper trajectory:
  glUniform1f(glGetUniformLocation(glsl, 'Acceleration'), 0.5);

  % Done with setup:
  glUseProgram(0);

  % Assign random RGB colors to the particles: The shader will use these, but
  % also assign an alpha value that makes the particles "fade out" at the end
  % of there lifetime:
  particlecolors = rand(3, nparticles);
  
  % Maximum speed for particles:
  maxspeed = 1.25;

  % Per-component speed: We select these to shape the fountain in our wanted
  % direction:
  vxmax = 0.1 * maxspeed;
  vymax = 0.1 * maxspeed;
  vzmax = maxspeed;

  % Assign random velocities in (vx,vy,vz) direction: Intervals chosen to
  % shape the beam into something visually pleasing for a teapot:
  particlesxyzt(1,:) = RandLim([1, nparticles], -vxmax, +vxmax); % Beam dispersion horizontal.
  particlesxyzt(2,:) = RandLim([1, nparticles], -vymax, +vymax); % Beam dispersion vertical.
  particlesxyzt(3,:) = RandLim([1, nparticles], -vzmax, -0.7 * vzmax); % Emission speed range.

  % The w-component (4th dimension) encodes the birthtime of the particle. We
  % assign random birthtimes within the possible particlelifetime to get a
  % nice continuous stream of particles. Well, kind of: The flowfactor
  % controls the "burstiness" of particle flow. A value of 1 will create a
  % continous stream, whereas smaller values will create bursts of particles,
  % as if the teapot is choking:
  particlesxyzt(4,:) = RandLim([1, nparticles], 0.0, particlelifetime * flowfactor);

  % Setup the OpenGL rendering context of the onscreen window for use by the
  % OpenGL wrapper. After this command, all following OpenGL commands will
  % draw into the onscreen window 'win':
  Screen('BeginOpenGL', win);

  % Set viewport properly:
  glViewport(0, 0, RectWidth(winRect), RectHeight(winRect));

  % Setup default drawing color to yellow (R,G,B)=(1,1,0). This color only
  % gets used when lighting is disabled - if you comment out the call to
  % glEnable(GL.LIGHTING).
  glColor3f(1,1,0);

  % Enable the first local light source GL.LIGHT_0.
  glEnable(GL.LIGHT0);

  % Enable alpha-blending for smooth dot drawing:
  glEnable(GL.BLEND);
  glBlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA);

  % Retrieve and set camera projection matrix for optimal rendering on the HMD:
  [projMatrix{1}, projMatrix{2}] = PsychVRHMD('GetStaticRenderParameters', hmd);

  % Setup modelview matrix: This defines the position, orientation and
  % looking direction of the virtual camera:
  glMatrixMode(GL.MODELVIEW);
  glLoadIdentity;

  % Set background clear color to 'black' (R,G,B,A)=(0,0,0,0):
  glClearColor(0,0,0,0);

  % Clear out the backbuffer: This also cleans the depth-buffer for
  % proper occlusion handling: You need to glClear the depth buffer whenever
  % you redraw your scene, e.g., in an animation loop. Otherwise occlusion
  % handling will screw up in funny ways...
  glClear;

  % Enable lighting:
  glEnable(GL.LIGHTING);

  % Enable proper occlusion handling via depth tests:
  glEnable(GL.DEPTH_TEST);

  % Predraw the particles. Here particlesxyzt does not encode position, but
  % speed -- this because our shader interprets positions as velocities!
  gld = glGenLists(1);
  glNewList(gld, GL.COMPILE);
  moglDrawDots3D(win, particlesxyzt, particleSize, particlecolors, [], 1);
  glEndList;

  % Finish OpenGL rendering into PTB window. This will switch back to the
  % standard 2D drawing functions of Screen and will check for OpenGL errors.
  Screen('EndOpenGL', win);

  % Make sure all keys are released:
  KbReleaseWait;

  % Realtime scheduling:
  Priority(MaxPriority(win));

  fcount = 0;
  globalPos = [0, 0, 3];
  heading = 0;
  pulseEnd = [];

  if ~bitand(controllerTypes, OVR.ControllerType_LTouch + OVR.ControllerType_RTouch)
    [xc, yc] = RectCenter(winRect);
    SetMouse(xc,yc, screenid);
    [xo, yo] = GetMouse(screenid);
  else
    SetMouse(0, 0, screenid);
  end

  HideCursor(screenid);

  % Initial flip to sync us to VBL and get start timestamp:
  vbl = Screen('Flip', win);
  tstart = vbl;

  % VR render loop: Runs until keypress:
  while ~KbCheck && ~(istate.Buttons(OVR.Button_Back) || istate.Buttons(OVR.Button_Enter))
    if ~bitand(controllerTypes, OVR.ControllerType_LTouch + OVR.ControllerType_RTouch)
        % Update global position (x,y,z) by mouse movement:
        [xm, ym, buttons] = GetMouse(screenid);
        if ~any(buttons)
          % x-movement:
          dx = (xm - xo);
          globalPos(1) = globalPos(1) + 0.005 * dx;

          % y-movement:
          dy = (yo - ym);
          globalPos(2) = globalPos(2) + 0.005 * dy;
        else
          if buttons(1)
            % z-movement:
            dz = (ym - yo);
            globalPos(3) = globalPos(3) + 0.005 * dz;
          end

          if buttons(2)
            % Heading, ie. looking direction:
            dh = (xm - xo);
            heading = heading + 0.01 * dh;
          end
        end

        % Reposition mouse cursor for next render cycle:
        SetMouse(xc,yc, screenid);
        [xo, yo] = GetMouse(screenid);
    end

    % Allow to use thumbsticks, if any, to move the teapot:
    if istate.Buttons(OVR.Button_LThumb) || istate.Buttons(OVR.Button_RThumb)
      thumbmult = 0.05;
    else
      thumbmult = 0.005;
    end

    globalPos(1) = globalPos(1) - thumbmult * istate.Thumbstick(1,1);
    globalPos(2) = globalPos(2) - thumbmult * istate.Thumbstick(2,1);
    globalPos(3) = globalPos(3) + thumbmult * istate.Thumbstick(2,2);

    % Compute a transformation matrix to globally position and orient the
    % observer in the scene. This allows mouse control of observer position
    % and heading on top of the head tracking:
    globalHeadPose = PsychGetPositionYawMatrix(globalPos, heading);

    % Track and predict head + hands position and orientation, retrieve modelview
    % camera matrices for rendering of each eye. Apply some global transformation
    % to returned camera matrices. In this case a translation + rotation, as defined
    % by the PsychGetPositionYawMatrix() helper function:
    state = PsychVRHMD('PrepareRender', hmd, globalHeadPose, 1+2);

    % Get controller input state, buttons, triggers etc.:
    istate = PsychVRHMD('GetInputState', hmd, OVR.ControllerType_Active);

    % We render the scene separately for each eye:
    for renderPass = 0:1
      % Selected 'view' to render (left- or right-eye):
      Screen('SelectStereoDrawbuffer', win, renderPass);

      % Extract modelView matrix for this renderPass's eye:
      modelView = state.modelView{renderPass + 1};

      % Manually reenable 3D mode in preparation of eye draw cycle:
      Screen('BeginOpenGL', win);

      % Set per-eye projection matrix: This defines a perspective projection,
      % corresponding to the model of a pin-hole camera - which is a good
      % approximation of the human eye and of standard real world cameras --
      % well, the best aproximation one can do with 2 lines of code ;-)
      glMatrixMode(GL.PROJECTION);
      glLoadMatrixd(projMatrix{renderPass+1});

      % Setup camera position and orientation for this eyes view:
      glMatrixMode(GL.MODELVIEW);
      glLoadMatrixd(modelView);

      % Light position:
      glLightfv(GL.LIGHT0,GL.POSITION,[ 1 2 3 0 ]);

      % Clear color and depths buffers:
      glClear;

      % Visualize projection of guardian "walls" to the floor, if any are defined:
      if ~isempty(outerboundsxyz)
        glDisable(GL.LIGHTING);

        % Change color of guardian lines, depending if guardian grid visible or not:
        if PsychVRHMD('VRAreaBoundary', hmd);
          glColor3f(1.0, 0.0, 0.0);
        else
          glColor3f(1.0, 1.0, 0.0);
        end

        % Outer bounds - hard walls:
        glBegin(GL.LINE_LOOP);
          for i = 1:size(outerboundsxyz, 2)
            glVertex3dv(outerboundsxyz(:, i) + globalPos');
          end
        glEnd;

        % Inner play area - a  rectangle inscribed to the outer bounds:
        glBegin(GL.LINE_LOOP);
          for i = 1:size(playboundsxyz , 2)
            glVertex3dv(playboundsxyz(:, i) + globalPos');
          end
        glEnd;

        glEnable(GL.LIGHTING);
      end

      glPushMatrix;

      % Compute simulation time for this draw cycle:
      telapsed = vbl - tstart;

      % Bring a bit of extra spin into this :-)
      glRotated(10 * telapsed, 0, 1, 0);
      glRotated(5  * telapsed, 1, 0, 0);

      % Draw a solid teapot of size 1.0:
      glutSolidTeapot(1);

      % Done drawing the mighty teapot:
      glPopMatrix;

      % Visualize users hands / hand controllers:
      for hand = 1:2
        % Position and orientation of hand tracked? Otherwise we don't show them:
        if state.handStatus(hand) == 3
          % Yes: Lets visualize it:
          glPushMatrix;
          glMultMatrixd(state.globalHandPoseMatrix{hand});
          glutSolidCone(0.1 * (1.1 - istate.Grip(hand)), -0.4, 10, 10);

          if istate.Trigger(hand) > 0.015
            % Draw the particle fountain. We use a vertex shader in the shader
            % program glsl to compute the physics:
            glUseProgram(glsl);

            % Assign updated simulation time to shader:
            glUniform1f(glGetUniformLocation(glsl, 'Time'), telapsed);

            % Assign simulated gravity constant 'g' for proper trajectory:
            glUniform1f(glGetUniformLocation(glsl, 'Acceleration'), 1 - istate.Trigger(hand));

            % Draw the particles: We have preencoded them into a OpenGL display list
            % above for higher performance of drawing:
            glCallList(gld);

            % Done with shaded drawing:
            glUseProgram(0);
          end

          glPopMatrix;
        end
      end

      % Manually disable 3D mode before switching to other eye or to flip:
      Screen('EndOpenGL', win);

      % Repeat for renderPass of other eye:
    end

    % Head position tracked?
    if ~bitand(state.tracked, 2)
      % Nope, user out of cameras view frustum. Tell it like it is:
      DrawFormattedText(win, 'Vision based tracking lost\nGet back into the cameras field of view!', 'center', 'center', [1 0 0]);
    end

    if hmdinfo.hapticFeedbackSupported
      if (istate.Grip(1) > 0.5 || istate.Grip(2) > 0.5) && isempty(pulseEnd)
          % Initiate new pulse: 0.75 seconds, 25% or 100% frequency, 0.8 amplitude:
          if istate.Grip(2) > 0.5
              controller = OVR.ControllerType_RTouch;
          else
              controller = OVR.ControllerType_LTouch;
          end

          PsychVRHMD('HapticPulse', hmd, controller, [], 0.25, 0.8);

          % Set end time for pulse:
          pulseEnd = GetSecs + 0.75;
      elseif GetSecs > pulseEnd
          % End current pulse:
          pulseEnd = [];
          PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_RTouch, [], 0, 0);
      end
    end

    % Stimulus ready. Show it on the HMD. We don't clear the color buffer here,
    % as this is done in the next iteration via glClear() call anyway:
    vbl = Screen('Flip', win, [], 1);
    fcount = fcount + 1;
  end

  % Cleanup:
  Priority(0);

  % Stats for nerds:
  fps = fcount / (vbl - tstart);
  fprintf('Average framerate was %f fps. Bye!\n', fps);
end

% Close windows, shutdown HMD, show cursor, general visual cleanup:
sca;
