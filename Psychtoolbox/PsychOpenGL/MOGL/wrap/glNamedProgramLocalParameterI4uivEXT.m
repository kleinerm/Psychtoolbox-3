function glNamedProgramLocalParameterI4uivEXT( program, target, index, params )

% glNamedProgramLocalParameterI4uivEXT  Interface to OpenGL function glNamedProgramLocalParameterI4uivEXT
%
% usage:  glNamedProgramLocalParameterI4uivEXT( program, target, index, params )
%
% C function:  void glNamedProgramLocalParameterI4uivEXT(GLuint program, GLenum target, GLuint index, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedProgramLocalParameterI4uivEXT', program, target, index, uint32(params) );

return
