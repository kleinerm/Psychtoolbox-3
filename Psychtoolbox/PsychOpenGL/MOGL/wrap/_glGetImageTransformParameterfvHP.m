function params = glGetImageTransformParameterfvHP( target, pname )

% glGetImageTransformParameterfvHP  Interface to OpenGL function glGetImageTransformParameterfvHP
%
% usage:  params = glGetImageTransformParameterfvHP( target, pname )
%
% C function:  void glGetImageTransformParameterfvHP(GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetImageTransformParameterfvHP', target, pname, params );

return
