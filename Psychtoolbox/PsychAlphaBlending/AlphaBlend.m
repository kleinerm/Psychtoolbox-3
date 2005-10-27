function newImage=AlphaBlend(sourceFactorStr, destinationFactorStr, sourceImage, destinationImage)

% newImage=AlphaBlend(sourceFactorStr, destinationFactorStr, sourceImage, destinationImage)
%
% Simulate alpha blending.  Calculate an image matrix which matches the
% result of alpha blending in a a window by Screen.     
%
% see also: PsychAlphaBlending

% HISTORY
% 
% mm/dd/yy
% 
%  2/11/05  awi wrote it.


newImage=AlphaSum(  AlphaSourceTerm(sourceFactorStr, sourceImage, destinationImage), ...
                    AlphaDestinationTerm(destinationFactorStr, sourceImage, destinationImage));
                
                    
