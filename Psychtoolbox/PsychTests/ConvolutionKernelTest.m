function [passed difference speedup] = ConvolutionKernelTest(win, nrinchannels, nroutchannels, kernel1, kernel2, imgsize, shadertype, debug)
% [passed difference speedup] = ConvolutionKernelTest(win, nrinchannels, nroutchannels, kernel1, kernel2, imgsize, shadertype, debug)
%
% Test Psychtoolbox imaging pipeline's 2D convolution shaders for
% correctness and accuracy, perform speed benchmark, return fastest setup,
% when using a specific (pair) of kernel(s) and parameters.
%
% This routine builds and tests a set of convolution shaders from the given
% convolution kernel (or pair of kernels for separable dual-pass
% convolution). Each shader is compared against the results of
% Matlabs/Octaves conv2 function, applied to a random noise luminance image
% matrix. The shader is tagged as working correctly if the conv2 result and
% PTB's result do not disagree by more than 1 unit at any location in the
% convolved output images. Accuracy (maximum difference) is reported. All
% correctly working shaders are then benchmarked for speed during a test
% period of 10 seconds and the speedup of the GPU vs. Matlab (CPU) is
% determined and reported. At the end, the best configuration (wrt.
% correctness, accuracy and speed) is reported/recommended for use with the
% given kernel.
%
% The routine takes at least 10 seconds per tested shader, so a full test
% run will take at least 4*10 = 40 seconds, probably a bit more for setup
% and shutdown. Status messages will tell you about progress of the
% operation. You shouldn't use your machine and don't run any other
% applications during benchmarking, otherwise the measured speedup numbers
% may be wrong due to GPU or CPU overload.
%
% Optional parameters and their defaults:
%
% 'win' Window handle of the onscreen window to test on. If none provided,
% will open a suitable one by itself on screen 0.
%
% 'nrinchannels' number of image color channels in test image: Default is 1
% for pure luminance convolution. This script always only tests the first
% channel (red/luminance) for correctness/accuracy, even on multi-channel
% images, but choice of channels will affect overall correctness and speed.
%
% 'nroutchannels' number of image output channels from convolution: By
% default 1 == convolve, replicate result to RGB channels, pass alpha
% through. 3 == Convolve RGB separately, pass through alpha. 4 == Convolve
% RGBA separately.
%
% 'kernel1' == 2D kernel or first 1D kernel in separable mode.
% 'kernel2' == 1D kernel for 2nd pass in separable convolution test.
%
% 'imgsize' == Either size of the random noise test image (default =
% 512x512), or a Matlab image matrix to test on.
%
% 'shadertype' == Vector of mode ids: Tests all modes in the vector. By
% default all shadertypes are tested ie shadertype = [0 1 2 3]. PTB
% provides different implementations of convolution (0,1,2,3) which may
% have different accuracy and performance for a given hardware and kernel.
% The shaders provided in this vector will be tested against each other.
%
% 'debug'Defaults to zero (no output): Amount of debug output to write to
% Matlab window.
%
% THIS SCRIPT IS NOT COMPLETELY FINISHED YET.

% Some Benchmark results for convolution:
%
% MacOS/X 10.4.10 + Matlab 7.1 + PowerPC G5 1.6 Ghz single core vs. NVidia
% GeforceFX-5200 Ultra:
%
% Gaussian kernel, 2D single pass, 1->1 channels, 512 x 512 image:
% Kernel of size 3x3: Speedup 2.5x
%
% Gaussian kernel, 1D separable dual pass, 1->1 channels, 512 x 512 image:
% Kernels of size 9: Speedup 3.5x
%
% -> Kernels greater than 3x3 2D or 9 1D are unsupported on GF5200, can
% cause a system crash!
%
% WindowsXP + Matlab 7.4 + PentiumIV, 3.0 Ghz DualCore (CPU) vs. NVidia
% Geforce7800-GTX:
%
% Case 1: Gaussian kernel, 2D single-pass, 1->1 channels, 512 x 512 image:
% 
% Kernel of size 3x3: Speedup   21.0x
% Kernel of size 5x5: Speedup   12.3x
% Kernel of size 7x7: Speedup   10.5x
% Kernel of size 9x9: Speedup    3.4x
% Kernel of size 11x11: Speedup  6.9x
% Kernel of size 13x13: Speedup  6.8x
% Kernel of size 15x15: Speedup  7.0x
% Kernel of size 17x17: Speedup  6.8x
% Kernel of size 19x19: Speedup  6.9x
%
% 3x3 Sobel, Prewitt, Laplace: Speedup 70x
%
% Case 2: Gaussian kernel, 1D dual-pass, 1->1 channels, 512 x 512 image:
% size   5: 0.65ms vs. 21.7ms: Speedup 33.0x
% size  15: 1.62ms vs. 55.5ms: Speedup 34.3x
% size  33: 3.78ms vs. 115ms:  Speedup 30.6x
% size  65: 7.57ms vs. 242ms:  Speedup 32.0x
% size 129:14.85ms vs. 603ms:  Speedup 40.6x
%
% --> 4-channels vs. 1 channel: CPU x4, GPU const. ---> Speedup times 4!
%
% History:
% 10/13/2007 Written (MK).
% 06/13/2009 Remove Octave special case handling.

global GL;

% Prepare test for this configuration:
% ------------------------------------

% Parse inputs, assign defaults:
if nargin < 2 || isempty(nrinchannels)
    nrinchannels = 1;
end

if nargin < 3 || isempty(nroutchannels)
    nroutchannels = 1;
end

if nargin < 4 || isempty(kernel1)
    error('You must provide at least kernel1 !!');
end

if nargin < 5 || isempty(kernel2)
    kernel2 = [];
end

noiseimg = [];
if nargin < 6 || isempty(imgsize)
    imgsize = 512;
else
    if length(imgsize)>1
        % imgsize is an image matrix:
        noiseimg = imgsize;
        imgsize = length(imgsize);
    end
end

if nargin < 7 || isempty(shadertype)
    shadertype = [0 1 2 3];
end

if nargin < 8 || isempty(debug)
    debug = 0;
end

if nargin < 1 || isempty(win) || win < 10
    % No window provided: Open a suitable one:
    if nargin >=1 && ~isempty(win) && win < 10
        screenid = win;
        win = 0;
    else
        screenid = max(Screen('Screens'));
    end
    win = Screen('OpenWindow', screenid, 0, [], [], [], [], [], kPsychNeedFastBackingStore);
    doclose = 1;
else
    doclose = 0;
end

try

    Screen('TextSize', win, 24);
    Screen('TextColor', win, 255);
    DrawFormattedText(win, 'Preparing test\nThis can take multiple seconds\nPlease standby...', 'center', 'center');
    Screen('Flip', win);

    % Create operator's:
    for i=1:length(shadertype)
        % Store shader type:
        gloperatorid(i) = shadertype(i);

        % Create operator of maximum precision:
        gloperator(i) = CreateGLOperator(win, kPsychNeed32BPCFloat);

        try
            % Separable or non-separable?
            if isempty(kernel2)
                % Non-separable: Full blown single-pass 2D convolution:
                Add2DConvolutionToGLOperator(gloperator(i), kernel1, [], nrinchannels, nroutchannels, debug, shadertype(i));
            else
                % Separable: Two 1D convolutions:
                Add2DSeparableConvolutionToGLOperator(gloperator(i), kernel1, kernel2, [], nrinchannels, nroutchannels, debug, shadertype(i));
            end
        catch
            % Shader creation failed, e.g., GLSL compile/link failure due
            % to shader which would overload hardware ressources. Mark this
            % mode as invalid:
            gloperator(i)=-1;
            le = psychlasterror;
            fprintf('Failed to create shader: %s\n', le.message);
        end
    end
    
    % Test correctness and accuracy:

    % Build test-image: Random noise with mean 128 and stddev. +/- 50:
    if isempty(noiseimg)
        noiseimg=(50*randn(imgsize, imgsize) + 128);
    end

    % Cast to uint8 to make sure both CPU and GPU get the same uint8 input
    % data:
    noiseimg=uint8(noiseimg);

    % Convert it to a texture 'tex':
    tex=Screen('MakeTexture', win, noiseimg);

    % Show it onscreen:
    Screen('DrawTexture', win, tex, [], Screen('Rect', tex));
    Screen('Flip', win);

    % Cast back to single precision for CPU:
    noiseimg=single(noiseimg(:,:,1));
    kernel1= single(kernel1);
    kernel2= single(kernel2);

    kernel = kernel1;
    
    for i=1:length(shadertype)
        if gloperator(i)==-1
            maxdiff(i)=inf;
            fprintf('Shadertype %i: Excluded - Beyond hardware limits.\n', shadertype(i));
            continue;
        end
        
        % On GPU: Apply filter to texture:
        xtex = Screen('TransformTexture', tex, gloperator(i));

        % On CPU: Do the same with conv2 routine of Matlab/Octave:
        if isempty(kernel2)
            % Non-separable convolution with single-precision kernel:
            ref = conv2(noiseimg, kernel1, 'same');
        else
            % Separable dual-pass convolution with single-precision kernels:
            ref = conv2(kernel1, kernel2, noiseimg, 'same');
        end

        % Readback result from GPU with highest precision (ie. float):
        clear gpu;
        gpu = Screen('GetImage', xtex, [], [], 1, 1) * 255;

        % Compute difference matrix:
        difference = gpu(:,:,1) - ref;

        % Only look at inner region, ie. ignore borders the size of the kernel
        % to make sure we don't get confused by boundary artefacts. Compute
        % absolute difference ie. discard sign:
        kernel = kernel1;
        difference = abs(difference(length(kernel):end-length(kernel), length(kernel):end-length(kernel)));

        % Compute maximum difference:
        maxdiff(i) = max(max(difference));

        % Show original and xformed tex onscreen:
        Screen('DrawTexture', win, tex, [], Screen('Rect', tex));
        Screen('DrawTexture', win, xtex, [], AdjoinRect(Screen('Rect', xtex), Screen('Rect', tex), RectRight));
        DrawFormattedText(win, sprintf('Result of mode %i.\nTesting...\n', shadertype(i)), 'center', 'center');
        Screen('Flip', win);

        % Discard old 'xtex':
        Screen('Close', xtex);

        fprintf('Shadertype %i: Maximum difference: %f units. --> ', shadertype(i), maxdiff(i));
        if maxdiff(i) < 1
            fprintf('PASSED!\n');
        else
            fprintf('FAILED!\n');
        end

        % Next shadertype...
    end

    % For those tests that passed, measure performance vs. Matlab/Octave:
    for i=1:length(shadertype)
        if maxdiff(i) < 1
            % Perform 5 seconds of testing on GPU:

            % On GPU: Apply filter to texture once to preheat:
            xtex = Screen('TransformTexture', tex, gloperator(i));

            Screen('DrawTexture', win, tex, [], Screen('Rect', tex));
            Screen('DrawTexture', win, xtex, [], AdjoinRect(Screen('Rect', xtex), Screen('Rect', tex), RectRight),0,0);
            DrawFormattedText(win, sprintf('Benchmarking mode %i.\nCan take more than 10 seconds - Please wait...\n', shadertype(i)), 'center', 'center');
            Screen('Flip', win);

            % Prepare test:
            glFinish;

            count = 0;
            tstart=GetSecs;
            % Do as many xforms as possible in 5 seconds...
            while GetSecs - tstart < 5
                % We recycle the cached xtex target texture as we would do in a
                % real experiment script:
                xtex = Screen('TransformTexture', tex, gloperator(i), [], xtex);
                % Increment counter:
                count = count + 1;
            end

            % Done. Sync the pipeline, take elapsed time:
            glFinish;
            telapsed = GetSecs - tstart;

            avggpu(i) = telapsed / count;

            % Retain last texture for double-checking:
            clear gpu;
            %err = glGetError;
            %fprintf('GL-ERROR1: %i %s\n', err, gluErrorString(err));
            gpu = Screen('GetImage', xtex, [], [], 1, 1) * 255;
            %err = glGetError;
            %fprintf('GL-ERROR2: %i %s\n', err, gluErrorString(err));

            % Compute difference matrix:
            difference = gpu(:,:,1) - ref;

            % Only look at inner region, ie. ignore borders the size of the kernel
            % to make sure we don't get confused by boundary artefacts. Compute
            % absolute difference ie. discard sign:
            kernel = kernel1;
            difference = abs(difference(length(kernel):end-length(kernel), length(kernel):end-length(kernel)));

            % Compute maximum difference:
            maxdiff2(i) = max(max(difference));
            
            if maxdiff2(i) > 1
                fprintf('Shadertype %i FAILED revalidation with error %f!\n', shadertype(i), maxdiff2(i));
            else
                fprintf('Shadertype %i revalidated with error %f!\n', shadertype(i), maxdiff2(i));                
            end
            
            % Close texture:
            Screen('Close', xtex);
            xtex = 0;

            % Now the same game on the CPU:

            % On CPU: Do the same with conv2 routine of Matlab/Octave:
            if isempty(kernel2)
                % Non-separable convolution with single-precision kernel:
                ref = conv2(noiseimg, kernel1, 'same');
            else
                % Separable dual-pass convolution with single-precision kernels:
                ref = conv2(kernel1, kernel2, noiseimg, 'same');
            end

            count = 0;
            tstart=GetSecs;

            % Do as many xforms as possible in 5 seconds...
            while GetSecs - tstart < 5
                % On CPU: Do the same with conv2 routine of Matlab/Octave:
                if isempty(kernel2)
                    % Non-separable convolution with single-precision kernel:
                    ref = conv2(noiseimg, kernel, 'same');
                else
                    % Separable dual-pass convolution with single-precision kernels:
                    ref = conv2(kernel1, kernel2, noiseimg, 'same');
                end

                % Increment counter:
                count = count + 1;
            end

            % Done. Sync the pipeline, take elapsed time:
            telapsed = GetSecs - tstart;

            avgcpu(i) = telapsed / count;


            fprintf('Shadertype %i: Msecs per xform:  GPU = %f   :  CPU = %f  --> ', shadertype(i), 1000 * avggpu(i), 1000 * avgcpu(i));
            fprintf('Speedup is %f times vs. CPU.\n', avgcpu(i) / avggpu(i));
            speedup(i) = avgcpu(i) / avggpu(i);
        else
            % This one failed the test:
            avgcpu(i)=inf;
            avggpu(i)=inf;
            speedup(i)=0;
        end
        % Next shadertype...
    end

    % Done. Destroy operators and textures:
    if doclose
        % Close onscreen window as well:
        Screen('CloseAll');
    else
        % Do not close onscreen window:
        Screen('Close');
    end

    if exist('maxdiff2', 'var')
        for i=1:length(maxdiff2)
            if maxdiff2(i)>1
                speedup(i)=0;
            end
        end

        [maxspeedup bestid] = max(speedup);
        bestid = gloperatorid(bestid);

        fprintf('Finished: Optimum configuration is mode %i with a speedup of %f x.\n', bestid, maxspeedup);
    end
    
    return;
catch
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end
