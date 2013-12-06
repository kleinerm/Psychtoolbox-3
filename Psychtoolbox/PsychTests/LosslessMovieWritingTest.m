function LosslessMovieWritingTest(codec, nrchannels, bpc)
% LosslessMovieWritingTest - Test lossless encoding and decoding of video in movie files.
%
% LosslessMovieWritingTest([codec=huffyuv][, nrchannels=3][, bpc=8])
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
%
% 'codec' name of video codec to use. Defaults to huffyuv, for use of the
% lossless FFMpeg Huffman encoder. ffenc_ljpeg would be another option for
% a lossless codec, however ffenc_ljpeg can't be decoded by Psychtoolbox's
% GStreamer movie playback functions. Another lossless codec is
% 'ffenc_sgi', which allows for lossless encoding and relatively high
% compression rates, but its movie files can only be read by Psychtoolbox,
% not by other tools.
%
% 'nrchannels' How many color channels to test: 1, 3, or 4 are possible
% values for grayscale, RGB or RGBA encoding with 8 bpc. For 16 bpc
% encoding, only 1 or 3 channels are possible. Note that most codecs can't
% really encode the alpha channel and discard it instead, so nrchannels=4
% does not actually verify integrity of the alpha channel.
%
% 'bpc' Bitdepth for color encoding: 8 is the default, which should work.
% 16 is the other allowed option for 16 bpc testing. Most codecs can't
% encode 16 bpc though and will reduce precision to <= 8 bpc. Check the
% code of this script to see how a Psychtoolbox proprietary 16 bpc encoding
% can be used to handle lossless 16 bpc, and how this can be decoded. Only
% Psychtoolbox can handle movies in this proprietary encoding, no other 3rd
% party tools! Other constraints for 16 bpc mode: 1 layer grayscale images
% must have a height for which height * 2/3 is an integral value. 3 layer
% RGB images must not be wider than 2048 pixels for use with most codecs.
%
% This test script will write the - potentially huge - temporary test file
% into the current working directory and leave it there after the test!
%
% Requires a GPU which can handle at least 4096 x 4096 pixel textures,
% otherwise test failure will occur. The GPU must also support floating
% point textures for > 8 bpc tests.
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

% A restriction of Psychtoolbox proprietary 16 bpc encoding:
if (bpc > 8) && ~ismember(nrchannels, [1, 3])
    error('Sorry, can only test 1 or 3 channels for bpc > 8.');
end

% Startup check and unified keynames, but old 0-255 color encoding:
PsychDefaultSetup(1);

% Fullscreen onscreen window with black background on screen 0:
win = Screen('Openwindow', 0, 0);

% Step 1: Create reference data:
if bpc == 8
    % 8 bpc: Build a huge msize x msize pixels, 3-layer RGB8 uint8 image
    % matrix which contains all possible RGB color pixel values for a RGB8
    % encoding once:
    
    % Use a msize by msize test image with 2^12 * 2^12 pixels to cover the
    % whole possible range of 2^24 different color value in a RGB8 image:
    msize = 2^12;
    nsize = msize;
    
    x = typecast(uint32(0:(msize * msize - 1)), 'uint8');
    y = reshape(x, 4, msize, msize);
    z = shiftdim(y, 1);
    
    % Now that we have a classic 4-layer color image, fill its alpha
    % channel with 255 instead of 0:
    z(:,:,4) = 255;
    
    % Extract wanted number of channels for test image:
    imgref = z(:,:,1:nrchannels);
    
    % Turn into a L8 / RGB8 / RGBA8 texture:
    tex = Screen('MakeTexture', win, imgref);
    
    % Readback to verify precision of GPU:
    imgrefcheck = Screen('GetImage', tex, [], [], [], nrchannels);
    
    highbitflag = 0;
else
    % 16 bpc: We can not cover the whole color value set. Best we can do is
    % randomly sample, which we'll do:
    
    % Use a msize by msize test image with 2^11 * 1200 pixels. 2^11 is the
    % maximum width most lossless codecs can handle if we use PTB's
    % proprietary 16 bpc encoding, which allows for at most a width half
    % the width of what the codec can do at best:
    msize = 2^11;
    nsize = 1200;
    
    % Draw msize x msize x 3 samples between 0 and 65535, normalize to
    % 0.0 - 1.0 range for floating point image textures. Note: We use
    % rand() instead of randi(), because randi() wasn't available in Octave 3.2,
    % and we want to continue support for it for now.
    imgref = single(floor(rand([nsize, msize, nrchannels]) * (2^16 - 1)) / (2^16 - 1));
    
    % Turn into a Luminance 32 bpc float or RGB 32 float texture:
    tex = Screen('MakeTexture', win, double(imgref), [], [], 2);
    
    % Readback in float precision to verify precision of GPU:
    imgrefcheck = single(Screen('GetImage', tex, [], [], 1, nrchannels));
    
    % Add keyword to request Psychtoolbox's own proprietary 16 bpc encoding:
    codec = [codec ' UsePTB16BPC'];
    
    % Set 'OpenMovie' specialflags1 setting 512 for 16 bpc PTB movie decode:
    highbitflag = 512;
end

if isequal(imgref, imgrefcheck)
    % No need to report the expected, so commented out:
    %fprintf('Ref image and reftexture are identical. Good.\n');
else
    warning('Ref image and reftexture are DIFFERENT!!! GPU or graphics driver malfunction?!?\n'); %#ok<WNTAG>
end

Screen('TextSize', win, 48);
Screen('DrawTexture', win, tex);
DrawFormattedText(win, 'REFERENCE IMAGE!\nEncoding...', 'center', 'center', 255);
Screen('Flip', win);

moviefile = [pwd filesep 'Testmovie.avi'];

% Step 2: Create a movie file:
movie = Screen('CreateMovie', win, moviefile, msize, nsize, 1, codec, nrchannels, bpc);

% Add 3 frames with our test image texture:
for i=1:3
    Screen('AddFrameToMovie', tex);
end

% Finish and close movie:
Screen('FinalizeMovie', movie);

% Step 3: Read movie. Use specialFlags1 = 256 to prevent deinterlacing, as
% that can incur some color-space conversions on 1 channel gray-scale data,
% which would be lossy and thereby defeat the purpose of this test. Optionally
% add a highbitflag of 512 for decoding 16 bpc HDR movies encoded in PTB's
% proprietary movie encoding format:
[movie durationSecs] = Screen('OpenMovie', win, moviefile, [], [], 256 + highbitflag, nrchannels);

% A very huge durationSecs indicates movie duration couldn't get queried.
% This in turn means some non-standard movie container which is not
% seekable. Try to handle this:
if durationSecs < realmax
    % This seek is crucially needed if no active playback but passive frame
    % fetching is used:
    Screen('SetMovieTimeIndex', movie, 0);
else
    % This is not a real seekable movie, but some special container like
    % multipart-mux'ed. Must use active playback to get this working at
    % all:
    Screen('PlayMovie', movie, 1, 0, 0);
end

% Fetch first frame from movie:
movietex = Screen('GetMovieImage', win, movie);
Screen('DrawTexture', win, movietex);
DrawFormattedText(win, 'DECODED MOVIE IMAGE!\nComparing...', 'center', 'center', 255);
Screen('Flip', win);

if bpc <= 8
    % Retrieve decoded image data:
    imgdec = Screen('GetImage', movietex, [], [], 0, nrchannels);
else
    % Retrieve decoded image data:
    imgdec = single(Screen('GetImage', movietex, [], [], 1, nrchannels));
end

% If encoding and decoding was lossless then imgref and imgdec should be
% identical, ie. imgdiff should be all-zero:
if bpc <= 8
    imgdec = int32(imgdec(:));
    imgref = int32(imgref(:));
    imgdiff = imgdec - imgref;
else
    imgdec = imgdec(:);
    imgref = imgref(:);
    imgdiff = imgdec - imgref;
    
    % Convert into 16 bpc encoding:
    imgdiff = int32(round(imgdiff * (2^16 - 1)));
end

% All zero imgdiff, ie., identical?
if ~any(imgdiff)
    fprintf('\n\nOriginal image and encoded->movie->decoded image are identical! Yay!\n\n');
    DrawFormattedText(win, 'IDENTICAL - SUCCESS!', 'center', 'center', [0 255 0]);
else
    fprintf('\n\nOriginal image and encoded->movie->decoded image are DIFFERENT!\n\n');
    DrawFormattedText(win, 'DIFFERENT - FAILURE!', 'center', 'center', [255 0 0]);
    
    mindelta = min(imgdiff) %#ok<NOPRT,NASGU>
    maxdelta = max(imgdiff) %#ok<NOPRT,NASGU>
    rangedelta = range(imgdiff) %#ok<NOPRT,NASGU>
    close all;
    hist(double(imgdiff));
    title('Error distribution in pixel values:');
end

Screen('Flip', win);
KbStrokeWait([], GetSecs + 5);

% Close movie:
Screen('CloseMovie', movie);

% Close display:
sca;
