function glMakeNamedBufferNonResidentNV( buffer )

% glMakeNamedBufferNonResidentNV  Interface to OpenGL function glMakeNamedBufferNonResidentNV
%
% usage:  glMakeNamedBufferNonResidentNV( buffer )
%
% C function:  void glMakeNamedBufferNonResidentNV(GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMakeNamedBufferNonResidentNV', buffer );

return
