function glFogCoordPointer( arg1, arg2, arg3 )

% glFogCoordPointer  Interface to glFogCoordPointer
%
% usage:  glFogCoordPointer( arg1, arg2, arg3 )
%
% C function:  void glFogCoordPointer(GLenum, GLsizei, const GLvoid*)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

% copy data to a persistent variable
persistent ppointer
ppointer=arg3;

moglcore( 'glFogCoordPointer', arg1, arg2, ppointer );

return


% ---autocode---
%
% function glFogCoordPointer( type, stride, pointer )
% 
% % glFogCoordPointer  Interface to OpenGL function glFogCoordPointer
% %
% % usage:  glFogCoordPointer( type, stride, pointer )
% %
% % C function:  void glFogCoordPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=3,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glFogCoordPointer', type, stride, pointer );
% 
% return
%
% ---skip---
