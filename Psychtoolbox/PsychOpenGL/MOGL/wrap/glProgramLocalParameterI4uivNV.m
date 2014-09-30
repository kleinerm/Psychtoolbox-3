function glProgramLocalParameterI4uivNV( target, index, params )

% glProgramLocalParameterI4uivNV  Interface to OpenGL function glProgramLocalParameterI4uivNV
%
% usage:  glProgramLocalParameterI4uivNV( target, index, params )
%
% C function:  void glProgramLocalParameterI4uivNV(GLenum target, GLuint index, const GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameterI4uivNV', target, index, uint32(params) );

return
