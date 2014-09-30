function glNamedProgramLocalParameter4fvEXT( program, target, index, params )

% glNamedProgramLocalParameter4fvEXT  Interface to OpenGL function glNamedProgramLocalParameter4fvEXT
%
% usage:  glNamedProgramLocalParameter4fvEXT( program, target, index, params )
%
% C function:  void glNamedProgramLocalParameter4fvEXT(GLuint program, GLenum target, GLuint index, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameter4fvEXT', program, target, index, single(params) );

return
