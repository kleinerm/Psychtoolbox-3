function glPointParameterivNV( pname, params )

% glPointParameterivNV  Interface to OpenGL function glPointParameterivNV
%
% usage:  glPointParameterivNV( pname, params )
%
% C function:  void glPointParameterivNV(GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPointParameterivNV', pname, int32(params) );

return
