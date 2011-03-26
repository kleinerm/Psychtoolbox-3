function glPointParameterf( pname, param )

% glPointParameterf  Interface to OpenGL function glPointParameterf
%
% usage:  glPointParameterf( pname, param )
%
% C function:  void glPointParameterf(GLenum pname, GLfloat param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameterf', pname, param );

return
