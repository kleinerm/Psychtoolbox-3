function glProgramLocalParameterI4uiNV( target, index, x, y, z, w )

% glProgramLocalParameterI4uiNV  Interface to OpenGL function glProgramLocalParameterI4uiNV
%
% usage:  glProgramLocalParameterI4uiNV( target, index, x, y, z, w )
%
% C function:  void glProgramLocalParameterI4uiNV(GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glProgramLocalParameterI4uiNV', target, index, x, y, z, w );

return
