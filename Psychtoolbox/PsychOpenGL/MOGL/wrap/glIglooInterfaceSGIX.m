function glIglooInterfaceSGIX( pname, params )

% glIglooInterfaceSGIX  Interface to OpenGL function glIglooInterfaceSGIX
%
% usage:  glIglooInterfaceSGIX( pname, params )
%
% C function:  void glIglooInterfaceSGIX(GLenum pname, const void* params)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glIglooInterfaceSGIX', pname, params );

return
