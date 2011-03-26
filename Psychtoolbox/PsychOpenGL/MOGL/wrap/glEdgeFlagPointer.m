function glEdgeFlagPointer( stride, pointer )

% glEdgeFlagPointer  Interface to glEdgeFlagPointer
%
% usage:  glEdgeFlagPointer( stride, pointer )
%
% C function:  void glEdgeFlagPointer(GLsizei stride, const GLvoid* pointer)

% 25-Dec-2005 -- created (moglgen)

% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

% copy data to a persistent variable
persistent ppointer
ppointer=pointer;

moglcore( 'glEdgeFlagPointer', stride, ppointer );

return


% ---autocode---
%
% function glEdgeFlagPointer( stride, pointer )
% 
% % glEdgeFlagPointer  Interface to OpenGL function glEdgeFlagPointer
% %
% % usage:  glEdgeFlagPointer( stride, pointer )
% %
% % C function:  void glEdgeFlagPointer(GLsizei stride, const GLvoid* pointer)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% if nargin~=2,
%     error('invalid number of arguments');
% end
% 
% moglcore( 'glEdgeFlagPointer', stride, pointer );
% 
% return
%
% ---skip---
