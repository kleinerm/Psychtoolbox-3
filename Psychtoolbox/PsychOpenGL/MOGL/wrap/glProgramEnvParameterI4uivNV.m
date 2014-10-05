function glProgramEnvParameterI4uivNV( target, index, params )

% glProgramEnvParameterI4uivNV  Interface to OpenGL function glProgramEnvParameterI4uivNV
%
% usage:  glProgramEnvParameterI4uivNV( target, index, params )
%
% C function:  void glProgramEnvParameterI4uivNV(GLenum target, GLuint index, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParameterI4uivNV', target, index, uint32(params) );

return
