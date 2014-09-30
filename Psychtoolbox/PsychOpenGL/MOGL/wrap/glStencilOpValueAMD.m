function glStencilOpValueAMD( face, value )

% glStencilOpValueAMD  Interface to OpenGL function glStencilOpValueAMD
%
% usage:  glStencilOpValueAMD( face, value )
%
% C function:  void glStencilOpValueAMD(GLenum face, GLuint value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glStencilOpValueAMD', face, value );

return
