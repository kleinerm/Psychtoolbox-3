% MakeTextureTimingTest
%
% Screen 'MakeTexture' both allocates memory to hold an OpenGL texture and
% loads a MATLAB matrix into the OpenGL texture. TestMakeTextureTiming
% times those two steps separately, reporting the fraction of time which
% MakeTexture spends allocating memory.   
%
% On 1GHz G4, MakeTexture spends less than 3% of its time allocating memory.
% Providing separate Screen subfuntions to allocate and fill textures would
% gain little.
%
% see also: PsychTests

% HISTORY
%
% mm/dd/yy
%
% 1/26/05   awi     wrote it.


try
    %test for a variety of texture sizes up to the screen size. 
    screenNumber=max(Screen('Screens'));
    w=Screen('OpenWindow', screenNumber, [], [], [], 2);
    screenRect=Screen('Rect', screenNumber);
    screenDimensions=[RectWidth(screenRect), RectHeight(screenRect)];
    maxTextureSize=min(screenDimensions);
    numTextures=floor(log2(maxTextureSize));
    textureSizes=2.^([1:numTextures])+1;
    numSamples=10;
    %turn on the debugging switch for 'MakeTexture' to record internal
    %timing.  See the C source to MakeTexture to see at where times are recorded. 

    %The preference call tells MakeTexture to record timestamps marking internal events into the time list. 
    Screen('Preference','DebugMakeTexture', 1);
    for i=1:numTextures
        fprintf(['Step ' int2str(i) ' of ' int2str(numTextures) ': ']);
        fprintf(['Timing generation of a ' int2str(textureSizes(i)) 'x' int2str(textureSizes(i)) ' texture:\n']);
        fprintf(['Repeating measurement ' int2str(numSamples) ' times; counting ']);
        for s=1:numSamples
            fprintf(int2str(s));
            if s<numSamples
                fprintf('.. ');
            else
                fprintf('.');
            end
            Screen('ClearTimeList');
            t=Screen('MakeTexture',w,magic(textureSizes(i)));
            rawTimes=Screen('GetTimeList');  %index by texture, sample, timestamp
            zeroRefTimes=rawTimes-rawTimes(1);
            timeList(i,s,:)=zeroRefTimes;
            %no reason to accumulate textures except to investigate memory loading vs allocation time.
            Screen('Close', t);
        end
        fprintf('\n\n');
    end
    Screen('Preference','DebugMakeTexture', 0);
catch 
    Screen('Preference','DebugMakeTexture', 0);
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end

%Verify that turning off DebugMakeTexture causes values not to accumulate in
%the timing list.
Screen('ClearTimeList');
tFoo=Screen('MakeTexture',w,magic(textureSizes(1)));
if Screen('GetTimeList') > 0
    error('Turning off DebugMakeTexture failed to disable debugging in MakeTexture');
end %if


Screen('CloseAll')


%Average measurements and plot results

%When DebugMakeTexture is enabled, each call to MakeTexture generates four
% timestamps at these events:
% timestamp 1: Upon entering the SCREENMakeTexture C function
% timestamp 2: Immediatly before the valloc call to allocate texture memory
% timestamp 3: Immediatly before the valloc call to allocate texture memory
% timestamp 4: Immediatly before exit from SCREENMakeTexture.

%the indices into our result array are:
%timeList( texture size index,  repetition index, timestamp index)

[foo, bar, numTimestamps]=size(timeList);
if numTimestamps ~= 4
    error('MakeTexture stores more than the expected four timestamps.');
end


%first average across repeated measurements
for tex=1:numTextures
    for ts=1:numTimestamps
        avgTimes(tex, ts)=mean(timeList(tex, :, ts));
    end %for ts
end %for tx

allocationTimes=avgTimes(:,3) - avgTimes(:,2);
meanAllocationTimes=mean(allocationTimes);
copyTimes=avgTimes(:,4) - avgTimes(:,3);
meanCopyTimes=mean(copyTimes);

fprintf('Result:\n');
fprintf(['Average time to allocate texture memory: ' num2str(meanAllocationTimes) ' Secs.\n']);
fprintf(['Average time to copy the MATLAB matrix into the OpenGL texture: ' num2str(meanCopyTimes) ' Secs\n']);
fprintf(['Average proportion of time which MakeTexture spends allocating memory: ' num2str(meanAllocationTimes/meanCopyTimes) '\n']);

