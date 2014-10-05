function glBufferAddressRangeNV( pname, index, address, length )

% glBufferAddressRangeNV  Interface to OpenGL function glBufferAddressRangeNV
%
% usage:  glBufferAddressRangeNV( pname, index, address, length )
%
% C function:  void glBufferAddressRangeNV(GLenum pname, GLuint index, GLuint64EXT address, GLsizeiptr length)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferAddressRangeNV', pname, index, address, length );

return
