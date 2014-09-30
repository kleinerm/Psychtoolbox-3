function glDeleteVertexShaderEXT( id )

% glDeleteVertexShaderEXT  Interface to OpenGL function glDeleteVertexShaderEXT
%
% usage:  glDeleteVertexShaderEXT( id )
%
% C function:  void glDeleteVertexShaderEXT(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glDeleteVertexShaderEXT', id );

return
