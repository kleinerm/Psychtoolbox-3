function glStencilOpSeparateATI( face, sfail, dpfail, dppass )

% glStencilOpSeparateATI  Interface to OpenGL function glStencilOpSeparateATI
%
% usage:  glStencilOpSeparateATI( face, sfail, dpfail, dppass )
%
% C function:  void glStencilOpSeparateATI(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glStencilOpSeparateATI', face, sfail, dpfail, dppass );

return
