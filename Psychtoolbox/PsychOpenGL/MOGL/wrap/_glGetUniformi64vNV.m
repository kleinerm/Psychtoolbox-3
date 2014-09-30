function params = glGetUniformi64vNV( program, location )

% glGetUniformi64vNV  Interface to OpenGL function glGetUniformi64vNV
%
% usage:  params = glGetUniformi64vNV( program, location )
%
% C function:  void glGetUniformi64vNV(GLuint program, GLint location, GLint64EXT* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int64(0);

moglcore( 'glGetUniformi64vNV', program, location, params );

return
