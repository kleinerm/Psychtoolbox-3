function [maximumError, roundTypeStr, independentFlag]=AlphaMultiplicationAccuracyTest(screenNumber)

% [maximumError, roundTypeStr, independentFlag]=AlphaMultiplicationAccuracyTest([screenNumber])
%
% Test the accuracy of alpha blending multiplication. OpenGL guarantees
% perfect accuracy of alpha multiplication for values 0 and 1 only.
% AlphaMultiplicationAccuracyTest measures accuracy of intermediate values.
%
% Return argument "maximumError" is the maximum unsigned difference between
% OpenGL alpha multiplication and  simulated alpha blending in MATLAB using
% double-precisions floating point multiplication.
%
% Values of maximumError fall into three categories:
%
% 0 <= maximumError < 0.5   : OpenGL rounds to nearest integer.  No 
%                             accuracy loss for a single multiplication.
%
% 0.5 <= maximumError < 1   : OpenGL truncates or rounds up. For a single 
%                             multiplication, Accuracy is off  0.5 parts
%                             in 255 more than would multiplying luminances 
%                             using floating-point values and rounding.
%
% maximumError >= 1         : Something is wrong.  
% 
% 
% AlphaMultiplicationAccuracyTest tries to determine whether OpenGL alpha
% multiplication rounds to the nearest integer, rounds down or rounds up
% and returns in "roundTypeStr" a string indicating which, either, "round"
% "floor", or "ceil".  If AlphaMultiplicationAccuracyTest can not determine
% the rounding method, then it returns "unknown".
%
% AlphaMultiplicationAccuracyTest also tests that multiplication errors are
% independent of the choice of blending factor string and the blending
% surface, setting return argument "independentFlag" accordingly.     
%
% Because in OpenGL pixel color components are ultimately encoded as 8-bit
% integers in video RAM, the results of OpenGL alpha multiplicaion will be
% less accurate than those predicted by floating-point calculations.  If
% OpenGL rounds to the nearest integer then the alpha multiply error will
% be less than 0.5. This is the limit of precision of the color components
% of a pixel. Therefore, when alpha blending rounds to the nearest integer,
% no more accuracy is lost with OpenGL alpha blending than by calculating
% pixel values in MATLAB with floating point precision then rounding them
% to integer pixel values for display.
%
% Note that errors are cumulative and iterative alpha multiplication, in
% which a product of a prevoius multiplication becomes a factor in a
% subsequent multiplication, can produce large errors, even in the best
% case of rounding where 0 <= maximumError < 0.5.  Note also that A single
% alpha blending operation may result in two multipliations, because both
% source and destination surfaces may be multiplied before they are added. 
%    
% We test blending accuracy becasue OpenGL makes no gurantees.  The OpenGL
% policy on alpha multiplician is summarized here:  
% 
% http://msdn.microsoft.com/library/default.asp?url=/library/en-us/opengl/glfunc01_4vs3.asp
%
%         "Despite the apparent precision of the above equations, blending
%         arithmetic is not exactly specified, because blending operates with
%         imprecise integer color values. However, a blend factor that should be
%         equal to one is guaranteed not to modify its multiplicand, and a blend
%         factor equal to zero reduces its multiplicand to zero." 
%
% See also: AlphaBlendingTest, PsychAlphaBlending


% HISTORY
%
% mm/dd/yy
%
% 1/28/05   awi  Wrote it.


% BUGS
%
% There is possibility that an OpenGL implementation would round
% multiplication to the nearest integer but have a different policy for
% which direction to round 0.5 than does MATLAB.  In that case,
% AlphaMultiplicationAccuracyTest would return a maximumError of 0.5 and
% roundTypeStr 'unknown' instead of 'round'.  


exitOnError= nargout < 1;
if nargin==0
    screenNumber=max(Screen('Screens'));
end



%first test GetImage using PutImage because in later steps we rely on
%GetImage alpha blending.
background=127;
w=Screen('OpenWindow', screenNumber, [], [], [], 2);
Screen('FillRect', w, background);
wRect=Screen('Rect',w);


testPlane1=repmat(0:255, 256, 1);
testPlane2=rot90(testPlane1);
testImage1=repmat(testPlane1, [1, 1, 3]);
testImage2=repmat(testPlane2, [1,1,3]);
testImageRect=RectOfMatrix(testImage1);
imageMat1=testImage1;
imageMat1(:,:,4)=zeros(256);
zerosImage=repmat(zeros(256), [1,1,4]);
matRect=RectOfMatrix(testImage1);


sourceBlendFactors={ 'GL_ZERO', 'GL_ONE', 'GL_DST_COLOR', 'GL_ONE_MINUS_DST_COLOR', 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA', 'GL_DST_ALPHA', 'GL_ONE_MINUS_DST_ALPHA', 'GL_SRC_ALPHA_SATURATE'};
destinationBlendFactors={'GL_ZERO','GL_ONE', 'GL_SRC_COLOR', 'GL_ONE_MINUS_SRC_COLOR', 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA', 'GL_DST_ALPHA', 'GL_ONE_MINUS_DST_ALPHA'};
numSourceFactors=length(sourceBlendFactors);
numDestinationFactors=length(destinationBlendFactors);

sourceBlendVariableFactors={ 'GL_DST_COLOR', 'GL_ONE_MINUS_DST_COLOR', 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA', 'GL_DST_ALPHA', 'GL_ONE_MINUS_DST_ALPHA', 'GL_SRC_ALPHA_SATURATE'};
destinationBlendVariableFactors={'GL_SRC_COLOR', 'GL_ONE_MINUS_SRC_COLOR', 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA', 'GL_DST_ALPHA', 'GL_ONE_MINUS_DST_ALPHA'};
numSourceBlendVariableFactors=length(sourceBlendVariableFactors);
numDestinationBlendVariableFactors=length(destinationBlendVariableFactors);



% first, check that alpha multiplication gives the same result regardless of
% the mode. We do not test trivial cases of GL_ZERO and GL_ONE.  Those are tested
% elsewhere, by AlphaMultiplicationTest.  

%for source blending
for i=1:numSourceBlendVariableFactors
    %make source and destination images
    [sourceImagesA{i}, destinationImagesA{i}]=SetSourceAlpha(sourceBlendVariableFactors{i}, testPlane2, imageMat1, zerosImage);
    %write the destination matrix
    Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
    Screen('PutImage', w, destinationImagesA{i}, matRect);
    %write the source matrix.
    Screen('BlendFunction', w, sourceBlendVariableFactors{i}, 'GL_ZERO');
    Screen('PutImage', w, sourceImagesA{i}, matRect);
    sourceReadbacks{i}=Screen('GetImage',w,matRect);
    Screen('Flip', w);
end


%compare results from every mode to results from every other.  
sourceEqualityMat=zeros(numSourceBlendVariableFactors);
for i=1:numSourceBlendVariableFactors
    for j=1:numSourceBlendVariableFactors
        sourceEqualityMat(i,j)=isempty(find(sourceReadbacks{i}~=sourceReadbacks{j}));
    end
end


%for destination blending
for i=1:numDestinationBlendVariableFactors
    %make source and destination images
    [sourceImagesB{i}, destinationImagesB{i}]=SetDestinationAlpha(destinationBlendVariableFactors{i}, testPlane2, zerosImage, imageMat1);
    %write the destination matrix
    Screen('BlendFunction', w, 'GL_ONE', 'GL_ZERO');
    Screen('PutImage', w, destinationImagesB{i}, matRect);
    %write the source matrix.
    Screen('BlendFunction', w,  'GL_ZERO', destinationBlendVariableFactors{i});
    Screen('PutImage', w, sourceImagesB{i}, matRect);
    destinationReadbacks{i}=Screen('GetImage',w,matRect);
    Screen('Flip', w);
end

%on the destination surface, for each mode compare the product to the products for
%every other mode and for each comparison store a bit into a table indicating a match.  
destinationEqualityMat=zeros(numDestinationBlendVariableFactors);
for i=1:numDestinationBlendVariableFactors
    for j=1:numDestinationBlendVariableFactors
        destinationEqualityMat(i,j)=isempty(find(destinationReadbacks{i}~=destinationReadbacks{j}));
    end
end

%on the source surface, for each mode compare the product to the products for
%every other mode and for each comparison store a bit into a table indicating a match.  
sourceEqualityMat=zeros(numSourceBlendVariableFactors);
for i=1:numSourceBlendVariableFactors
    for j=1:numSourceBlendVariableFactors
        sourceEqualityMat(i,j)=isempty(find(sourceReadbacks{i}~=sourceReadbacks{j}));
    end
end


%As above, but combine both source and destination results into a larger
%table indicating whether source surface product matches destination surface product.
allReadbacks={sourceReadbacks{:} destinationReadbacks{:}};
equalityMat=zeros(length(allReadbacks));
for i=1:length(allReadbacks)
    for j=1:length(allReadbacks)
        equalityMat(i,j)=isempty(find(allReadbacks{i}~=allReadbacks{j}));
    end
end

%Test whether multliplication errors on the source surface are independent
%of the blending factor string
sourceIndependentFlag=isempty(find(sourceEqualityMat ~= 1));

%Test whether multliplication errors on the destination surface are independent
%of blending factor string
destinationIndependentFlag=isempty(find(destinationEqualityMat ~= 1));

%Test whether multliplication errors are independent of both the surface and the blending factor string 
independentFlag=isempty(find(equalityMat ~= 1));


% whatever the multiplication accuracy, we  now know from previous tests in
% this file if accuracy is the same for all modes.  It should be,
% therefore, in the next step when testing that accuracy, we need only test
% one mode and know that those results apply to all others.
predictedReadbackAll=AlphaSourceTerm(sourceBlendVariableFactors{1}, sourceImagesA{1}, destinationImagesA{1});
predictedReadback=predictedReadbackAll(:,:,1:3);
maxFloatDiff=max(max(max(abs(predictedReadback-double(sourceReadbacks{1})))));
maxRoundDiff=max(max(max(abs(round(predictedReadback)-double(sourceReadbacks{1})))));
maxFloorDiff=max(max(max(abs(floor(predictedReadback)-double(sourceReadbacks{1})))));
maxCeilDiff=max(max(max(abs(ceil(predictedReadback)-double(sourceReadbacks{1})))));

maximumError=maxFloatDiff;
if maxRoundDiff==0
    roundTypeStr='round';
elseif maxFloorDiff==0
    roundTypeStr='floor';
elseif maxCeilDiff==0
    roundTypeStr='ceil';
else
    roundTypeStr='unknown';
end


Screen('CloseAll');


