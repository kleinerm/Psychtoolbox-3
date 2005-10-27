function [newSourceImage, newDestinationImage]=SetSourceAlpha(sourceFactorStr, alpha, sourceImage, destinationImage)

% [newSourceMat, newDestinationMat]=SetSourceAlpha(SourceFactorStr, alpha, sourceImage, destinationImage)
%
% Given an alpha blending factor, "sourceFactorStr", insert "alpha"
% into the source or destination image such that on alpha blending with the
% specified sourceFactorStr "alpha" is selected as the destination
% alpha.
%
% For some combinations of destination factor string and source factor
% string, SetSourceAlpha may undo a subsequent call to SetDestinationAlpha
% or be undone by a previous call to SetDestinationAlpha.  This happens
% when both the destination factor string and the source factor string
% specify the same location of alpha values yet store different alpha
% values.  For example, if both the source alpha and destination alpha are
% drawn from the source alpha plane (GL_SRC_ALPHA)  then setting the source
% alpha to 0 would also change the destination alpha to 0.  Both source and
% destination surfaces would share the source surface alpha plane for
% storage of the alpha value. 
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
if strcmp(sourceFactorStr, 'GL_ZERO')
    if any(any(alpha ~= 0))
        error('destination factor GL_ZERO is incompatible with alpha values not equal to 0.');
    end
    %change nothing
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
elseif strcmp(sourceFactorStr, 'GL_ONE')
    if any(any(alpha ~= 255))
        error('destination factor GL_ONE is incompatible with alpha values not equal to 255.');
    end
    %change nothing
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
elseif strcmp(sourceFactorStr, 'GL_DST_COLOR')
    newSourceImage=sourceImage;
    newDestinationImage=repmat(alphaPlane,[1,1,4]);
elseif strcmp(sourceFactorStr, 'GL_ONE_MINUS_DST_COLOR')
    newSourceImage=sourceImage;
    newDestinationImage=255-repmat(alphaPlane,[1,1,4]);
elseif strcmp(sourceFactorStr, 'GL_SRC_ALPHA');
    %change source image
    newSourceImage=sourceImage;
    newSourceImage(:,:,4)=alphaPlane;
    newDestinationImage=destinationImage;
elseif strcmp(sourceFactorStr, 'GL_ONE_MINUS_SRC_ALPHA')
    %change source image
    newSourceImage=sourceImage;
    newSourceImage(:,:,4)=255-alphaPlane;
    newDestinationImage=destinationImage;
elseif strcmp(sourceFactorStr, 'GL_DST_ALPHA');
    %change the destination image
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
    newDestinationImage(:,:,4)=alphaPlane;
elseif strcmp(sourceFactorStr, 'GL_ONE_MINUS_DST_ALPHA');
    %change the destination image
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
    newDestinationImage(:,:,4)=255-alphaPlane;
elseif strcmp(sourceFactorStr, 'GL_SRC_ALPHA_SATURATE');
    %change the source image
    %change the destination image
    newSourceImage=sourceImage;
    newDestinationImage=destinationImage;
    newSourceImage(:,:,4)=alphaPlane;
    newDestinationImage(:,:,4)=255-alphaPlane;
else
    error('Argument "sourceFactorStr" is unrecognized or invalid');
end
    





