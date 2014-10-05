function r = glIsEnabledi( target, index )

% glIsEnabledi  Interface to OpenGL function glIsEnabledi
%
% usage:  r = glIsEnabledi( target, index )
%
% C function:  GLboolean glIsEnabledi(GLenum target, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glIsEnabledi', target, index );

return
