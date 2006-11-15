% Function: bitsEncodeDIO



% Use it when you want to write DIO synchronised with screen frame

% It will sync timer interrupt with frame each frame

% You have to initialise BITS - you could use bitsDviDataInit

% Use: bitsDVI_DIO(Mask,Data,Line)

% Mask is DIO mask

% Mask has to be integer

% Data is na array(1:248)

% Data has to be integer array

% Line is line where data packet is out on the screen

% Line has to be integer



% 18/4/05 ejw Converted it to run with OSX version of Psychtoolbox





function encodedDIOdata = bitsEncodeDIO(Mask, Data, Command, windowPtr)

    % Define screen

    whichScreen=max(Screen('Screens'));



    % find out how big the window is

    [screenWidth, screenHeight]=Screen('WindowSize', windowPtr);



    % check that the screen width is at least 512 pixels wide

    if (screenWidth < 508)

        error('window is not big enough to encode the Bits++ digital output');

    end     

    

    % THE FOLLOWING STEP IS IMPORTANT.

    % make sure the graphics card LUT is set to a linear ramp

    % (else the encoded data will not be recognised by Bits++).

    % There is a bug with the underlying OpenGL function, hence the scaling 0 to 255/256.  

    % This demo will not work using a default gamma table in the graphics card,

    % or even if you set the gamma to 1.0, due to this bug.

    % This is NOT a bug with Psychtoolbox!

    Screen('LoadNormalizedGammaTable',windowPtr,linspace(0,(255/256),256)'*ones(1,3));

    

    % Preparing data array

	%encodedDIOdata = uint8(zeros(max_texture_size, max_texture_size, 3)); 

	encodedDIOdata = uint8(zeros(1, 508, 3));    

    

    % we use the first line of the texture to encode the digital output data



    % Putting the unlock code for DVI Data Packet

    encodedDIOdata(1,1:8,1:3) =  ...

    uint8([69  40  19  119 52  233 41  183;  ...

           33  230 190 84  12  108 201 124;  ...

           56  208 102 207 192 172 80  221])';               

    

    % Length of a packet - it could be changed

    encodedDIOdata(1,9,3) = uint8(249); % length of data packet = number + 1

   

    %Command - data packet

    encodedDIOdata(1,10,3)= uint8(2); % this is a command from the digital output group

    encodedDIOdata(1,10,2) = uint8(Command); % command code

    encodedDIOdata(1,10,1)= uint8(6);  % address 

   

    %DIO output mask

    encodedDIOdata(1,12,3)=uint8(Mask); % LSB DIO Mask data

    encodedDIOdata(1,12,2)=uint8(Mask/256); % MSB DIO Mask data

    encodedDIOdata(1,12,1)=uint8(7);  % address 

   

    % vectorised

    encodedDIOdata(1,14:2:508,3)= uint8(bitand(Data,255));

    encodedDIOdata(1,14:2:508,2)= uint8(bitand(Data,768)/256);

    encodedDIOdata(1,14:2:508,1)= uint8(8:255);



    % display the Tlock packet on the back buffer starting at the bottom left

    rect=[0 0 size(encodedDIOdata,2) 1];

    last_line=[0 screenHeight-1 size(encodedDIOdata,2) screenHeight];

    Screen('PutImage', windowPtr, encodedDIOdata, last_line);

    

    % make the make buffer current during the next blanking period

    Screen(windowPtr,'Flip');