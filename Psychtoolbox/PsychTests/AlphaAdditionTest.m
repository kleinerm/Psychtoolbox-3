function failFlag=AlphaAdditionTest(screenNumber)

% failFlag=AlphaAdditionTest([screenNumber])
%
% Combine two alpha layers by addition and verify the result.
%
% If no return argument is provided, then TestAlphaAddition issues an error
% when a test fails.  If a return argument is supplied then it signals a
% failed test only by returning true, without issuing an error.  
%
% See also: AlphaBlendingTest, PsychAlphaBlending


if nargin==0
    screenNumber=max(Screen('Screens'));
end
exitOnError=nargout < 1;
failFlag=0;


background=0;
w=Screen('OpenWindow', screenNumber, [], [], [], 2);
Screen('FillRect', w, background);
wRect=Screen('Rect',w);

testPlane=repmat(0:255, 256, 1);
testImage1=repmat(testPlane, [1, 1, 3]);
testImage2=repmat(rot90(testPlane), [1,1,3]);
testImageRect=RectOfMatrix(testImage1);

%calculate expected results
expectedImage=AlphaSum(testImage1, testImage2);

%write the destination matrix
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage', w, testImage1, testImageRect);
Screen('BlendFunction', w, 'GL_ONE', 'GL_ONE');
Screen('PutImage', w, testImage2, testImageRect);
readbackImage=Screen('GetImage', w, testImageRect, 'backBuffer');
Screen('Flip', w);
WaitSecs(0.5);
if find(readbackImage~=expectedImage)
    if exitOnError
        error('Addition failed');
    else
        fprintf('Addition succeeded\n');
    end
    failFlag=1;
end

%test commutivity by interchangeing source and destination matrices 
Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
Screen('PutImage', w, testImage2, testImageRect);
Screen('BlendFunction', w, 'GL_ONE', 'GL_ONE');
Screen('PutImage', w, testImage1, testImageRect);
readbackImage=Screen('GetImage', w, testImageRect, 'backBuffer');
Screen('Flip', w);
WaitSecs(0.5);
if find(readbackImage~=expectedImage)
    if exitOnError
        error('Addition failed');
    else
        fprintf('Addition succeeded\n');
    end
    failFlag=1;
end

Screen('CloseAll');
