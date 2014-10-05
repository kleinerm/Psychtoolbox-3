function params = glGetUniformui64vNV( program, location )

% glGetUniformui64vNV  Interface to OpenGL function glGetUniformui64vNV
%
% usage:  params = glGetUniformui64vNV( program, location )
%
% C function:  void glGetUniformui64vNV(GLuint program, GLint location, GLuint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint64(0);

moglcore( 'glGetUniformui64vNV', program, location, params );

return
