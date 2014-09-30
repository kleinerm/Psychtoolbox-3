function glImageTransformParameteriHP( target, pname, param )

% glImageTransformParameteriHP  Interface to OpenGL function glImageTransformParameteriHP
%
% usage:  glImageTransformParameteriHP( target, pname, param )
%
% C function:  void glImageTransformParameteriHP(GLenum target, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glImageTransformParameteriHP', target, pname, param );

return
