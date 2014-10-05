function glMinmaxEXT( target, internalformat, sink )

% glMinmaxEXT  Interface to OpenGL function glMinmaxEXT
%
% usage:  glMinmaxEXT( target, internalformat, sink )
%
% C function:  void glMinmaxEXT(GLenum target, GLenum internalformat, GLboolean sink)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMinmaxEXT', target, internalformat, sink );

return
