function glColorTableParameterfv( target, pname, params )

% glColorTableParameterfv  Interface to OpenGL function glColorTableParameterfv
%
% usage:  glColorTableParameterfv( target, pname, params )
%
% C function:  void glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColorTableParameterfv', target, pname, moglsingle(params) );

return
