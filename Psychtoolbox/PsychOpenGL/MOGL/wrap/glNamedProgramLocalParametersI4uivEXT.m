function glNamedProgramLocalParametersI4uivEXT( program, target, index, count, params )

% glNamedProgramLocalParametersI4uivEXT  Interface to OpenGL function glNamedProgramLocalParametersI4uivEXT
%
% usage:  glNamedProgramLocalParametersI4uivEXT( program, target, index, count, params )
%
% C function:  void glNamedProgramLocalParametersI4uivEXT(GLuint program, GLenum target, GLuint index, GLsizei count, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParametersI4uivEXT', program, target, index, count, uint32(params) );

return
