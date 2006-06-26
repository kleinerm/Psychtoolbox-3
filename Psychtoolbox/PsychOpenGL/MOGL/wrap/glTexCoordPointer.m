function glTexCoordPointer( size, type, stride, pointer )

% glTextCoordPointer  Interface to glTexCoordPointer
%
% usage:  glTextCoordPointer( size, type, stride, pointer )
%
% C function:  void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

% copy data to a persistent variable
%persistent ppointer
%ppointer=pointer;

moglcore( 'glTexCoordPointer', size, type, stride, pointer );

return


% ---autocode---
%
% function glTexCoordPointer( size, type, stride, pointer )
% 
% % glTexCoordPointer  Interface to OpenGL function glTexCoordPointer
% %
% % usage:  glTexCoordPointer( size, type, stride, pointer )
% %
% % C function:  void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=4,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glTexCoordPointer', size, type, stride, pointer );
% 
% return
%
