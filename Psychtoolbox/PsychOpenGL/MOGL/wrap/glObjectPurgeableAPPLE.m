function r = glObjectPurgeableAPPLE( objectType, name, option )

% glObjectPurgeableAPPLE  Interface to OpenGL function glObjectPurgeableAPPLE
%
% usage:  r = glObjectPurgeableAPPLE( objectType, name, option )
%
% C function:  GLenum glObjectPurgeableAPPLE(GLenum objectType, GLuint name, GLenum option)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

r = moglcore( 'glObjectPurgeableAPPLE', objectType, name, option );

return
