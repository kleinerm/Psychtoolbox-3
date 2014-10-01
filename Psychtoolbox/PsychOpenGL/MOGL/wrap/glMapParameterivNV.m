function glMapParameterivNV( target, pname, params )

% glMapParameterivNV  Interface to OpenGL function glMapParameterivNV
%
% usage:  glMapParameterivNV( target, pname, params )
%
% C function:  void glMapParameterivNV(GLenum target, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMapParameterivNV', target, pname, int32(params) );

return
