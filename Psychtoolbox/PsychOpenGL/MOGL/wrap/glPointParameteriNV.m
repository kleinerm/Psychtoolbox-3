function glPointParameteriNV( pname, param )

% glPointParameteriNV  Interface to OpenGL function glPointParameteriNV
%
% usage:  glPointParameteriNV( pname, param )
%
% C function:  void glPointParameteriNV(GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameteriNV', pname, param );

return
