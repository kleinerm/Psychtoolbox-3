function glPointParameterfEXT( pname, param )

% glPointParameterfEXT  Interface to OpenGL function glPointParameterfEXT
%
% usage:  glPointParameterfEXT( pname, param )
%
% C function:  void glPointParameterfEXT(GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameterfEXT', pname, param );

return
