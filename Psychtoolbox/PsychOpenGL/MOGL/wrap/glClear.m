function glClear( mask )

% glClear  Interface to glClear
% 
% usage:  glClear( mask )
% 
% - 'mask' defaults to GL.COLOR_BUFFER_BIT | GL.DEPTH_BUFFER_BIT
%
% C function:  void glClear(GLbitfield mask)

% 21-Dec-2005 -- created (moglgen)

% ---protected---

if nargin==0,
    global GL
    mask=mor(GL.COLOR_BUFFER_BIT,GL.DEPTH_BUFFER_BIT);
elseif nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glClear', mask );

return


% ---autocode---
%
% function glClear( mask )
% 
% % glClear  Interface to OpenGL function glClear
% %
% % usage:  glClear( mask )
% %
% % C function:  void glClear(GLbitfield mask)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=1,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glClear', mask );
% 
% return
%
% ---skip---
