function bounds=TextCenteredBounds(w,text,yPositionIsBaseline)
% bounds=TextCenteredBounds(window, string [, yPositionIsBaseline=0])
%
% Returns the smallest enclosing rect for the drawn text, relative to the
% current location. This bound is based on the actual pixels drawn, so it
% incorporates effects of text smoothing, etc. All text is drawn on the
% same baseline, horizontally centered by using the x offset
% -Screen(w,'TextWidth',string)/2. "text" may be a cell array or matrix of
% 1 or more strings. The strings are drawn one on top of another, all
% horizontally centered at the current position, before the bounds are
% calculated. This returns the smallest box that will contain all the
% strings. The prior contents of the scratch window are lost. Usually it
% should be an offscreen window, so the user won't see it. If you only
% know your nominal text size and number of characters, you might do this
%
% w=Screen('OpenOffscreenWindow',[],[],[0 0 1.5*textSize*length(string) 2*textSize]);
%
% The suggested window size in that call is generously large because there
% aren't any guarantees from the font makers about how big the text might
% be for a specified point size.
%
% Be warned that TextBounds and TextCenteredBounds are slow (taking many
% seconds) if the window is large. They use the whole window, so if the
% window is 1024x1204 they process a million pixels. The two slowest calls
% are Screen 'GetImage' and FIND. Their processing time is proportional to
% the number of pixels in the window.
%
% Also see TextBounds and Screen 'TextWidth' and 'DrawText'.

% 9/1/98 dgp wrote it.
% 3/19/00 dgp debugged it.
% 11/17/02 dgp Added fix, image1(:,:,1), suggested by Keith Schneider to
%              support 16 and 32 bit images.
% 9/16/04  dgp Suggest a pixelSize of 1.
% 12/16/04 dgp Fixed handling of cell array.
% 12/17/04 dgp Round x0 so bounds will always be integer. Add comment about speed.
% 2/4/05   dgp Support both OSX and OS9.
% 1/6/17   dgp Replaced all code by a call to TextBounds, using a new
%              fourth argument in TextBounds to request text centering.

if nargin<3
    yPositionIsBaseline=0;
end

centerTheText=1;
bounds=TextBounds(w,text,yPositionIsBaseline,centerTheText);
