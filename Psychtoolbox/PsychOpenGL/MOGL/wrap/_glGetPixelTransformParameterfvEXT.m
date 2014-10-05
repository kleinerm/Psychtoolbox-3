function params = glGetPixelTransformParameterfvEXT( target, pname )

% glGetPixelTransformParameterfvEXT  Interface to OpenGL function glGetPixelTransformParameterfvEXT
%
% usage:  params = glGetPixelTransformParameterfvEXT( target, pname )
%
% C function:  void glGetPixelTransformParameterfvEXT(GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetPixelTransformParameterfvEXT', target, pname, params );

return
