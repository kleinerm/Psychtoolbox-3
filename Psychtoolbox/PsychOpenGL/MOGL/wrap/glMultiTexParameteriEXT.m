function glMultiTexParameteriEXT( texunit, target, pname, param )

% glMultiTexParameteriEXT  Interface to OpenGL function glMultiTexParameteriEXT
%
% usage:  glMultiTexParameteriEXT( texunit, target, pname, param )
%
% C function:  void glMultiTexParameteriEXT(GLenum texunit, GLenum target, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexParameteriEXT', texunit, target, pname, param );

return
