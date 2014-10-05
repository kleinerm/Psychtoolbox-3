function glMapParameterfvNV( target, pname, params )

% glMapParameterfvNV  Interface to OpenGL function glMapParameterfvNV
%
% usage:  glMapParameterfvNV( target, pname, params )
%
% C function:  void glMapParameterfvNV(GLenum target, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMapParameterfvNV', target, pname, single(params) );

return
