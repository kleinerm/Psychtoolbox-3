function VideoCaptureDemo(fullscreen, fullsize, roi, depth, fps, deviceId, cameraname, bpc)
% Demonstrate simple use of built-in video capture engine.
%
% VideoCaptureDemo([fullscreen=0][, fullsize=0][, roi=[0 0 640 480]][, depth][, fps=realmax][,deviceId=0][, cameraname][, bpc=8])
%
% VideoCaptureDemo initializes the first attached and supported camera on
% your computer (e.g, the built-in iSight of Apple Macintosh computers),
% then shows its video image in a Psychtoolbox window.
%
% By default, a capture rate of 30 frames per second is requested, and the
% timecode and interframe interval of each captured image is displayed in
% the top-left corner of the display. A press of the ESCape key ends the
% demo.
%
% See also ImagingVideoCaptureDemo, VideoDelayloopMiniDemo and a few other
% nice demos.
%
% Optional parameters:
%
% 'fullscreen' If set to non-zero value, the image is displayed in a
% fullscreen window, as usual, otherwise a normal GUI window is used.
%
% 'fullsize' If set to 1, the cameras image is scaled up to full screen
% resolution, ie. so it fills the maximum amount of display area, but
% preserving the original aspect ratio.
%
% 'roi' Selects a rectangular subregion of the camera for display. By
% default, it selects a [0 0 640 480] rectangle, ie. the full area of a
% camera with 640 x 480 pixels resolution. This parameter may need tweaking
% for some cameras, as some drivers have bugs and don't work well with all
% settings.
%
% 'depth' Number of color channels 1 = grayscale, 3 = rgb, 4 = rgba etc.
%
% 'fps' Target capture framerate. Maximum for given resolution and color depth
% if omitted.
%
% 'deviceId' Device index of video capture device. Defaults to system default.
%
% 'cameraname' Name string for selection of video capture device. This is
% only honored if 'deviceId' is a negative number, and only for certain
% video capture plugins. Defaults to none.
%
% 'bpc' Optional bit depth in bits per channel. Defaults to classic 8 bpc, but
% some cameras support up to 16 bpc. Setting 16 bpc will try to coerce those into
% providing "HDR" data. Usually this works with higher end firewire cameras and
% the dc1394 capture engine. Your mileage with standard consumer cameras and the
% default GStreamer capture engine will likely be less great. If at all, it would
% probably only work on Linux or on OSX with GStreamer built from source, so you
% have the camerabin1 plugin available.
%

% History:
% Written sometimes 2006 by MK.

AssertOpenGL;

if nargin < 1
    fullscreen=[];
end

if isempty(fullscreen)
    fullscreen=0;
end;

if nargin < 2
    fullsize=[];
end

if isempty(fullsize)
    fullsize=0;
end

if nargin < 3
    roi = [];
end

if nargin < 4
    depth = [];
end

if nargin < 5
    fps = realmax;
end

if nargin < 6
    deviceId = [];
end

if nargin < 7
    cameraname = [];
end

if nargin < 8
    % Default bpc to internal 8 bpc default:
    bpc = [];
end

screenid=max(Screen('Screens'));

try
    if fullscreen<1
        win=Screen('OpenWindow', screenid, 0, [0 0 800 600]);
    else
        win=Screen('OpenWindow', screenid, 0);
    end;
    
    % Initial flip to a blank screen:
    Screen('Flip',win);
    
    % Set text size for info text. 24 pixels is also good for Linux.
    Screen('TextSize', win, 24);
    
    grabber = Screen('OpenVideoCapture', win, deviceId, roi, depth, [], [], cameraname, [], [], bpc);
    %brightness = Screen('SetVideoCaptureParameter', grabber, 'Brightness',383)
    %exposure = Screen('SetVideoCaptureParameter', grabber, 'Exposure',130)
    %gain = Screen('SetVideoCaptureParameter', grabber, 'Gain')
    %gamma = Screen('SetVideoCaptureParameter', grabber, 'Gamma')
    %shutter = Screen('SetVideoCaptureParameter', grabber, 'Shutter', 7)
    %Screen('SetVideoCaptureParameter', grabber, 'PrintParameters')
    %vendor = Screen('SetVideoCaptureParameter', grabber, 'GetVendorname')
    %model  = Screen('SetVideoCaptureParameter', grabber, 'GetModelname')
    %fps  = Screen('SetVideoCaptureParameter', grabber, 'GetFramerate')
    %roi  = Screen('SetVideoCaptureParameter', grabber, 'GetROI')
    
    for repcount=1:1
        Screen('StartVideoCapture', grabber, fps, 1);
        
        dstRect = [];
        oldpts = 0;
        count = 0;
        t=GetSecs;
        while (GetSecs - t) < 600
            if KbCheck
                break;
            end;
            
            [tex pts nrdropped,intensity]=Screen('GetCapturedImage', win, grabber, 1); %#ok<ASGLU,NASGU>
            % fprintf('tex = %i  pts = %f nrdropped = %i\n', tex, pts, nrdropped);
            
            if (tex>0)
                % Perform first-time setup of transformations, if needed:
                if fullsize && (count == 0)
                    texrect = Screen('Rect', tex);
                    winrect = Screen('Rect', win);
                    sf = min([RectWidth(winrect) / RectWidth(texrect) , RectHeight(winrect) / RectHeight(texrect)]);
                    dstRect = CenterRect(ScaleRect(texrect, sf, sf) , winrect);
                end
                
                % texinfo = Screen('Getwindowinfo', tex)
                % outintens = intensity
                
                % Draw new texture from framegrabber.
                Screen('DrawTexture', win, tex, [], dstRect);
                
                %Screen('glPopMatrix', win);
                
                % Print pts:
                Screen('DrawText', win, sprintf('%.4f', pts - t), 0, 0, 255);
                if count>0
                    % Compute delta:
                    delta = (pts - oldpts) * 1000;
                    oldpts = pts;
                    Screen('DrawText', win, sprintf('%.4f', delta), 0, 20, 255);
                end;
                
                % Show it.
                Screen('Flip', win);
                Screen('Close', tex);
                tex=0; %#ok<NASGU>
            end;
            count = count + 1;
        end;
        telapsed = GetSecs - t %#ok<NOPRT>
        Screen('StopVideoCapture', grabber);
    end
    Screen('CloseVideoCapture', grabber);
    Screen('CloseAll');
    avgfps = count / telapsed %#ok<NOPRT,NASGU>
catch %#ok<CTCH>
    Screen('CloseAll');
end
