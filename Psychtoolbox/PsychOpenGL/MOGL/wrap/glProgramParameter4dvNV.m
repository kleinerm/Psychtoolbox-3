function glProgramParameter4dvNV( target, index, v )

% glProgramParameter4dvNV  Interface to OpenGL function glProgramParameter4dvNV
%
% usage:  glProgramParameter4dvNV( target, index, v )
%
% C function:  void glProgramParameter4dvNV(GLenum target, GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramParameter4dvNV', target, index, double(v) );

return
