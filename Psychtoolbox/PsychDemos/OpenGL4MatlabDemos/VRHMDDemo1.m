function VRHMDDemo1(doSeparateEyeRender, multiSample, fountain)
% VRHMDDemo1 -- Show 3D stereo display via MOGL OpenGL on a VR headset.
%
% Usage: VRHMDDemo1([doSeparateEyeRender=1][, multiSample=0][, fountain=0]);
%
% Pressing any key continues the demo and progresses to next
% subdemo. Mouse clicks will pause some demos, until another mouse click
% continues the demo.
%
% Optional parameters:
%
% 'doSeparateEyeRender' if set to 1, perform per eye render passes in an optimized
% order, as recommended by the HMD driver, and query the per eye camera matrices
% individually during each render pass to optimize for head tracking prediction
% accuracy. If set to 0, query matrices for rendering simultaneously for both eyes,
% and render in a potentially non-optimized order. The latter is a bit simpler, but
% potentially less accurate, causing additional motion artifacts. Defaults to 1, ie.
% use more complex but potentially more optimal method in terms of quality.
%
% 'multiSample' if set to a non-zero value will enable multi-sample
% anti-aliasing. This however usually doesn't give good results with
% smoothed 3D dots.
%
% 'fountain' if set to 1 will also emit a particle fountain from the nozzle
% of the teapot. Nicer, but higher gpu load.
%
% Press any key to end the demo.
%
% Mouse left/right = Global camera movement left/right.
% Mouse up/down = Global camera movement up/down.
% Mouse up/down + Left mouse button pressed = Global camera movement forward/backward.
%

% History:
% 10-Sep-2015  mk  Written. Derived from DrawDots3DDemo.m

% GL data structure needed for all OpenGL demos:
global GL;

if nargin < 1 || isempty(doSeparateEyeRender)
  doSeparateEyeRender = 1;
end

if nargin < 2 || isempty(multiSample)
  multiSample = 0;
end

if nargin < 3 || isempty(fountain)
  fountain = 0;
end

% Default setup:
PsychDefaultSetup(2);
%RestrictKeysForKbCheck(KbName('ESCAPE'));

% Find the screen to use for display:
screenid = max(Screen('Screens'));

try
  % Setup Psychtoolbox for OpenGL 3D rendering support and initialize the
  % mogl OpenGL for Matlab/Octave wrapper:
  InitializeMatlabOpenGL;

  % Setup the HMD and open and setup the onscreen window for VR display:
  PsychImaging('PrepareConfiguration');
  hmd = PsychOculusVR('AutoSetupDefaultHMD');
  [win, winRect] = PsychImaging('OpenWindow', screenid, 0, [], [], [], [], multiSample);

  % Textsize for text:
  Screen('TextSize', win, 18);

  % Setup the OpenGL rendering context of the onscreen window for use by
  % OpenGL wrapper. After this command, all following OpenGL commands will
  % draw into the onscreen window 'win':
  Screen('BeginOpenGL', win);

  % Set viewport properly:
  glViewport(0, 0, RectWidth(winRect), RectHeight(winRect));

  % Setup default drawing color to yellow (R,G,B)=(1,1,0). This color only
  % gets used when lighting is disabled - if you comment out the call to
  % glEnable(GL.LIGHTING).
  glColor3f(1,1,0);

  % Setup OpenGL local lighting model: The lighting model supported by
  % OpenGL is a local Phong model with Gouraud shading.

  % Enable the first local light source GL.LIGHT_0. Each OpenGL
  % implementation is guaranteed to support at least 8 light sources,
  % GL.LIGHT0, ..., GL.LIGHT7
  glEnable(GL.LIGHT0);

  % Enable alpha-blending for smooth dot drawing:
  glEnable(GL.BLEND);
  glBlendFunc(GL.SRC_ALPHA, GL.ONE_MINUS_SRC_ALPHA);

  % Set projection matrix: This defines a perspective projection,
  % corresponding to the model of a pin-hole camera - which is a good
  % approximation of the human eye and of standard real world cameras --
  % well, the best aproximation one can do with 3 lines of code ;-)
  glMatrixMode(GL.PROJECTION);

  % Retrieve projection matrix for optimal rendering on the HMD:
  [projL, projR] = PsychOculusVR('GetStaticRenderParameters', hmd);

  % Set left cameras matrix:
  glLoadMatrixd(projL);

  % Setup modelview matrix: This defines the position, orientation and
  % looking direction of the virtual camera:
  glMatrixMode(GL.MODELVIEW);
  glLoadIdentity;

  % Our point lightsource is at position (x,y,z) == (1,2,3)...
%  glLightfv(GL.LIGHT0,GL.POSITION,[ 1 2 3 0 ]);

  % Set background clear color to 'black' (R,G,B,A)=(0,0,0,0):
  glClearColor(0,0,0,0);

  % Clear out the backbuffer: This also cleans the depth-buffer for
  % proper occlusion handling: You need to glClear the depth buffer whenever
  % you redraw your scene, e.g., in an animation loop. Otherwise occlusion
  % handling will screw up in funny ways...
  glClear;

  % Finish OpenGL rendering into PTB window. This will switch back to the
  % standard 2D drawing functions of Screen and will check for OpenGL errors.
  Screen('EndOpenGL', win);

  % Second version: Does use occlusion testing via depth buffer, does use
  % lighting. Uses manual switching between 2D and 3D for higher efficiency.
  % Creates a real 3D point-cloud around a teapot, as well as a vertex-shaded
  % fountain of particles that is emitted by the teapot:

  % Number of random dots, whose positions are computed in Matlab on CPU:
  ndots = 100;

  % Number of fountain particles whose positions are computed on the GPU:
  nparticles = 10000;

  % Diameter of particles in pixels:
  particleSize = 5;

  % 'StartPosition' is the 3D position where all particles originate. It is
  % faked to a position, so that the particles seem to originate from the
  % teapots "nozzle":
  StartPosition = [1.44, 0.40, 0.0];

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

  % Assign static 3D startposition for fountain:
  glUniform3f(glGetUniformLocation(glsl, 'StartPosition'), StartPosition(1), StartPosition(2), StartPosition(3));

  % Assign lifetime:
  glUniform1f(glGetUniformLocation(glsl, 'LifeTime'), particlelifetime);

  % Assign simulated gravity constant 'g' for proper trajectory:
  glUniform1f(glGetUniformLocation(glsl, 'Acceleration'), 1.5);

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
  vxmax = maxspeed;
  vymax = maxspeed;
  vzmax = 0.4 * maxspeed;

  % Assign random velocities in (vx,vy,vz) direction: Intervals chosen to
  % shape the beam into something visually pleasing for a teapot:
  particlesxyzt(1,:) = RandLim([1, nparticles],    0.7, +vxmax);
  particlesxyzt(2,:) = RandLim([1, nparticles],    0.7, +vymax);
  particlesxyzt(3,:) = RandLim([1, nparticles], -vzmax, +vzmax);

  % The w-component (4th dimension) encodes the birthtime of the particle. We
  % assign random birthtimes within the possible particlelifetime to get a
  % nice continuous stream of particles. Well, kind of: The flowfactor
  % controls the "burstiness" of particle flow. A value of 1 will create a
  % continous stream, whereas smaller values will create bursts of particles,
  % as if the teapot is choking:
  particlesxyzt(4,:) = RandLim([1, nparticles], 0.0, particlelifetime * flowfactor);

  % Manually enable 3D mode:
  Screen('BeginOpenGL', win);

  % Predraw the particles. Here particlesxyzt does not encode position, but
  % speed -- this because our shader interprets positions as velocities!
  gld = glGenLists(1);
  glNewList(gld, GL.COMPILE);
  moglDrawDots3D(win, particlesxyzt, particleSize, particlecolors, -StartPosition, 1);
  glEndList;

  % Enable lighting:
  glEnable(GL.LIGHTING);

  % Enable proper occlusion handling via depth tests:
  glEnable(GL.DEPTH_TEST);

  % Set light position:
%  glLightfv(GL.LIGHT0,GL.POSITION,[ 1 2 3 0 ]);

  % Manually disable 3D mode.
  Screen('EndOpenGL', win);

  telapsed = 0;
  fcount = 0;

  % Allocate for up to 1000 seconds at 75 fps:
  gpudur = zeros(1, 75 * 1000);

  % Make sure all keys are released:
  KbReleaseWait;

  Priority(MaxPriority(win));

  % Start the headtracker of the HMD:
  PsychOculusVR('Start', hmd);
  PsychOculusVRCore ('Verbosity', 2);

  eyeShift(1, :) = -1 * PsychOculusVR('GetEyeShiftVector', hmd, 0);
  eyeShift(2, :) = -1 * PsychOculusVR('GetEyeShiftVector', hmd, 1);

  % Get duration of a single frame:
  ifi = Screen('GetFlipInterval', win);

  % Initial flip to sync us to VBL and get start timestamp:
  vbl = Screen('Flip', win);
  tstart = vbl;
  globalPos = [0, 0, 0];
  [xo, yo] = GetMouse;

  % VR render loop: Runs until keypress:
  while ~KbCheck
    % Update global position (x,y,z) by mouse movement:
    [xm, ym, buttons] = GetMouse;
    globalPos(1) = globalPos(1) + 0.005 * (xm - xo);
    globalPos(2 + buttons(1)) = globalPos(2 + buttons(1)) + 0.005 * (ym - yo);
    xo = xm;
    yo = ym;

    % Track head position and orientation, retrieve modelview camera matrices for each eye:
    [eyePoseL, eyePoseR] = PsychOculusVR('StartRender', hmd);

    % Start rendertime measurement on GPU: 'gpumeasure' will be 1 if
    % this is supported by the current GPU + driver combo:
    gpumeasure = 0;
    gpumeasure = Screen('GetWindowInfo', win, 5);

    % We render the scene separately for each eye:
    for view = 0:1
      if doSeparateEyeRender
        % Query which eye to render in this 'view' renderpass, and query its
        % eyePose vector for the predicted eye position to use for the virtual
        % camera rendering that eyes view. The returned pose vector actually
        % describes tracked head pose, ie. HMD position and orientation in space.
        [headPose, eyeIndex] = PsychOculusVR('GetEyePose', hmd, view);

        % Select 'eyeIndex' to render (left- or right-eye):
        Screen('SelectStereoDrawbuffer', win, eyeIndex);

        % Add additional global translation, mouse controlled in 2D x-z plane:
        headPose(1:3) = headPose(1:3) + globalPos;

        % Setup modelView matrix:
        modelView = eyePoseToCameraGLModelviewMatrix(headPose, eyeShift(eyeIndex + 1, :));
      else
        % Select 'view' to render (left- or right-eye):
        Screen('SelectStereoDrawbuffer', win, view);

        % Setup modelView matrix:
        if view == 0
            modelView = eyePoseToCameraGLModelviewMatrix(eyePoseL);
        else
            modelView = eyePoseToCameraGLModelviewMatrix(eyePoseR);
        end
      end

      % Manually reenable 3D mode in preparation of eye draw cycle:
      Screen('BeginOpenGL', win);

      % Setup camera position and orientation for this eyes 'view':
      glMatrixMode(GL.MODELVIEW);
      glLoadMatrixd(modelView);

      glLightfv(GL.LIGHT0,GL.POSITION,[ 1 2 3 0 ]);

      % Clear color and depths buffers:
      glClear;

      % Position teapot at origin - 1.5 m in z direction:
      glTranslatef(0, 0, -1.5);

      % Bring a bit of extra spin into this :-)
      glRotated(10 * telapsed, 0, 1, 0);
      glRotated(5  * telapsed, 1, 0, 0);

      % Draw a solid teapot of size 1.0:
      glutSolidTeapot(1);

      % Compute simulation time for this draw cycle:
      telapsed = (vbl - tstart) * 1;

      if fountain
        % Draw the particle fountain. We use a vertex shader in the shader
        % program glsl to compute the physics:
        glUseProgram(glsl);

        % Assign updated simulation time to shader:
        glUniform1f(glGetUniformLocation(glsl, 'Time'), telapsed);

        % Draw the particles. Here particlesxyzt does not encode position,
        % but speed vectors -- this because our shader interprets positions
        % as velocities!
        moglDrawDots3D(win, particlesxyzt, particleSize, particlecolors, [], 1);
        %glCallList(gld);

        % Done with shaded drawing:
        glUseProgram(0);
      end

      % Manually disable 3D mode before switching to other eye or to flip:
      Screen('EndOpenGL', win);

      % Screen('FillRect', win, [1,1,1], CenterRect([0 0 10 10], winRect));

      % Repeat for view of other eye:
    end

    % Stimulus ready. Show it on the HMD. We don't clear the color buffer here,
    % as this is done in the next iteration via glClear() call anyway:
    vbl = Screen('Flip', win, [], 1);
    fcount = fcount + 1;

    % Result of GPU time measurement expected?
    if gpumeasure
        % Retrieve results from GPU load measurement:
        % Need to poll, as this is asynchronous and non-blocking,
        % so may return a zero time value at first invocation(s),
        % depending on how deep the rendering pipeline is:
        while 1
            winfo = Screen('GetWindowInfo', win);
            if winfo.GPULastFrameRenderTime > 0
                break;
            end
        end

        % Store it:
        gpudur(fcount) = winfo.GPULastFrameRenderTime;
    end

    % Next frame ...
  end

  fps = fcount / (vbl - tstart);
  gpudur = gpudur(1:fcount);
  fprintf('Average framerate was %f fps. Average GPU rendertime per frame = %f msec.\n', fps, 1000 * mean(gpudur));

  Priority(0);

  % Now a benchmark run to test different strategies for their speed...

  KbReleaseWait;
  Screen('Flip', win);

  sca;
  plot(1000 * gpudur);

catch
  sca;
  psychrethrow(psychlasterror);
end
