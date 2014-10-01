function r = glIsVertexAttribEnabledAPPLE( index, pname )

% glIsVertexAttribEnabledAPPLE  Interface to OpenGL function glIsVertexAttribEnabledAPPLE
%
% usage:  r = glIsVertexAttribEnabledAPPLE( index, pname )
%
% C function:  GLboolean glIsVertexAttribEnabledAPPLE(GLuint index, GLenum pname)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glIsVertexAttribEnabledAPPLE', index, pname );

return
