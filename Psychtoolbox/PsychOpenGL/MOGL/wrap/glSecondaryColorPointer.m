function glSecondaryColorPointer( arg1, arg2, arg3, arg4 )

% glSecondaryColorPointer  Interface to glSecondaryColorPointer
%
% usage:  glSecondaryColorPointer( arg1, arg2, arg3, arg4 )
%
% C function:  void glSecondaryColorPointer(GLint, GLenum, GLsizei, const GLvoid*)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

% copy data to a persistent variable
persistent ppointer
ppointer=arg4;

moglcore( 'glSecondaryColorPointer', arg1, arg2, arg3, ppointer );

return


% ---autocode---
%
% function glSecondaryColorPointer( size, type, stride, pointer )
% 
% % glSecondaryColorPointer  Interface to OpenGL function glSecondaryColorPointer
% %
% % usage:  glSecondaryColorPointer( size, type, stride, pointer )
% %
% % C function:  void glSecondaryColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=4,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glSecondaryColorPointer', size, type, stride, pointer );
% 
% return
%
% ---skip---
