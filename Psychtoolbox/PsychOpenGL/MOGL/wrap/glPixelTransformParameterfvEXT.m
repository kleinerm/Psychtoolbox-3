function glPixelTransformParameterfvEXT( target, pname, params )

% glPixelTransformParameterfvEXT  Interface to OpenGL function glPixelTransformParameterfvEXT
%
% usage:  glPixelTransformParameterfvEXT( target, pname, params )
%
% C function:  void glPixelTransformParameterfvEXT(GLenum target, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPixelTransformParameterfvEXT', target, pname, single(params) );

return
