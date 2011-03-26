function r = gluCheckExtension( extName, extString )

% gluCheckExtension  Interface to OpenGL function gluCheckExtension
%
% usage:  r = gluCheckExtension( extName, extString )
%
% C function:  GLboolean gluCheckExtension(const GLubyte* extName, const GLubyte* extString)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'gluCheckExtension', uint8(extName), uint8(extString) );

return
