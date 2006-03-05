function glVertexAttribPointer( index, size, type, normalized, stride, pointer )

% glVertexAttribPointer  Interface to glVertexAttribPointer
%
% usage:  glVertexAttribPointer( index, size, type, normalized, stride, pointer )
%
% C function:  void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=6,
    error('invalid number of arguments');
end

% copy data to a persistent variable
persistent ppointer
ppointer=pointer;

moglcore( 'glVertexAttribPointer', index, size, type, normalized, stride, ppointer );

return


% ---autocode---
%
% function glVertexAttribPointer( index, size, type, normalized, stride, pointer )
% 
% % glVertexAttribPointer  Interface to OpenGL function glVertexAttribPointer
% %
% % usage:  glVertexAttribPointer( index, size, type, normalized, stride, pointer )
% %
% % C function:  void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=6,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glVertexAttribPointer', index, size, type, normalized, stride, pointer );
% 
% return
%
