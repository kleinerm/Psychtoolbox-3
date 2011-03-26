function glMinmax( target, internalformat, sink )

% glMinmax  Interface to OpenGL function glMinmax
%
% usage:  glMinmax( target, internalformat, sink )
%
% C function:  void glMinmax(GLenum target, GLenum internalformat, GLboolean sink)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMinmax', target, internalformat, sink );

return
