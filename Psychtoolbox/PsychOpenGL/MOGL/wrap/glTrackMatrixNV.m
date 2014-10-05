function glTrackMatrixNV( target, address, matrix, transform )

% glTrackMatrixNV  Interface to OpenGL function glTrackMatrixNV
%
% usage:  glTrackMatrixNV( target, address, matrix, transform )
%
% C function:  void glTrackMatrixNV(GLenum target, GLuint address, GLenum matrix, GLenum transform)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTrackMatrixNV', target, address, matrix, transform );

return
