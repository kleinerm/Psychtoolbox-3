function glLightf( light, pname, param )

% glLightf  Interface to OpenGL function glLightf
%
% usage:  glLightf( light, pname, param )
%
% C function:  void glLightf(GLenum light, GLenum pname, GLfloat param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glLightf', light, pname, param );

return
