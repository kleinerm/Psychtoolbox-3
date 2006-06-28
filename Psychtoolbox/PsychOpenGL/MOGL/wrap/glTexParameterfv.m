function glTexParameterfv( target, pname, params )

% glTexParameterfv  Interface to OpenGL function glTexParameterfv
%
% usage:  glTexParameterfv( target, pname, params )
%
% C function:  void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexParameterfv', target, pname, moglsingle(params) );

return
