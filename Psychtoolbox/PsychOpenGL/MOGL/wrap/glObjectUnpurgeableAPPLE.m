function r = glObjectUnpurgeableAPPLE( objectType, name, option )

% glObjectUnpurgeableAPPLE  Interface to OpenGL function glObjectUnpurgeableAPPLE
%
% usage:  r = glObjectUnpurgeableAPPLE( objectType, name, option )
%
% C function:  GLenum glObjectUnpurgeableAPPLE(GLenum objectType, GLuint name, GLenum option)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glObjectUnpurgeableAPPLE', objectType, name, option );

return
