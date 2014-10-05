function glDeleteNamesAMD( identifier, num, names )

% glDeleteNamesAMD  Interface to OpenGL function glDeleteNamesAMD
%
% usage:  glDeleteNamesAMD( identifier, num, names )
%
% C function:  void glDeleteNamesAMD(GLenum identifier, GLuint num, const GLuint* names)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glDeleteNamesAMD', identifier, num, uint32(names) );

return
