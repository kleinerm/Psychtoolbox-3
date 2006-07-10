function glmMovie( imlist, poslist, vbllist, varargin )

% glmMovie  Play a movie
%
% usage:  glmMovie( imlist, poslist, vbllist, packimage1, ... )

% 01-Jan-2006 -- created (RFM)

% ---protected---

if nargin<4,
	error('invalid number of arguments');
end
if nargin<max(imlist+3),
	error('image indices in ''imlist'' exceed number of images');
end

% get constants
global GL
xy=glmGetScreenInfo;

% clear screen
glClear;

% set read and draw buffers
glReadBuffer(GL.FRONT);
glDrawBuffer(GL.BACK);

% step through images
for i=1:numel(imlist),

	% draw image
	glWindowPos2dv(poslist(:,i));
	glmPutPixels(varargin{imlist(i)});

	% flush and pause
	if vbllist(i)>0,
		glmSwapBuffers;
		if vbllist(i)>1,
			% copy front buffer to back buffer
			glWindowPos2dv([ 0 0 ]);
			glCopyPixels(0,0,xy(1),xy(2),GL.COLOR);
			% flush buffer
			for j=2:vbllist(i),
				glmSwapBuffers;
			end
		end
		% prepare for next frame
		glClear;
	end

end

return
