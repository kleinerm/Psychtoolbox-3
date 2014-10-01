function glDrawElementsInstancedEXT( mode, count, type, indices, primcount )

% glDrawElementsInstancedEXT  Interface to OpenGL function glDrawElementsInstancedEXT
%
% usage:  glDrawElementsInstancedEXT( mode, count, type, indices, primcount )
%
% C function:  void glDrawElementsInstancedEXT(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei primcount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glDrawElementsInstancedEXT', mode, count, type, indices, primcount );

return
