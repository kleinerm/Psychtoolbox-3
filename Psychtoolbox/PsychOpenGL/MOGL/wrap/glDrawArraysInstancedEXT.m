function glDrawArraysInstancedEXT( mode, start, count, primcount )

% glDrawArraysInstancedEXT  Interface to OpenGL function glDrawArraysInstancedEXT
%
% usage:  glDrawArraysInstancedEXT( mode, start, count, primcount )
%
% C function:  void glDrawArraysInstancedEXT(GLenum mode, GLint start, GLsizei count, GLsizei primcount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glDrawArraysInstancedEXT', mode, start, count, primcount );

return
