function [newSourceImage, newDestinationImage]=SetDestinationAlpha(destinationFactorStr, alpha, sourceImage, destinationImage)

% [newSourceMat, newDestinationMat]=SetDestinationAlpha(destinationFactorStr, alpha, sourceImage, destinationImage)
%
% Given an alpha blending factor, "destinationFactorStr", insert "alpha"
% into the source or destination image such that on alpha blending with the
% specified destinationFactorStr "alpha" is selected as the destination
% alpha.
%
% For some combinations of destination factor string and source factor
% string, SetDestinationAlpha may undo a subsequent call to SetSourceAlpha
% or be undone by a previous call to SetSourceAlpha.  This happens when
% both the destination factor string and the source factor string specify
% the same location of alpha values yet store different alpha values.  For
% example, if both the source alpha and destination alpha are drawn from
% the source alpha plane (GL_SRC_ALPHA)  then setting the source alpha to 0
% would also change the destination alpha to 0. Both source and destination
% would share the source alpha plane for storage of the alpha value. 
%
% SetAlphaDestination helps test OpenGL alpha blending precision.        
%
% see also: PsychAlphaBlending

% HISTORY
% 
% mm/dd/yy
% 
%  2/17/05  awi wrote it.


%expand alpha to be an alpha plane
destinationMatDims=size(destinationImage);
alphaMatDims=size(alpha) ;
if alphaMatDims(1)*alphaMatDims(2)==1
    alphaPlane=repmat(alpha, destinationMatDims(1), destinationMatDims(2));
else
    alphaPlane=alpha;
end
glOnePlane=repmat(255, destinationMatDims(1), destinationMatDims(2));
glZeroPlane=zeros(destinationMatDims(1), destinationMatDims(2));
    
  
%insert the alpha into the storage location for alpha
if strcmp(destinationFactorStr, 'GL_ZERO')
    if any(any(alpha ~= 0))
        error('destination factor GL_ZERO is incompatible with alpha values not equal to 0.');
    end
    %change nothing
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
elseif strcmp(destinationFactorStr, 'GL_ONE')
    if any(any(alpha ~= 255))
        error('destination factor GL_ONE is incompatible with alpha values not equal to 255.');
    end
    %change nothing
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
elseif strcmp(destinationFactorStr, 'GL_SRC_COLOR')
    %change source image
    newSourceImage=repmat(alphaPlane,[1,1,4]);
    newDestinationImage=destinationImage;
elseif strcmp(destinationFactorStr, 'GL_ONE_MINUS_SRC_COLOR')
    %change source image
    newSourceImage=255-repmat(alphaPlane,[1,1,4]);
    newDestinationImage=destinationImage;
elseif strcmp(destinationFactorStr, 'GL_SRC_ALPHA');
    %change source image
    newSourceImage=sourceImage;
    newSourceImage(:,:,4)=alphaPlane;
    newDestinationImage=destinationImage;
elseif strcmp(destinationFactorStr, 'GL_ONE_MINUS_SRC_ALPHA')
    %change source image
    newSourceImage=sourceImage;
    newSourceImage(:,:,4)=255-alphaPlane;
    newDestinationImage=destinationImage;
elseif strcmp(destinationFactorStr, 'GL_DST_ALPHA');
    %change the destination image
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
    newDestinationImage(:,:,4)=alphaPlane;
elseif strcmp(destinationFactorStr, 'GL_ONE_MINUS_DST_ALPHA');
    %change the destination image
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
    newDestinationImage(:,:,4)=255-alphaPlane;
else
    error('Argument "sourceFactor" is unrecognized or invalid');
end
    

