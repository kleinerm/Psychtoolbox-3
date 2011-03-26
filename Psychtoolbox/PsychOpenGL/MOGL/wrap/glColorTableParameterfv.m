function glColorTableParameterfv( target, pname, params )

% glColorTableParameterfv  Interface to OpenGL function glColorTableParameterfv
%
% usage:  glColorTableParameterfv( target, pname, params )
%
% C function:  void glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat* params)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColorTableParameterfv', target, pname, single(params) );

return
