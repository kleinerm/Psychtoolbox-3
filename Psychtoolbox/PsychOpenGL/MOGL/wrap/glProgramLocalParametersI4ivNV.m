function glProgramLocalParametersI4ivNV( target, index, count, params )

% glProgramLocalParametersI4ivNV  Interface to OpenGL function glProgramLocalParametersI4ivNV
%
% usage:  glProgramLocalParametersI4ivNV( target, index, count, params )
%
% C function:  void glProgramLocalParametersI4ivNV(GLenum target, GLuint index, GLsizei count, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParametersI4ivNV', target, index, count, int32(params) );

return
