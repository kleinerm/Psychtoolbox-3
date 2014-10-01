function glNamedProgramLocalParameters4fvEXT( program, target, index, count, params )

% glNamedProgramLocalParameters4fvEXT  Interface to OpenGL function glNamedProgramLocalParameters4fvEXT
%
% usage:  glNamedProgramLocalParameters4fvEXT( program, target, index, count, params )
%
% C function:  void glNamedProgramLocalParameters4fvEXT(GLuint program, GLenum target, GLuint index, GLsizei count, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameters4fvEXT', program, target, index, count, single(params) );

return
