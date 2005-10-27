function newDestinationMat=AlphaDestinationTerm(destinationFactorStr, sourceMat, destinationMat)

% newDestinationMat=AlphaDestinationTerm(destinationFactorStr, sourceMatrix, destinationMatrix)
%
% AlphaDestinationTerm simuluates the step of multplying an alpha factor with the
% destination image in OpenGL alpha blending.    
%
% Apply an OpenGL source factor rule such as 'GL_ZERO' to the image matrix
% 'destinationMat', returing the dot product of destinationMat and the
% alpha factor selected by string 'sourceFactor'.  
%
% The source image matrix 'sourceMat' is required because destination
% factor strings may select alpha values from the source image.
%
% AlphaDestinationTerm calculates with double-precision (64-bit) floating
% point arithmatitic whereas the precision of an OpenGL renderer which it
% simulates is unspecified, except that OpenGL guarantees perfect precision
% for alpha values 0 and 1 (255 via Screen).  Comparison of
% AlphaDestinationTerm with the OpenGL renderer shows that alpha
% multiplicaion discards up to one bit of precision.  
%
% see also: AlphaSourceTerm, PsychAlphaBlending

% HISTORY
% 
% mm/dd/yy
% 
%  2/11/05  awi wrote it.


sourceMatDims=size(sourceMat);
destinationMatDims=size(destinationMat);
if length(sourceMatDims) ~= 3 ||  sourceMatDims(3) ~= 4                     
    error('Argument ''sourceMat'' must be a matrix with dimensions [x,y,4]');
end
if length(destinationMatDims) ~= 3 ||  destinationMatDims(3) ~= 4                     
    error('Argument ''destinationMat'' must be a matrix with dimensions [x,y,4]');
end
sourceMatAlphaPlane=sourceMat(:,:,4);
destinationMatAlphaPlane=destinationMat(:,:,4);
  

if strcmp(destinationFactorStr, 'GL_ZERO')
    newDestinationMat=AlphaTimes(destinationMat,0);
elseif strcmp(destinationFactorStr, 'GL_ONE')
    newDestinationMat=AlphaTimes(destinationMat,255);
elseif strcmp(destinationFactorStr, 'GL_SRC_COLOR')
    newDestinationMat=AlphaTimes(destinationMat, sourceMat);
elseif strcmp(destinationFactorStr, 'GL_ONE_MINUS_SRC_COLOR')
     newDestinationMat=AlphaTimes(destinationMat, 255 - sourceMat);
elseif strcmp(destinationFactorStr, 'GL_SRC_ALPHA');
    newDestinationMat=AlphaTimes(destinationMat, sourceMat(:,:,4));
elseif strcmp(destinationFactorStr, 'GL_ONE_MINUS_SRC_ALPHA')
    newDestinationMat=AlphaTimes(destinationMat, 255 - sourceMat(:,:,4));
elseif strcmp(destinationFactorStr, 'GL_DST_ALPHA');
    newDestinationMat=AlphaTimes(destinationMat, destinationMat(:,:,4));
elseif strcmp(destinationFactorStr, 'GL_ONE_MINUS_DST_ALPHA');
    newDestinationMat=AlphaTimes(destinationMat, 255-destinationMat(:,:,4));
else
    error('Argument "sourceFactor" is unrecognized or invalid');
end
    

