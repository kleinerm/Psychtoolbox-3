function r = glIsVertexArrayAPPLE( array )

% glIsVertexArrayAPPLE  Interface to OpenGL function glIsVertexArrayAPPLE
%
% usage:  r = glIsVertexArrayAPPLE( array )
%
% C function:  GLboolean glIsVertexArrayAPPLE(GLuint array)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsVertexArrayAPPLE', array );

return
