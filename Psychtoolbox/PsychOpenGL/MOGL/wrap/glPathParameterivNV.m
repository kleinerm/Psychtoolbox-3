function glPathParameterivNV( path, pname, value )

% glPathParameterivNV  Interface to OpenGL function glPathParameterivNV
%
% usage:  glPathParameterivNV( path, pname, value )
%
% C function:  void glPathParameterivNV(GLuint path, GLenum pname, const GLint* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPathParameterivNV', path, pname, int32(value) );

return
