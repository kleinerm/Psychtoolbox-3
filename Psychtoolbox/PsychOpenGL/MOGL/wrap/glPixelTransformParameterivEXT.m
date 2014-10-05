function glPixelTransformParameterivEXT( target, pname, params )

% glPixelTransformParameterivEXT  Interface to OpenGL function glPixelTransformParameterivEXT
%
% usage:  glPixelTransformParameterivEXT( target, pname, params )
%
% C function:  void glPixelTransformParameterivEXT(GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPixelTransformParameterivEXT', target, pname, int32(params) );

return
