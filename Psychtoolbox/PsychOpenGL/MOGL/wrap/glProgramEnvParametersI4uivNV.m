function glProgramEnvParametersI4uivNV( target, index, count, params )

% glProgramEnvParametersI4uivNV  Interface to OpenGL function glProgramEnvParametersI4uivNV
%
% usage:  glProgramEnvParametersI4uivNV( target, index, count, params )
%
% C function:  void glProgramEnvParametersI4uivNV(GLenum target, GLuint index, GLsizei count, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramEnvParametersI4uivNV', target, index, count, uint32(params) );

return
