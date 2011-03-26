function glFogf( pname, param )

% glFogf  Interface to OpenGL function glFogf
%
% usage:  glFogf( pname, param )
%
% C function:  void glFogf(GLenum pname, GLfloat param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFogf', pname, param );

return
