function glIndexPointer( type, stride, pointer )

% glIndexPointer  Interface to glIndexPointer
%
% usage:  glIndexPointer( type, stride, pointer )
%
% C function:  void glIndexPointer(GLenum type, GLsizei stride, const GLvoid* pointer)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

% copy data to a persistent variable
persistent ppointer
ppointer=pointer;

moglcore( 'glIndexPointer', type, stride, ppointer );

return


% ---autocode---
%
% function glIndexPointer( type, stride, pointer )
% 
% % glIndexPointer  Interface to OpenGL function glIndexPointer
% %
% % usage:  glIndexPointer( type, stride, pointer )
% %
% % C function:  void glIndexPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=3,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glIndexPointer', type, stride, pointer );
% 
% return
%
% ---skip---
