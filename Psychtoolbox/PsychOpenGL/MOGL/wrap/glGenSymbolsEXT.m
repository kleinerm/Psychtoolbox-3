function r = glGenSymbolsEXT( datatype, storagetype, range, components )

% glGenSymbolsEXT  Interface to OpenGL function glGenSymbolsEXT
%
% usage:  r = glGenSymbolsEXT( datatype, storagetype, range, components )
%
% C function:  GLuint glGenSymbolsEXT(GLenum datatype, GLenum storagetype, GLenum range, GLuint components)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

r = moglcore( 'glGenSymbolsEXT', datatype, storagetype, range, components );

return
