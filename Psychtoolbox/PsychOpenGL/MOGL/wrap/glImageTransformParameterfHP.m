function glImageTransformParameterfHP( target, pname, param )

% glImageTransformParameterfHP  Interface to OpenGL function glImageTransformParameterfHP
%
% usage:  glImageTransformParameterfHP( target, pname, param )
%
% C function:  void glImageTransformParameterfHP(GLenum target, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glImageTransformParameterfHP', target, pname, param );

return
