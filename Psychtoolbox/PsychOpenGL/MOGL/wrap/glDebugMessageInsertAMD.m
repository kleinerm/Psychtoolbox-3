function glDebugMessageInsertAMD( category, severity, id, length, buf )

% glDebugMessageInsertAMD  Interface to OpenGL function glDebugMessageInsertAMD
%
% usage:  glDebugMessageInsertAMD( category, severity, id, length, buf )
%
% C function:  void glDebugMessageInsertAMD(GLenum category, GLenum severity, GLuint id, GLsizei length, const GLchar* buf)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glDebugMessageInsertAMD', category, severity, id, length, uint8(buf) );

return
