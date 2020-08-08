function glMultiDrawElementsIndirectCount( mode, type, indirect, drawcount, maxdrawcount, stride )

% glMultiDrawElementsIndirectCount  Interface to OpenGL function glMultiDrawElementsIndirectCount
%
% usage:  glMultiDrawElementsIndirectCount( mode, type, indirect, drawcount, maxdrawcount, stride )
%
% C function:  void glMultiDrawElementsIndirectCount(GLenum mode, GLenum type, const void* indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawElementsIndirectCount', mode, type, indirect, drawcount, maxdrawcount, stride );

return
