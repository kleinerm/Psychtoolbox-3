function ImageUndistortionDemo(calibfilename, imagefilename)
% ImageUndistortionDemo(calibfilename [, imagefilename])
%
% Very sketchy demo.

% History:
% 7/27/8 mk Written.

% Basic check if runnning on PTB-3:
AssertOpenGL;

% Choose output screen as usual:
screenid=max(Screen('Screens'));

% Only enable support for fast offscreen windows, don't need full blown pipeline here:
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'UseFastOffscreenWindows');
w=PsychImaging('OpenWindow', screenid, 0);

if nargin < 2
    imagefilename = [];
end

if isempty(imagefilename)
    imagefilename = [PsychtoolboxRoot 'PsychDemos/konijntjes1024x768.jpg'];
end

% Use our standard bunny picture as some test case:
img = imread(imagefilename);
mytex=Screen('MakeTexture', w, img);

% Size of input image to apply correction to -- The size of the offscreen window:
srcSize = Screen('Rect', mytex);

% Setup oversized offscreen window: This as an example of an oversized
% input image buffer. Could also use a normal texture from
% Screen('MakeTexture') or any other function that returns texture handles:
[exampleImage, woffrect] = Screen('OpenOffscreenWindow', w, 0, srcSize);

% Create an offscreen window of the size of the wanted undistorted and
% scaled down image, as a target buffer:
undistortedImage = Screen('OpenOffscreenWindow', w, 128, [0 0 1024 768]);

% Create an empty image processing operator for onscreen window 'w':
gloperator = CreateGLOperator(w);

% Create and add image undistortion operation to gloperator:
% Setup calibration: 'calibfilename' is the path to the calibration file
% that defines undistortion and scaling: 73, 73 is resolution of warp-mesh.
% Higher numbers == finer undistortion but longer draw time. 'exampleImage'
% must be the handle to a texture or offscreen window that has the same
% size, color depth and format as the input images you're gonna use later
% on, otherwise weird things will happen.
AddImageUndistortionToGLOperator(gloperator, exampleImage, calibfilename, 0, 73, 73);

% Draw bunnies into exampleImage, scale 'em up to offscreen window size:
Screen('DrawTexture', exampleImage, mytex, [], woffrect);
% Some centered text for illustration...
Screen('TextSize', exampleImage, 64);
DrawFormattedText(exampleImage, 'Hello World', 'center', 'center', [0 255 0]);

% Ok, exampleImage contains the "distorted" oversized image we want to
% undistort and scale by application of our gloperator. Apply it,
% undistortedImage will contain the new undistorted image:
undistortedImage = Screen('TransformTexture', exampleImage, gloperator, [], undistortedImage);

% Draw undistorted image to onscreen window:
Screen('DrawTexture', w, undistortedImage);

% Show it:
Screen('Flip', w);

% Wait for keypress:
KbStrokeWait;

% Close all windows and ressources:
Screen('CloseAll');

return;
