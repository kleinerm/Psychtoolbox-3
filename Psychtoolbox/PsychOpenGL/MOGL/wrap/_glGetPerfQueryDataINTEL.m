function [ data, bytesWritten ] = glGetPerfQueryDataINTEL( queryHandle, flags, dataSize )

% glGetPerfQueryDataINTEL  Interface to OpenGL function glGetPerfQueryDataINTEL
%
% usage:  [ data, bytesWritten ] = glGetPerfQueryDataINTEL( queryHandle, flags, dataSize )
%
% C function:  void glGetPerfQueryDataINTEL(GLuint queryHandle, GLuint flags, GLsizei dataSize, GLvoid* data, GLuint* bytesWritten)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);
bytesWritten = uint32(0);

moglcore( 'glGetPerfQueryDataINTEL', queryHandle, flags, dataSize, data, bytesWritten );

return
