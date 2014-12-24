function SimpleImageMixingDemo(morphType, showMaskOnly)
% SimpleImageMixingDemo([morphType=gaussian][, showMaskOnly=0])
%
% This is a simple demonstration of shader use to morph two images/textures 
% back and forth using a non-uniform transparency pattern (here illustrated 
% with gaussian and left-to-right ramp, but you can use virtually any
% transparency mask).
% 
% INPUTS:
% morphType - string, either 'gaussian' or 'ramp'
% showMaskOnly - 0/1 to either see the mask or the resulting morphing
%
% Made after ImageMixingTutorial.m by Mario Kleiner
% Written by Natalia Zaretskaya 25 Nov 2014

% Basepath to our own demo images:
basepath = [ PsychtoolboxRoot 'PsychDemos' filesep ];
imageFile = [basepath 'konijntjes1024x768blur.jpg'];

% Use normalized color range, ranging from 0 to 1
PsychDefaultSetup(2);

if nargin < 1 || isempty(morphType)
    % decide whether you want a ramp or a gaussian
    morphType = 'gaussian'; % 'ramp' or 'gaussian'
end

if nargin < 2 || isempty(showMaskOnly)
    showMaskOnly = 0;
end

% open window
[w, wrect] = PsychImaging('OpenWindow',  max(Screen('Screens')), [0.5 0.5 0.5], []);
[cx, xy] = RectCenter(wrect);
Screen('TextSize', w, 20)


% Open an offscreen window the same size as the onscreen window. We use
% this to define the alpha/mixing weight channel used to later mix
% two images together:
masktex = Screen('OpenOffscreenWindow', w, [0 0 0 0]);


% -- make textures -- %
mat1 = imread(imageFile); % nice bunnies
mat2 = rand(size(mat1)); % random noise

tex1 = Screen('MakeTexture', w, mat1);
tex2 = Screen('MakeTexture', w, mat2); % upside-down


% Create a shader that allows to combine the up to four input channels
% of a texture into a weighted linear combination, using 'DrawTexture's
% modulateColor parameter to specify the weights. This is used for
% morphing between up to four alpha-masks, stored in the morphedAlphaTexture.
minimorphshader = CreateSinglePassImageProcessingShader(w, 'WeightedColorComponentSum');

% Create a texture with alpha pattern we want to morph between.
if strcmp(morphType, 'ramp')
    morphPattern = meshgrid(0:RectWidth(wrect./2), 0:RectHeight(wrect./2));
    morphPattern = morphPattern./max(morphPattern(:));
    disp('using ramp!')
elseif strcmp(morphType, 'gaussian')
    xsd = 130; % standard deviation
    ysd = 130;
    x = -RectHeight(wrect./2)/2:RectHeight(wrect./2)/2;
    y = -RectHeight(wrect./2)/2:RectHeight(wrect./2)/2;
    [x,y] = meshgrid(x,y);
    morphPattern = exp( -((x / xsd).^2) - ((y / ysd).^2) ); % gauss
    disp('using gaussian!')
else
    sca
    error('Undefined morph pattern');
end
morphTex = Screen('MakeTexture', w, morphPattern, [], [], [], [], minimorphshader);



c = 1;
while 1
    
    % check keyboard:
    keyIsDown = KbCheck;
    if keyIsDown
        break
    end
    
    % for simplicity: sine modulation
    % morph values range from 0 (image A) to 2 (image B)
    % 1 corresponds to intermediate stage
    morphValue = (sin(0.01*c)+1);
    
    % A mask morphing from all-zero to a gauss blob to all-one and back:
    if morphValue < 1
        weights = [morphValue, 0, 0, 0];
    else
        eweight = morphValue - 1;
        weights = [1 - eweight, 0, 0, eweight];
    end
    Screen('DrawTexture', masktex, morphTex, [], CenterRectOnPoint(wrect./1.5, cx, xy), [], [], [], weights);
    
    % First clear framebuffer to backgroundcolor, not using
    % alpha blending (== GL_ONE, GL_ZERO). Enable all channels
    % for writing [1 1 1 1], so everything gets cleared to good
    % starting values:
    Screen('BlendFunction', w, GL_ONE, GL_ZERO, [0 0 0 1]);
    Screen('FillRect', w, [0.5 0.5 0.5]);
    
    % Then keep alpha blending disabled and draw the mask
    % texture, but *only* into the alpha channel. Don't touch
    % the RGB color channels but use the channel mask via
    % [R G B A] = [0 0 0 1] to only enable the alpha-channel
    % for drawing into it. Use of modulateColor = [1 0 0 0] and
    % the minimorphshader causes the red channel to be copied into
    % the alpha channel. As red == luminance this means the grayscale
    % luminance value of masktext directly defines the final mask weights.
    
    if showMaskOnly
        Screen('BlendFunction', w, GL_ONE, GL_ZERO, [1 1 1 1]); % => use [1 1 1 1] without drawing the images to visualize the mask        
        Screen('DrawTexture', w, masktex, [], [], [], [], [], [1 0 0 0], minimorphshader);
    else
        Screen('BlendFunction', w, GL_ONE, GL_ZERO, [0 0 0 1]); % => use [1 1 1 1] without drawing the images to visualize the mask
        Screen('DrawTexture', w, masktex, [], [], [], [], [], [1 0 0 0], minimorphshader);
        
        % draw first image
        Screen('BlendFunction', w, GL_DST_ALPHA, GL_ZERO, [1 1 1 0]);
        Screen('DrawTexture', w, tex1, [], CenterRectOnPoint(wrect./1.5, cx, xy));
        
        % draw second image
        Screen('BlendFunction', w, GL_ONE_MINUS_DST_ALPHA, GL_ONE, [1 1 1 0]);
        Screen('DrawTexture', w, tex2, [], CenterRectOnPoint(wrect./1.5, cx, xy));
    end
    
    % show morphing stage as value:
    myString = sprintf('morph stage 0 to 2: %1.1f ', morphValue);
    DrawFormattedText(w, myString, 0, 0, [1 0 0 ]);
    
    Screen('Flip', w);
    
    c = c+1; % update the count
end

sca
end
