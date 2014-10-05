function glPixelTransformParameterfEXT( target, pname, param )

% glPixelTransformParameterfEXT  Interface to OpenGL function glPixelTransformParameterfEXT
%
% usage:  glPixelTransformParameterfEXT( target, pname, param )
%
% C function:  void glPixelTransformParameterfEXT(GLenum target, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPixelTransformParameterfEXT', target, pname, param );

return
