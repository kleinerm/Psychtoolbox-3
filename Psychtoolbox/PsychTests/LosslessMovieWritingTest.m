function LosslessMovieWritingTest(codec, nrchannels, bpc)
% LosslessMovieWritingTest - Test lossless encoding and decoding of video in movie files.
%
% This test is meant to exercise Screen's movie writing function with
% different GStreamer video codecs and to test if specific codecs are
% capable of encoding video without loss of information into a movie file.
%
% The test creates a test image, encodes/writes it into a movie file, then
% reads it back from the movie file and then compares the original test
% image against the image read back from the movie. If lossless encoding
% worked, both images should be identical.
%
% Optional parameters:
% 'codec' name of video codec to use. Defaults to huffyuv, for use of the
% lossless FFMpeg Huffman encoder. ffenc_ljpeg would be another option for
% a lossless codec.
%
% 'nrchannels' How many color channels to test: 1, 3, or 4 are possible
% values for grayscale, RGB or RGBA encoding. Note that most codecs can't
% really encode the alpha channel and discard it instead, so nrchannels=4
% does not actually verify integrity of the alpha channel.
%
% 'bpc' Bitdepth for color encoding: 8 is the default, which should work.
% 16 is the other allowed option for 16 bpc testing. Most codecs can't
% encode 16 bpc though and will reduce precision to <= 8 bpc.
%
% This test script will write the temporary test file into the current
% working directory and leave it there after the test!
%
% Requires a GPU which can handle at least 4096 x 4096 pixel textures,
% otherwise test failure will occur.
%

% History:
% 01-Dec-2013  mk  Written.

global imgref;
global imgrefcheck;
global imgdec;

% Default to ffenc_huffyuv codec, aka lossless Huffmann YUV encoder:
if nargin < 1 || isempty(codec)
    codec = 'huffyuv';
end

% Define codec type:
codec = [':CodecType=' codec];

% Default to RGB 3-channel testing:
if nargin < 2 || isempty(nrchannels)
    nrchannels = 3;
end
if ~ismember(nrchannels, [1, 3, 4])
    error('Sorry, can only test 1, 3, or 4 channels.');
end

% Default to 8 bpc resolution:
if nargin < 3 || isempty(bpc)
    bpc = 8;
end

if ~ismember(bpc, [8, 16])
    error('Sorry, can only test 8 or 16 bpc.');
end

% Startup check and unified keynames, but old 0-255 color encoding:
PsychDefaultSetup(1);

% Fullscreen onscreen window with black background on screen 0:
win = Screen('Openwindow', 0, 0);

% Use a msize by msize test image with 2^12 * 2^12 pixels to cover the
% whole possible range of 2^24 different color value in a RGB8 image:
msize = 2^12;

% Step 1: Create reference data:
if bpc == 8
    % 8 bpc: Build a huge msize x msize pixels, 3-layer RGB8 uint8 image
    % matrix which contains all possible RGB color pixel values for a RGB8
    % encoding once:
    x = typecast(uint32(0:(msize * msize - 1)), 'uint8');
    y = reshape(x, 4, msize, msize);
    z = shiftdim(y, 1);
    
    % Now that we have a classic 4-layer color image, fill its alpha
    % channel with 255 instead of 0:
    z(:,:,4) = 255;
    
    % Extract wanted number of channels for test image:
    imgref = z(:,:,1:nrchannels);
else
    % How to test this? Todo.
    sca;
    error('16 bpc test not yet implemented.');
end

% Turn into a RGB8 texture:
tex = Screen('MakeTexture', win, imgref);
imgrefcheck = Screen('GetImage', tex, [], [], [], nrchannels);
if isequal(imgref, imgrefcheck)
    % No need to report the expected, so commented out:
    %fprintf('Ref image and reftexture are identical. Good.\n');
else
    warning('Ref image and reftexture are DIFFERENT!!! GPU or graphics driver malfunction?!?\n'); %#ok<WNTAG>
end

Screen('TextSize', win, 96);
Screen('DrawTexture', win, tex);
DrawFormattedText(win, 'REFERENCE IMAGE!\nEncoding...', 'center', 'center', 255);
Screen('Flip', win);
%KbStrokeWait;

moviefile = [pwd filesep 'Testmovie.avi'];

% Step 2: Create a movie file:
movie = Screen('CreateMovie', win, moviefile, msize, msize, 1, codec, nrchannels, bpc);

% Add 3 frames with our test image texture:
for i=1:3
    Screen('AddFrameToMovie', tex);
end

% Finish and close movie:
Screen('FinalizeMovie', movie);

% Step 3: Read movie.
movie = Screen('OpenMovie', win, moviefile, [], [], [], nrchannels);

% This seek is crucially needed if no active playback but passive frame
% fetching is used:
Screen('SetMovieTimeIndex', movie, 0);

movietex = Screen('GetMovieImage', win, movie);
Screen('DrawTexture', win, movietex);
DrawFormattedText(win, 'DECODED MOVIE IMAGE!\nComparing...', 'center', 'center', 255);
Screen('Flip', win);
%KbStrokeWait;

% Retrieve decoded image data:
imgdec = Screen('GetImage', movietex, [], [], [], nrchannels);

% If encoding and decoding was lossless then imgref and imgdec should be
% identical:
if isequal(imgref, imgdec)
    fprintf('\n\nOriginal image and encoded->movie->decoded image are identical! Yay!\n\n');
else
    fprintf('\n\nOriginal image and encoded->movie->decoded image are DIFFERENT!\n\n');
end

% Close movie:
Screen('CloseMovie', movie);

% Close display:
sca;
