function res = VRInputStuffTest
% VRInputStuffTest - Test input functionality related to VR devices.

global OVR;

% Setup unified keymapping and unit color range:
PsychDefaultSetup(2);

% Select screen with highest id as Oculus output display:
screenid = max(Screen('Screens'));

% Open our fullscreen onscreen window with black background clear color:
PsychImaging('PrepareConfiguration');

hmd = PsychVRHMD('AutoSetupHMD');
[win, rect] = PsychImaging('OpenWindow', screenid);
ifi = Screen('GetFlipInterval', win);

controllerTypes = PsychVRHMD('GetConnectedControllers', hmd);
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

[isVisible, playboundsxyz, outerboundsxyz] = PsychVRHMD('VRAreaBoundary', hmd)

pause(4);

while 0 % ~KbCheck
    % Query and display all input state:
    istate = PsychVRHMD('GetInputState', hmd, OVR.ControllerType_Active);
    clc;
    disp(istate);

    if istate.Buttons(OVR.Button_Back)
        break;
    end
end

sca;
