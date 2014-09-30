function value = glGetPathTexGenivNV( texCoordSet, pname )

% glGetPathTexGenivNV  Interface to OpenGL function glGetPathTexGenivNV
%
% usage:  value = glGetPathTexGenivNV( texCoordSet, pname )
%
% C function:  void glGetPathTexGenivNV(GLenum texCoordSet, GLenum pname, GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value = int32(0);

moglcore( 'glGetPathTexGenivNV', texCoordSet, pname, value );

return
