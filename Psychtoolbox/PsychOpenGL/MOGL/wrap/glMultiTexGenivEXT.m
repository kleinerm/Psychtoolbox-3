function glMultiTexGenivEXT( texunit, coord, pname, params )

% glMultiTexGenivEXT  Interface to OpenGL function glMultiTexGenivEXT
%
% usage:  glMultiTexGenivEXT( texunit, coord, pname, params )
%
% C function:  void glMultiTexGenivEXT(GLenum texunit, GLenum coord, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexGenivEXT', texunit, coord, pname, int32(params) );

return
