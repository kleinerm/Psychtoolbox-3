function glImageTransformParameterivHP( target, pname, params )

% glImageTransformParameterivHP  Interface to OpenGL function glImageTransformParameterivHP
%
% usage:  glImageTransformParameterivHP( target, pname, params )
%
% C function:  void glImageTransformParameterivHP(GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glImageTransformParameterivHP', target, pname, int32(params) );

return
