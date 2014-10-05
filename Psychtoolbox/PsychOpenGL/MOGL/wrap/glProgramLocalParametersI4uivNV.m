function glProgramLocalParametersI4uivNV( target, index, count, params )

% glProgramLocalParametersI4uivNV  Interface to OpenGL function glProgramLocalParametersI4uivNV
%
% usage:  glProgramLocalParametersI4uivNV( target, index, count, params )
%
% C function:  void glProgramLocalParametersI4uivNV(GLenum target, GLuint index, GLsizei count, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParametersI4uivNV', target, index, count, uint32(params) );

return
