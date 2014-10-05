function glMakeBufferResidentNV( target, access )

% glMakeBufferResidentNV  Interface to OpenGL function glMakeBufferResidentNV
%
% usage:  glMakeBufferResidentNV( target, access )
%
% C function:  void glMakeBufferResidentNV(GLenum target, GLenum access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMakeBufferResidentNV', target, access );

return
