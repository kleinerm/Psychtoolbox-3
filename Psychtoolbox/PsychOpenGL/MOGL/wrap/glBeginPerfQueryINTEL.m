function glBeginPerfQueryINTEL( queryHandle )

% glBeginPerfQueryINTEL  Interface to OpenGL function glBeginPerfQueryINTEL
%
% usage:  glBeginPerfQueryINTEL( queryHandle )
%
% C function:  void glBeginPerfQueryINTEL(GLuint queryHandle)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBeginPerfQueryINTEL', queryHandle );

return
