function glNormalPointer( type, stride, pointer )

% glNormalPointer  Interface to glNormalPointer
%
% usage:  glNormalPointer( type, stride, pointer )
%
% C function:  void glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

% copy data to a persistent variable
%persistent ppointer
%ppointer=pointer;

moglcore( 'glNormalPointer', type, stride, pointer );

return


% ---autocode---
%
% function glNormalPointer( type, stride, pointer )
% 
% % glNormalPointer  Interface to OpenGL function glNormalPointer
% %
% % usage:  glNormalPointer( type, stride, pointer )
% %
% % C function:  void glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=3,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glNormalPointer', type, stride, pointer );
% 
% return
%
