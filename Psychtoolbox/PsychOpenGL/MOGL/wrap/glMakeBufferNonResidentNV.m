function glMakeBufferNonResidentNV( target )

% glMakeBufferNonResidentNV  Interface to OpenGL function glMakeBufferNonResidentNV
%
% usage:  glMakeBufferNonResidentNV( target )
%
% C function:  void glMakeBufferNonResidentNV(GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMakeBufferNonResidentNV', target );

return
