function glRasterSamplesEXT( samples, fixedsamplelocations )

% glRasterSamplesEXT  Interface to OpenGL function glRasterSamplesEXT
%
% usage:  glRasterSamplesEXT( samples, fixedsamplelocations )
%
% C function:  void glRasterSamplesEXT(GLuint samples, GLboolean fixedsamplelocations)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRasterSamplesEXT', samples, fixedsamplelocations );

return
