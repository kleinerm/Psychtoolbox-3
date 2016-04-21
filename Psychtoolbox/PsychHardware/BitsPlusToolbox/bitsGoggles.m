function [encodedDIOdata, Mask, Data, Command] = bitsGoggles(left,right,window)
% [encodedDIOdata] = bitsGoggles(left, right [, window])
%
% Drives the FE1 goggles connected to bits 4 and 5 of the digital output
% of a Bits+, Bits#, etc.
%
% 'left' and 'right' define the required state of the left and
% right goggle shutter, where a value of:
% 0 = goggle open
% 1 = goggle closed
%
% If 'window' is given, but the return argument 'encodedDIOdata' is not
% given, then the control code is drawn into the framebuffer and a flip
% is performed.
%
% If 'encodedDIOdata' is given, then 'window' is not used
% and the required T-Lock code is returned as a image matrix
% which could be used for Screen('PutImage') or Screen('DrawTexture').
%

% History:
% ??/???/????   Written by Cambridge Research Systems.
% 21-Apr-2016   Extended by mk for use in PTB's imaging pipeline:
%               Add 'encodedDIOdata' return argument and code path.
%

    %  Bits     Shutter
    % 5    4     L   R
    % --------------------
    % 1    1     O   O
    % 0    0     O   C
    % 1    0     C   O 
    % 0    1     C   C

    left=logical(left);
    right=logical(right);

    bit5 = ~right;
    bit4 = ~xor(left,right);

    Mask = 16 + 32;
    %Mask = 65535;

    Data=zeros(1,248);
    if (bit4)
        Data=Data + 16;
    end

    if (bit5)
        Data=Data + 32;
    end

    Command = 0;

    if nargout == 0
        % Call routine to draw the T-Lock code and then Screen('Flip'):
        bitsEncodeDIO(Mask,Data,Command, window, 0);
    else
        % Call common encoder routine to create T-Lock image matrix:
        encodedDIOdata = BitsPlusDIO2Matrix(Mask, Data, Command);
    end
return;
