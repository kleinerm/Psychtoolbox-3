function value = glGetPathColorGenivNV( color, pname )

% glGetPathColorGenivNV  Interface to OpenGL function glGetPathColorGenivNV
%
% usage:  value = glGetPathColorGenivNV( color, pname )
%
% C function:  void glGetPathColorGenivNV(GLenum color, GLenum pname, GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value = int32(0);

moglcore( 'glGetPathColorGenivNV', color, pname, value );

return
