function glActiveStencilFaceEXT( face )

% glActiveStencilFaceEXT  Interface to OpenGL function glActiveStencilFaceEXT
%
% usage:  glActiveStencilFaceEXT( face )
%
% C function:  void glActiveStencilFaceEXT(GLenum face)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glActiveStencilFaceEXT', face );

return
