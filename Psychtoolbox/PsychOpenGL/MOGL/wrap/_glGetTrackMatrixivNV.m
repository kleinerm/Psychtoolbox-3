function params = glGetTrackMatrixivNV( target, address, pname )

% glGetTrackMatrixivNV  Interface to OpenGL function glGetTrackMatrixivNV
%
% usage:  params = glGetTrackMatrixivNV( target, address, pname )
%
% C function:  void glGetTrackMatrixivNV(GLenum target, GLuint address, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTrackMatrixivNV', target, address, pname, params );

return
