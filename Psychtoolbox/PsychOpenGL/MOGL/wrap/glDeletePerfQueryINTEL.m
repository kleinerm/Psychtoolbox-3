function glDeletePerfQueryINTEL( queryHandle )

% glDeletePerfQueryINTEL  Interface to OpenGL function glDeletePerfQueryINTEL
%
% usage:  glDeletePerfQueryINTEL( queryHandle )
%
% C function:  void glDeletePerfQueryINTEL(GLuint queryHandle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDeletePerfQueryINTEL', queryHandle );

return
