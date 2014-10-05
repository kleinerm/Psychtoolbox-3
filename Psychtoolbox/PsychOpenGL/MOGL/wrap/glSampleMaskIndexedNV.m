function glSampleMaskIndexedNV( index, mask )

% glSampleMaskIndexedNV  Interface to OpenGL function glSampleMaskIndexedNV
%
% usage:  glSampleMaskIndexedNV( index, mask )
%
% C function:  void glSampleMaskIndexedNV(GLuint index, GLbitfield mask)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSampleMaskIndexedNV', index, mask );

return
