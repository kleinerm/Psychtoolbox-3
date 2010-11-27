function KinectDemo
% KinectDemo - Capture and display video and depths data from a Kinect box.
%
% This connects to a Microsoft Kinect device on the USB bus, then captures
% and displays video and depths data delivered by the Kinect.
%
% This is an early prototype.
%

% History:
% 25.11.2010  mk  Written.

AssertOpenGL
%PsychDebugWindowConfiguration;
InitializeMatlabOpenGL([],[],1);

dst1 = [0, 0, 640, 480];
dst2 = [650, 0, 650+640, 480];

w = Screen('Openwindow', 0, 0);
kinect = PsychKinect('Open');
PsychKinect('Start', kinect);

while 1
    [rc, cts] = PsychKinect('GrabFrame', kinect);
    if rc > 0
        [imbuff, width, height, channels] = PsychKinect('GetImage', kinect, 0, 1);
        tex = Screen('SetOpenGLTextureFromMemPointer', w, [], imbuff, width, height, channels, 0, GL.TEXTURE_RECTANGLE_EXT);
        Screen('DrawTexture', w, tex, [], dst1);
        Screen('Close', tex);

        [imbuff, width, height, channels] = PsychKinect('GetImage', kinect, 1, 1);
        tex = Screen('SetOpenGLTextureFromMemPointer', w, [], imbuff, width, height, channels, 0, GL.TEXTURE_RECTANGLE_EXT);
        Screen('DrawTexture', w, tex, [], dst2);
        Screen('Close', tex);

        PsychKinect('ReleaseFrame', kinect);
        Screen('Flip', w);
    end
    
    if KbCheck
        break;
    end
end

PsychKinect('Stop', kinect);
PsychKinect('Close', kinect);
sca;
