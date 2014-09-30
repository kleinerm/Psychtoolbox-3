function glMultiTexParameterfvEXT( texunit, target, pname, params )

% glMultiTexParameterfvEXT  Interface to OpenGL function glMultiTexParameterfvEXT
%
% usage:  glMultiTexParameterfvEXT( texunit, target, pname, params )
%
% C function:  void glMultiTexParameterfvEXT(GLenum texunit, GLenum target, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexParameterfvEXT', texunit, target, pname, single(params) );

return
