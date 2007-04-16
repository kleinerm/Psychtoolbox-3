function DriftTexturePrecisionTest(highprecision, verbose)
% DriftTexturePrecisionTest([highprecision=0][, verbose=0])
%
% This test finds the minimum useful subtexel stepsize of the graphics hardwares
% texture coordinate interpolators. The minimum stepsize is the finest
% subpixel stepwidth that the gfx-hardware can resolve into an image in a
% meaningful way, e.g., for slow subpixel scrolling, drifting gratings and
% such.
%
% It draws an alternating black-white pattern, reads it back, then draws
% the pattern shifted by some small horizontal amount, reads it back and
% compares the two drawn images. If they are different then the hardware
% could resolve the subpixel increment into two different, properly shifted
% images via bilinear texture interpolation. If they are the same then the
% increment was too small to resolve for this hardware. The smallest
% stepsize is found in multiple iterations that reduce the stepsize until
% no difference is found anymore.
%
% The minimum resolvable stepsize for an ATI Mobility Radeon X1600 (e.g.,
% MacBook Pro) is 1/64 th of a pixel, which corresponds to 6 bits subpixel
% accuracy.
%
% If the optional flag 'highprecision' is set to 1 or 2, then a special PTB
% GLSL interpolation shader is used instead of the hard-wired interpolator
% of your gfx chip. This is computationally more demanding and therefore
% slower, but it should provide higher precision and better resolution.

% History:
% 15.04.2007 Written (MK).

if nargin < 1 || isempty(highprecision)
    highprecision = 0;
end

if nargin < 2
    verbose = 0
end

AssertOpenGL;
screenNumber=max(Screen('Screens'));
texsize=256;            % Half-Size of the grating image.

try
	% Open a double buffered fullscreen window and draw a gray background 
	% to front and back buffers:
	[w screenRect]=Screen('OpenWindow',screenNumber, 128);
	Screen('Flip', w);

    % Create one single static alternating black-white image:
    grating=zeros(2*texsize, 2*texsize);
    if highprecision
        mfactor = 1;
    else
        mfactor = 255;
    end
    
    for x=1:2*texsize
        for y=1:2*texsize
            % Contrast is 255, so we can resolve stepsizes down to 1/255th.
            grating(y,x)=mod(x,2)*mfactor;
        end
    end
    
    % Store testpattern in texture:
    tex=Screen('MakeTexture', w, grating, [], [], highprecision);
    
    % Definition of the drawn rectangle on the screen:
    dstRect=[0 0 texsize texsize];
    dstRect=CenterRect(dstRect, screenRect);

    % We only sample one single pixel in the center of the display:
    sampleRect = OffsetRect(CenterRect([0 0 1 5], dstRect), 0, 0);
    den = 1;
    skewcount = 0;
    
    % Test for stepsizes down to 1/512 th of a texel.
    while den <= 512
        shiftperframe = 1/den;
        count = 0;
        for i=0:texsize
            % Shift the aperture by "shiftperframe" pixels per frame:
            xoffset = mod(i*shiftperframe,2*texsize);

            % Define shifted srcRect that cuts out the properly shifted rectangular
            % area from the texture:
            srcRect=[xoffset 0 xoffset + texsize texsize];

            % Draw texture:
            Screen('DrawTexture', w, tex, srcRect, dstRect);

            % Read back 1 sample pixel from drawn texture for comparison:
            readimgin = Screen('GetImage', w, sampleRect, 'backBuffer');
            readimg = double(readimgin(1,1,1));
            readimg2 = double(readimgin(5,1,1));

            if (readimg~=readimg2) skewcount = skewcount + 1; end
            
            Screen('Flip', w);
            
            if (i > 0)
                % Any difference in pixels value?
                difference=abs(readimg - oldreadimg);
                if verbose
                    fprintf('Sample was %f --> Delta for offset %f is %f\n', readimg, xoffset, difference);
                end
                
                % Count number of frames with non-differences:
                if difference == 0
                    count = count + 1;
                end
            else
                %KbWait;
                %while KbCheck; end;
            end
            
            % Update:
            oldreadimg = readimg;

            % Abort test if any key is pressed:
            if KbCheck
                break;
            end;
        end;
        
        % Output number of identical pixels in texsize samples sample set.
        % A count of 0 means that the chosen stepsize is still below the
        % resolution of the hardwares texture interpolators:
        fprintf('Stepsize 1 / %i th pixel. Nr. of identical frames: %i', den, count);
        if skewcount > 0
            fprintf('  --  Warning: Interpolator skew detected! Your hardware is sampling inaccurate!\n');
        else
            fprintf('\n');
        end
        
        skewcount = 0;
        
        if count < 2
            % Store this stepsize as our current best:
            bestden = den;
        end
        
        % Half the stepsize for next test run:
        den = den * 2;
    end

    % Output smallest stepsize that the texture coordinate interpolators of
    % the hardware can reliably resolve:
    fprintf('\n\nMinimum useable stepsize for interpolator is 1 / %i th of a pixel.\n', bestden);
        
	Screen('CloseAll');
catch
    %this "catch" section executes in case of an error in the "try" section
    %above.  Importantly, it closes the onscreen window if its open.
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end %try..catch..
