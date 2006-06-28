function glPointParameterfv( pname, params )

% glPointParameterfv  Interface to OpenGL function glPointParameterfv
%
% usage:  glPointParameterfv( pname, params )
%
% C function:  void glPointParameterfv(GLenum pname, const GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameterfv', pname, moglsingle(params) );

return
