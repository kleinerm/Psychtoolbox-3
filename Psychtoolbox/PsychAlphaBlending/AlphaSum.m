function sumImage=AlphaSum(imageSource, imageDestination)

% sumImage=AlphaSum(image1, image2)
%
% Return the sum of two images combined with alpha blending as would
% Screen.  
%
% see also: AlphaSourceProduct, AlphaDestinationProduct, PsychAlphaBlending

% HISTORY
% 
% mm/dd/yy
% 
%  2/11/05  awi wrote it.


sumImage=imageSource + imageDestination;
sumImageOver=sumImage > 255;
sumImage(sumImageOver)=255;

%sumImage=round(sumImage);


