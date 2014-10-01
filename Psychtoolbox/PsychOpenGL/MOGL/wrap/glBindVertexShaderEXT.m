function glBindVertexShaderEXT( id )

% glBindVertexShaderEXT  Interface to OpenGL function glBindVertexShaderEXT
%
% usage:  glBindVertexShaderEXT( id )
%
% C function:  void glBindVertexShaderEXT(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glBindVertexShaderEXT', id );

return
