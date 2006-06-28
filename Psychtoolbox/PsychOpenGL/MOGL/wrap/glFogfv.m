function glFogfv( pname, params )

% glFogfv  Interface to OpenGL function glFogfv
%
% usage:  glFogfv( pname, params )
%
% C function:  void glFogfv(GLenum pname, const GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFogfv', pname, moglsingle(params) );

return
