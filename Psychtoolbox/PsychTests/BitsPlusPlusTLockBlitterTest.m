function BitsPlusPlusTLockBlitterTest(screenId)
% BitsPlusPlusTLockBlitterTest([screenId])
%
% Test if the old style Bits++ T-Lock code generation and the new style
% built-in T-Lock code generation yield identical results.
%
% 'screenId' == Display screen to test. Defaults to max(Screen('Screens')).
%
% This test should pass on all graphics hardware. If it fails on some
% setup, we're interested in hearing from you.
%

% History:
% 03/11/08 Written. (MK)
%

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
    Screen('LoadNormalizedGammaTable',w,newclut, 2);
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
    diff = sum(sum(sum(dimg)));
    dimg2 = abs(double(img2(1,:,:)) - double(clutRow));
    diff2 = sum(sum(sum(dimg2)));
    
    fprintf('\n\nTest results of T-Lock blitter test:\n\n');

    if chk1==0 || chk2==0
        error('Framebuffer readout error or black image!');
    end
    
    if diff2 > 0
        fprintf('Difference between "PutImage" T-Lock and Matlab T-Lock! BAD.\n');
    end
    
    if diff > 0
        fprintf('T-Lock codes not identical! BAD.\n');
        ptbtlock = img1(1, 1:10, :)
        bpptlock = img2(1, 1:10, :)
    else
        fprintf('T-Lock codes are identical! GOOD.\n');
    end

    % Restore Bits++ Identity CLUT so it can be used as normal display:
    Screen('Hookfunction', w, 'Enable', 'LeftFinalizerBlitChain');
    BitsPlusPlus('LoadIdentityClut', w);
    Screen('Flip', w);
    Screen('CloseAll');

    % Clean up: Restore LUT's, close window:
    Screen('LoadNormalizedGammatable', screenId, oldlut);    
catch
    % Clean up: Restore LUT's, close window:
    Screen('LoadNormalizedGammatable', screenId, oldlut);
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end

