function glNamedProgramLocalParameter4dvEXT( program, target, index, params )

% glNamedProgramLocalParameter4dvEXT  Interface to OpenGL function glNamedProgramLocalParameter4dvEXT
%
% usage:  glNamedProgramLocalParameter4dvEXT( program, target, index, params )
%
% C function:  void glNamedProgramLocalParameter4dvEXT(GLuint program, GLenum target, GLuint index, const GLdouble* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameter4dvEXT', program, target, index, double(params) );

return
