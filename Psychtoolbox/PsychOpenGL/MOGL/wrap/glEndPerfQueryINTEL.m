function glEndPerfQueryINTEL( queryHandle )

% glEndPerfQueryINTEL  Interface to OpenGL function glEndPerfQueryINTEL
%
% usage:  glEndPerfQueryINTEL( queryHandle )
%
% C function:  void glEndPerfQueryINTEL(GLuint queryHandle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glEndPerfQueryINTEL', queryHandle );

return
