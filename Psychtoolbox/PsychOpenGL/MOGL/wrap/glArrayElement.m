function glArrayElement( i )

% glArrayElement  Interface to OpenGL function glArrayElement
%
% usage:  glArrayElement( i )
%
% C function:  void glArrayElement(GLint i)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glArrayElement', i );

return
