function glPNTrianglesfATI( pname, param )

% glPNTrianglesfATI  Interface to OpenGL function glPNTrianglesfATI
%
% usage:  glPNTrianglesfATI( pname, param )
%
% C function:  void glPNTrianglesfATI(GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPNTrianglesfATI', pname, param );

return
