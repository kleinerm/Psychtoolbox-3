function glUseShaderProgramEXT( type, program )

% glUseShaderProgramEXT  Interface to OpenGL function glUseShaderProgramEXT
%
% usage:  glUseShaderProgramEXT( type, program )
%
% C function:  void glUseShaderProgramEXT(GLenum type, GLuint program)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUseShaderProgramEXT', type, program );

return
