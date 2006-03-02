function white=WhiteIndex(windowPtrOrScreenNumber)
% color=WhiteIndex(windowPtrOrScreenNumber)
% Returns the CLUT index to produce white at the current screen depth,
% assuming a standard color lookup table for that depth. E.g.
% white=WhiteIndex(w);
% Screen(w,'FillRect',white);
% 
% See BlackIndex.
% 
% When the screen is 1 to 8 bit mode, the Macintosh OS always makes the
% first clut element white and the last black. In 16 or 32 bit mode the
% clut goes from black to white. These CLUT conventions can be overridden
% by Screen 'SetClut', which makes a direct call to the video driver,
% bypassing the Mac OS, allowing you to impose any CLUT whatsoever.

% 3/10/98	dgp Wrote it.
% 3/30/98	dgp Consider only one channel, even for 16 and 32 bit modes.
% 3/8/2000  emw Added Platform Conditionals
% 3/8/2000	dgp Fixed platform conditionals
% 3/30/2004 awi Added OS X case. For now OS X only supports true-color mode, so
%               WhiteIndex behavior on OS X will have to change when we add
%               more depth modes.
% 1/29/05  dgp  Cosmetic.

if nargin~=1
	error('Usage: color=WhiteIndex(windowPtrOrScreenNumber)');
end

if IsOSX
    white=255;
else
    if IsWin
        white8=255;
    else
        white8=0;
    end

    pixelSize=Screen(windowPtrOrScreenNumber,'PixelSize');
    switch pixelSize
    case 32;white=255;
    case 24;white=255;
    case 16;white=31;
    case 8;white=white8;
    case 4;white=0;
    case 2;white=0;
    case 1;white=0;
    end
end
