function failFlag=AlphaMultiplicationTest(screenNumber)

% passedFlag=AlphaMultipicationTest([screenNumber])
%
% AlphaMultiplicationTest: 
%
%     Test for perfect alpha multiplication precision for alpha values 0 and
%     1. OpenGL guarantees precise alpha muliplicaion for only those alpha
%     values. TestTestAlphaMultipicationAlphaTimes tests that guarantee. 
%
%     Test all alpha modes in all combinations.  This verifies that Screen sets
%     modes properly
%
% Note that Screen alpha values fall in the range 0-255 while OpenGL alpha
% values are normalized between 0-1; OpenGL alpha value 1 is equivalent to
% Screen alpha value 255.
%
% If no return argument is provided, then AlphaMultiplicationTest issues an error
% when a test fails.  If a return argument is supplied then it signals a
% failed test only by returning true, without issuing an error.  
%
% See also: AlphaBlendingTest, PsychAlphaBlending


% HISTORY
%
% mm/dd/yy
%
% 2/17/05   awi  Wrote it.


% TO DO
%
% Roll most tests into a loop, if we want to do that.  Unrolled, the
% comments at the point of failure document the test conditions which
% failed.  



if nargin==0
    screenNumber=max(Screen('Screens'));
end
exitOnError=nargout < 1;
failFlag=0;

background=0;
w=Screen('OpenWindow', screenNumber, [], [], [], 2);
Screen('FillRect', w, background);
wRect=Screen('Rect',w);

zerosImage=zeros(16,16,4);
onesImage=ones(16,16,4);
allValuesImage=repmat(magic(16)-1, [1,1,3]);
allValuesImage(:,:,4)=zerosImage(:,:,1);
imageRect=RectOfMatrix(zerosImage);

sourceBlendFactors={ 'GL_ZERO', 'GL_ONE', 'GL_DST_COLOR', 'GL_ONE_MINUS_DST_COLOR', 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA', 'GL_DST_ALPHA', 'GL_ONE_MINUS_DST_ALPHA', 'GL_SRC_ALPHA_SATURATE'};
destinationBlendFactors={'GL_ZERO','GL_ONE', 'GL_SRC_COLOR', 'GL_ONE_MINUS_SRC_COLOR', 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA', 'GL_DST_ALPHA', 'GL_ONE_MINUS_DST_ALPHA'};
numSourceFactors=length(sourceBlendFactors);
numDestinationFactors=length(destinationBlendFactors);


% TEST ALPHA BLENDING ON THE DESTINATION MATRIX
%  Test alpha multiplication at the destination layer by setting the
%  source layer GL_ZERO and try all possible destination blending factor strings with alpha values 0 and 1.


% DESTINATION: 'GL_ZERO' 
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_ZERO', 0, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
size(readback)
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end

% DESTINATION: 'GL_ONE' 
% GL ALPHA:     1       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_ONE', 255, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ONE');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% DESTINATION: 'GL_SRC_COLOR' 
% GL ALPHA:     0       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_SRC_COLOR', 0, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_SRC_COLOR');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end



% DESTINATION: 'GL_SRC_COLOR' 
% GL ALPHA:     1       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_SRC_COLOR', 255, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_SRC_COLOR');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% DESTINATION: 'GL_ONE_MINUS_SRC_COLOR' 
% GL ALPHA:     0       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_ONE_MINUS_SRC_COLOR', 0, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ONE_MINUS_SRC_COLOR');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% DESTINATION: 'GL_ONE_MINUS_SRC_COLOR' 
% GL ALPHA:     1       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_ONE_MINUS_SRC_COLOR', 255, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ONE_MINUS_SRC_COLOR');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end

% DESTINATION: 'GL_SRC_ALPHA' 
% GL ALPHA:     0       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_SRC_ALPHA', 0, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_SRC_ALPHA');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% DESTINATION: 'GL_SRC_ALPHA'
% GL ALPHA:     1       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_SRC_ALPHA', 255, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_SRC_ALPHA');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% DESTINATION: 'GL_ONE_MINUS_SRC_ALPHA' 
% GL ALPHA:     0       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_ONE_MINUS_SRC_ALPHA', 0, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ONE_MINUS_SRC_ALPHA');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end

% DESTINATION: 'GL_ONE_MINUS_SRC_ALPHA'
% GL ALPHA:     1       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_ONE_MINUS_SRC_ALPHA', 255, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ONE_MINUS_SRC_ALPHA');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% DESTINATION: 'GL_DST_ALPHA' 
% GL ALPHA:     0       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_DST_ALPHA', 0, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_DST_ALPHA');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% DESTINATION: 'GL_DST_ALPHA'
% GL ALPHA:     1       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_DST_ALPHA', 255, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_DST_ALPHA');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end



% DESTINATION: 'GL_ONE_MINUS_DST_ALPHA' 
% GL ALPHA:     0       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_ONE_MINUS_DST_ALPHA', 0, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ONE_MINUS_DST_ALPHA');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% DESTINATION: 'GL_ONE_MINUS_DST_ALPHA'
% GL ALPHA:     1       
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetDestinationAlpha('GL_ONE_MINUS_DST_ALPHA', 255, zerosImage, allValuesImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ONE_MINUS_DST_ALPHA');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% TEST ALPHA BLENDING ON THE SOURCE MATRIX
%  Test alpha multiplication on the source layer by setting the
%  destination layer to GL_ZERO and try all possible source blending factor strings with alpha values 0 and 1.

% SOURCE:       'GL_ZERO' 
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_ZERO', 0, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ZERO', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% SOURCE:       'GL_ONE' 
% GL ALPHA:     1
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_ONE', 255, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% SOURCE:       'GL_DST_COLOR' 
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_DST_COLOR', 0, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_DST_COLOR', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% SOURCE:       'GL_DST_COLOR' 
% GL ALPHA:     1
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_DST_COLOR', 255, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_DST_COLOR', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end



% SOURCE:       'GL_ONE_MINUS_DST_COLOR' 
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_ONE_MINUS_DST_COLOR', 0, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ONE_MINUS_DST_COLOR', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% SOURCE:       'GL_ONE_MINUS_DST_COLOR' 
% GL ALPHA:     1
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_ONE_MINUS_DST_COLOR', 255, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ONE_MINUS_DST_COLOR', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end




% SOURCE:       'GL_SRC_ALPHA' 
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_SRC_ALPHA', 0, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_SRC_ALPHA', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% SOURCE:       'GL_SRC_ALPHA' 
% GL ALPHA:     1
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_SRC_ALPHA', 255, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_SRC_ALPHA', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end




% SOURCE:       'GL_ONE_MINUS_SRC_ALPHA' 
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_ONE_MINUS_SRC_ALPHA', 0, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ONE_MINUS_SRC_ALPHA', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% SOURCE:       'GL_ONE_MINUS_SRC_ALPHA' 
% GL ALPHA:     1
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_ONE_MINUS_SRC_ALPHA', 255, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ONE_MINUS_SRC_ALPHA', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end

% SOURCE:       'GL_DST_ALPHA' 
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_DST_ALPHA', 0, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_DST_ALPHA', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% SOURCE:       'GL_DST_ALPHA' 
% GL ALPHA:     1
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_DST_ALPHA', 255, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_DST_ALPHA', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end



% SOURCE:       'GL_ONE_MINUS_DST_ALPHA'
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_ONE_MINUS_DST_ALPHA', 0, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ONE_MINUS_DST_ALPHA', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


% SOURCE:       'GL_ONE_MINUS_DST_ALPHA' 
% GL ALPHA:     1
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_ONE_MINUS_DST_ALPHA', 255, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_ONE_MINUS_DST_ALPHA', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end



% SOURCE:       'GL_SRC_ALPHA_SATURATE'
% GL ALPHA:     0
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_SRC_ALPHA_SATURATE', 0, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_SRC_ALPHA_SATURATE', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should be all zeros.
passed=all(all(all(readback==zeros(16,16,3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end

% SOURCE:       'GL_SRC_ALPHA_SATURATE' 
% GL ALPHA:     1
% IMAGE:        0-255
[newSourceImage, newDestinationImage]=SetSourceAlpha('GL_SRC_ALPHA_SATURATE', 255, allValuesImage, zerosImage);
% draw the destination image onto the destination
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage',w, newDestinationImage, imageRect);
% alpha blend the source and destination image, using 'PutImage' matrix as
% the source
Screen('BlendFunction', w, 'GL_SRC_ALPHA_SATURATE', 'GL_ZERO');
Screen('PutImage',w, newSourceImage, imageRect);
readback=Screen('GetImage', w, imageRect, 'backBuffer');
Screen('Flip',w);
% Test the readback value, which should match what we wrote out.
passed=all(all(all(readback==allValuesImage(:,:,1:3))));
if ~passed
    if exitOnError
        error('Failed test');
    end
    failFlag=1;
end


Screen('CloseAll');

        
