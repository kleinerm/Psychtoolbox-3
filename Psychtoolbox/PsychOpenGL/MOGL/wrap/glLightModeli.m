function glLightModeli( pname, param )

% glLightModeli  Interface to OpenGL function glLightModeli
%
% usage:  glLightModeli( pname, param )
%
% C function:  void glLightModeli(GLenum pname, GLint param)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glLightModeli', pname, param );

return
