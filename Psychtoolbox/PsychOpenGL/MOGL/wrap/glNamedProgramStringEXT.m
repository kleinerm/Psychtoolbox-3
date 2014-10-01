function glNamedProgramStringEXT( program, target, format, len, string )

% glNamedProgramStringEXT  Interface to OpenGL function glNamedProgramStringEXT
%
% usage:  glNamedProgramStringEXT( program, target, format, len, string )
%
% C function:  void glNamedProgramStringEXT(GLuint program, GLenum target, GLenum format, GLsizei len, const void* string)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramStringEXT', program, target, format, len, string );

return
