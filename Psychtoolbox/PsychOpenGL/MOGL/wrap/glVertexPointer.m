function glVertexPointer( size, type, stride, pointer )

% glVertexPointer  Interface to glVertexPointer
%
% usage:  glVertexPointer( size, type, stride, pointer )
%
% C function:  void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

% copy data to a persistent variable
%persistent ppointer
%ppointer=pointer;

moglcore( 'glVertexPointer', size, type, stride, pointer );

return


% ---autocode---
%
% function glVertexPointer( size, type, stride, pointer )
% 
% % glVertexPointer  Interface to OpenGL function glVertexPointer
% %
% % usage:  glVertexPointer( size, type, stride, pointer )
% %
% % C function:  void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=4,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glVertexPointer', size, type, stride, pointer );
% 
% return
%
