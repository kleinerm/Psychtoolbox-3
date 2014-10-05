function glPointParameterfvEXT( pname, params )

% glPointParameterfvEXT  Interface to OpenGL function glPointParameterfvEXT
%
% usage:  glPointParameterfvEXT( pname, params )
%
% C function:  void glPointParameterfvEXT(GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameterfvEXT', pname, single(params) );

return
