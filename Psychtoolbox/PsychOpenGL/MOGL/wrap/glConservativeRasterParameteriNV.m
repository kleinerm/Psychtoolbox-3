function glConservativeRasterParameteriNV( pname, param )

% glConservativeRasterParameteriNV  Interface to OpenGL function glConservativeRasterParameteriNV
%
% usage:  glConservativeRasterParameteriNV( pname, param )
%
% C function:  void glConservativeRasterParameteriNV(GLenum pname, GLint param)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glConservativeRasterParameteriNV', pname, param );

return
