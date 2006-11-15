% BitsPlusColorSimpleTest
% 
% Lets see if we can tame Color++ mode.
%
% 8/8/04	dhb		Started it.

% Generate pixel values
highPixel = bitset(255,1,0);
highPixel = bitset(highPixel,2,0);
lowPixel = 0;
theImage1 = uint8(repmat([highPixel lowPixel],200,100));

% Generate pixel values
highPixel = bitset(255,1,0);
highPixel = bitset(highPixel,2,0);
lowPixel = 255;
theImage2 = uint8(repmat([highPixel lowPixel],200,100));

% Generate pixel values
highPixel = 255;
lowPixel = 0;
theImage3 = uint8(repmat([highPixel lowPixel],200,100));

% Put together
theImage = [theImage1 theImage2];

% Show the image
[m,n] = size(theImage);
imageRect = [0 0 m n];
fprintf('Showing image\n');
[window,rect] = Screen(1,'OpenWindow',0,[],32);
identityClut = (0:255)'*ones(1,3);
Screen(window,'SetClut',identityClut);

Screen(window,'PutImage',theImage,[0 0 n m]);
fprintf('Hit any character to continue\n');
GetChar;
Screen(window,'Close'); 	 

	
