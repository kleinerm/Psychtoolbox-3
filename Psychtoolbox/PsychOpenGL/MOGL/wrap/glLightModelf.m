function glLightModelf( pname, param )

% glLightModelf  Interface to OpenGL function glLightModelf
%
% usage:  glLightModelf( pname, param )
%
% C function:  void glLightModelf(GLenum pname, GLfloat param)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glLightModelf', pname, param );

return
