function glImageTransformParameterfvHP( target, pname, params )

% glImageTransformParameterfvHP  Interface to OpenGL function glImageTransformParameterfvHP
%
% usage:  glImageTransformParameterfvHP( target, pname, params )
%
% C function:  void glImageTransformParameterfvHP(GLenum target, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glImageTransformParameterfvHP', target, pname, single(params) );

return
