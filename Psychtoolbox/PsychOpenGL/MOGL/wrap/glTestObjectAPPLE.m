function r = glTestObjectAPPLE( object, name )

% glTestObjectAPPLE  Interface to OpenGL function glTestObjectAPPLE
%
% usage:  r = glTestObjectAPPLE( object, name )
%
% C function:  GLboolean glTestObjectAPPLE(GLenum object, GLuint name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glTestObjectAPPLE', object, name );

return
