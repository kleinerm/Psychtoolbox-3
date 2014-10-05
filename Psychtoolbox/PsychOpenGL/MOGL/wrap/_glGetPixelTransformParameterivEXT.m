function params = glGetPixelTransformParameterivEXT( target, pname )

% glGetPixelTransformParameterivEXT  Interface to OpenGL function glGetPixelTransformParameterivEXT
%
% usage:  params = glGetPixelTransformParameterivEXT( target, pname )
%
% C function:  void glGetPixelTransformParameterivEXT(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetPixelTransformParameterivEXT', target, pname, params );

return
