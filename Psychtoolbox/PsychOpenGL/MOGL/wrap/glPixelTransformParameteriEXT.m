function glPixelTransformParameteriEXT( target, pname, param )

% glPixelTransformParameteriEXT  Interface to OpenGL function glPixelTransformParameteriEXT
%
% usage:  glPixelTransformParameteriEXT( target, pname, param )
%
% C function:  void glPixelTransformParameteriEXT(GLenum target, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPixelTransformParameteriEXT', target, pname, param );

return
