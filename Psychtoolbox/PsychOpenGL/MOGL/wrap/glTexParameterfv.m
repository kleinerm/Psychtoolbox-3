function glTexParameterfv( target, pname, params )

% glTexParameterfv  Interface to OpenGL function glTexParameterfv
%
% usage:  glTexParameterfv( target, pname, params )
%
% C function:  void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameterfv', target, pname, single(params) );

return
