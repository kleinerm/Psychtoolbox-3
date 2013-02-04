  function GPUFFTDemo1

    AssertOpenGL;

    Screen('Preference','SkipSyncTests', 2);

    GPUstart;

    try

    PsychImaging('PrepareConfiguration');
    %PsychImaging('AddTask', 'General', 'FloatingPoint32Bit');
    w = PsychImaging('OpenWindow', 0, 0);

    % Read our beloved bunny image from filesystem:
    bunnyimg = imread([PsychtoolboxRoot 'PsychDemos/konijntjes1024x768.jpg']);
    bunnyimg(:,:,4) = 255;
  
    dbunny = double(bunnyimg(1:512,1:512,2))/255;
    
    ft = fft2(dbunny);
    figure; imshow(abs(ft).^0.3, []);


    % Maketexture in float precision with upright texture orientation:
    bunnytex = Screen('MakeTexture', w, dbunny, [], [], 2, 1);

    % Show it:
    Screen('DrawTexture', w, bunnytex);
    Screen('TextSize', w, 28);
    DrawFormattedText(w, 'Original pre-FFT Bunny:', 'center', 40, [255 255 0]);
    Screen('Flip', w);
    KbStrokeWait(-1);

    %% CUFFT example
    A = GPUTypeFromToGL(0, bunnytex, [], [], 0);
    A = A(1, :, :);

    if 1
        GPUrand(A);
        
        % Perform forward 2D-FFT:
        F = fft2(A.*512);
        
        P = abs(F) .^ 0.1;
        
        pclass = class(P)
        preal = isreal(P)
        
        % Perform inverse 2D-FFT
        B = ifft2(F);
        
        % Convert B into texture:
        bsize = size(B)
        bclass = class(B)
        breal = isreal(B)
        
        R = real(B);
        rreal = isreal(R)
        rclass = class(R)
        rsize = size(R)
        
        Rh = single(P);
        vmax = max(max(Rh))
        vmin = min(min(Rh))
        
        P = 1/vmax .* P;
        
        
        
        t = GPUTypeFromToGL(1, R, [], [], 0);
        
        Screen('DrawTexture', w, t);
        DrawFormattedText(w, 'Processed post-FFT Bunny:', 'center', 40, [0 255 0]);
        Screen('Flip', w);
        KbStrokeWait(-1);
        
    else

        fftType = cufftType;
        fftDir  = cufftTransformDirections;

        % FFT plan
        plan = 0;
        [status, plan] = cufftPlan2d(plan, size(d_A, 1), size(d_A, 2), fftType.CUFFT_R2C);
        cufftCheckStatus(status, 'Error in cufftPlan2D');

        % Run GPU FFT
        [status] = cufftExecR2C(plan, getPtr(d_A), getPtr(d_B), fftDir.CUFFT_FORWARD);
        cufftCheckStatus(status, 'Error in cufftExecR2C');

        % Run GPU IFFT
        [status] = cufftExecC2R(plan, getPtr(d_B), getPtr(d_A), fftDir.CUFFT_INVERSE);
        cufftCheckStatus(status, 'Error in cufftExecC2C');

        % results should be scaled by 1/N if compared to CPU
        % h_B = 1/N*single(d_A);

        [status] = cufftDestroy(plan);
        cufftCheckStatus(status, 'Error in cuffDestroyPlan');
    end

    % Reset interop cache:
    GPUTypeFromToGL(4);

    % Close window.
    sca;

  catch
    % Reset interop cache:
    GPUTypeFromToGL(4);

    % Close window.
    sca;
    psychrethrow(psychlasterror);
  end

