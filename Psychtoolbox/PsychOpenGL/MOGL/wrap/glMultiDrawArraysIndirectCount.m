function glMultiDrawArraysIndirectCount( mode, indirect, drawcount, maxdrawcount, stride )

% glMultiDrawArraysIndirectCount  Interface to OpenGL function glMultiDrawArraysIndirectCount
%
% usage:  glMultiDrawArraysIndirectCount( mode, indirect, drawcount, maxdrawcount, stride )
%
% C function:  void glMultiDrawArraysIndirectCount(GLenum mode, const void* indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawArraysIndirectCount', mode, indirect, drawcount, maxdrawcount, stride );

return
