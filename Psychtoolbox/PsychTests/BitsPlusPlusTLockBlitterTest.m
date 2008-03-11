function BitsPlusPlusTLockBlitterTest(screenId)

if nargin < 1
    screenId = max(Screen('Screens'));
end

oldlut = Screen('ReadNormalizedGammatable', screenId);

try
    % Enable built-in Bits++ support:
    w = BitsPlusPlus('OpenWindowBits++', screenId, 0);

    % Create all zero CLUT for Bits++
    newclut = 0 * ones(256,3);

    % Load it via PTB support:
    Screen('LoadNormalizedGammaTable',w,newclut,2);
    Screen('Flip',w);
    
    % Retrieve generated T-Lock framebuffer image:
    img1 = Screen('GetImage', w, [0 0 524 2]);
    chk1 = sum(sum(sum(double(img1))));
    
    % Do T-Lock again manually, disable PTB's built-in support:
    Screen('Hookfunction', w, 'Disable', 'LeftFinalizerBlitChain');

    clutRow = BitsPlusEncodeClutRow(floor(newclut * (2^16 - 1) + 0.5));
    Screen('PutImage',w,clutRow,[0 0 524 1]);
    Screen('Flip',w);
    
    % Retrieve generated T-Lock framebuffer image:
    img2 = Screen('GetImage', w, [0 0 524 2]);
    chk2 = sum(sum(sum(double(img2))));

    % Compute differences between both T-Lock images. They should be
    % identical if the GPU is rasterizing properly:
    dimg = abs(double(img1) - double(img2));
    diff = sum(sum(sum(dimg)))
    dimg2 = abs(double(img2(1,:,:)) - double(clutRow));
    diff2 = sum(sum(sum(dimg2)))
    
    if chk1==0 || chk2==0
        error('Framebuffer readout error or black image!');
    end
    
    if diff2 > 0
        fprintf('Difference between "PutImage" T-Lock and Matlab T-Lock! BAD.\n');
    end
    
    if diff > 0
        fprintf('T-Lock codes not identical! BAD.\n');
    else
        fprintf('T-Lock codes are identical! GOOD.\n');
    end

    % Clean up: Restore LUT's, close window:
    Screen('LoadNormalizedGammatable', screenId, oldlut);
    Screen('CloseAll');
    
catch
    % Clean up: Restore LUT's, close window:
    Screen('LoadNormalizedGammatable', screenId, oldlut);
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end

