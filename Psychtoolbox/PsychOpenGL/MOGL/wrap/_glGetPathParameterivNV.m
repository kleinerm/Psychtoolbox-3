function value = glGetPathParameterivNV( path, pname )

% glGetPathParameterivNV  Interface to OpenGL function glGetPathParameterivNV
%
% usage:  value = glGetPathParameterivNV( path, pname )
%
% C function:  void glGetPathParameterivNV(GLuint path, GLenum pname, GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value = int32(0);

moglcore( 'glGetPathParameterivNV', path, pname, value );

return
