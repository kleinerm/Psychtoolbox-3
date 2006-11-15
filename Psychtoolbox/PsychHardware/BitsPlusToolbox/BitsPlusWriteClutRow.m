function checkRow=BitsPlusWriteClutRow(window, bitsPlusClutRow)
% checkRow=BitsPlusWriteClutRow(window,bitsPlusClutRow)
%
% Write CLUT to first row of image. 

% 10/7/02  dhb Wrote it.
% 11/15/02 dgp Don't waste time. Return checkRow only if requested.

rect=[0 0 size(bitsPlusClutRow,2) 1];
rect=OffsetRect(rect,0,0);
Screen(window,'PutImage',bitsPlusClutRow,rect);
if nargout==1
	checkRow=double(Screen(window,'GetImage',rect));
end
