function glConservativeRasterParameterfNV( pname, value )

% glConservativeRasterParameterfNV  Interface to OpenGL function glConservativeRasterParameterfNV
%
% usage:  glConservativeRasterParameterfNV( pname, value )
%
% C function:  void glConservativeRasterParameterfNV(GLenum pname, GLfloat value)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glConservativeRasterParameterfNV', pname, value );

return
