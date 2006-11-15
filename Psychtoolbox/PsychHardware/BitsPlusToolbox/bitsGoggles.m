% usage bitsGoggles(left,right,window)
%
% where 0 = goggle open
% and 1 = goggle closed
%
% drives the FE1 goggles connected to bits 4 and 5 of the digital output.

function bitsGoggles(left,right,window)

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

    bitsEncodeDIO(Mask,Data,Command, window);
