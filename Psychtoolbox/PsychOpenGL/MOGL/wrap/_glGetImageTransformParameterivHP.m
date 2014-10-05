function params = glGetImageTransformParameterivHP( target, pname )

% glGetImageTransformParameterivHP  Interface to OpenGL function glGetImageTransformParameterivHP
%
% usage:  params = glGetImageTransformParameterivHP( target, pname )
%
% C function:  void glGetImageTransformParameterivHP(GLenum target, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetImageTransformParameterivHP', target, pname, params );

return
