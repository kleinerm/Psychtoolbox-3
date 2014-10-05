function glPNTrianglesiATI( pname, param )

% glPNTrianglesiATI  Interface to OpenGL function glPNTrianglesiATI
%
% usage:  glPNTrianglesiATI( pname, param )
%
% C function:  void glPNTrianglesiATI(GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPNTrianglesiATI', pname, param );

return
