function glColorTableParameterfvSGI( target, pname, params )

% glColorTableParameterfvSGI  Interface to OpenGL function glColorTableParameterfvSGI
%
% usage:  glColorTableParameterfvSGI( target, pname, params )
%
% C function:  void glColorTableParameterfvSGI(GLenum target, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColorTableParameterfvSGI', target, pname, single(params) );

return
