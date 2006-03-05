function glColorPointer( size, type, stride, pointer )

% glColorPointer  Interface to glColorPointer
%
% usage:  glColorPointer( size, type, stride, pointer )
%
% C function:  void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

% copy data to a persistent variable
persistent ppointer
ppointer=pointer;

moglcore( 'glColorPointer', size, type, stride, ppointer );

return


% ---autocode---
%
% function glColorPointer( size, type, stride, pointer )
% 
% % glColorPointer  Interface to OpenGL function glColorPointer
% %
% % usage:  glColorPointer( size, type, stride, pointer )
% %
% % C function:  void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=4,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glColorPointer', size, type, stride, pointer );
% 
% return
%
