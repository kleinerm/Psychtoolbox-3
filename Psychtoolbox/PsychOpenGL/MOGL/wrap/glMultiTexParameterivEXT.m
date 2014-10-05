function glMultiTexParameterivEXT( texunit, target, pname, params )

% glMultiTexParameterivEXT  Interface to OpenGL function glMultiTexParameterivEXT
%
% usage:  glMultiTexParameterivEXT( texunit, target, pname, params )
%
% C function:  void glMultiTexParameterivEXT(GLenum texunit, GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexParameterivEXT', texunit, target, pname, int32(params) );

return
