function glMultiDrawElementsIndirectCountARB( mode, type, indirect, drawcount, maxdrawcount, stride )

% glMultiDrawElementsIndirectCountARB  Interface to OpenGL function glMultiDrawElementsIndirectCountARB
%
% usage:  glMultiDrawElementsIndirectCountARB( mode, type, indirect, drawcount, maxdrawcount, stride )
%
% C function:  void glMultiDrawElementsIndirectCountARB(GLenum mode, GLenum type, GLintptr indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawElementsIndirectCountARB', mode, type, indirect, drawcount, maxdrawcount, stride );

return
