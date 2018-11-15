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
pulseEnd = 0;

while ~KbCheck
    WaitSecs('YieldSecs', 0.1);

    % Query and display all input state:
    istate = PsychVRHMD('GetInputState', hmd, OVR.ControllerType_Active);
    clc;
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

    if istate.Buttons(OVR.Button_Back)
        break;
    end

    fprintf('\n');

    if istate.Buttons(OVR.Button_A)
        if GetSecs < pulseEnd
            % Pressed A before end of previously initiated pulse. Abort current pulse:
            pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_XBox, 0, 0);
        else
            % Initiate new pulse: 2.5 (max) seconds, 100% frequency, 0.8 amplitude:
            pulseEnd = PsychVRHMD('HapticPulse', hmd, OVR.ControllerType_XBox, [], 1.0, 0.8)
            % Debounce button:
            WaitSecs('YieldSecs', 0.5);
        end
    end
end

sca;
