function glLightfv( light, pname, params )

% glLightfv  Interface to OpenGL function glLightfv
%
% usage:  glLightfv( light, pname, params )
%
% C function:  void glLightfv(GLenum light, GLenum pname, const GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glLightfv', light, pname, moglsingle(params) );

return
