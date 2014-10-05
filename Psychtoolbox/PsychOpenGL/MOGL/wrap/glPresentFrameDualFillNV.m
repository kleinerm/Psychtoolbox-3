function glPresentFrameDualFillNV( video_slot, minPresentTime, beginPresentTimeId, presentDurationId, type, target0, fill0, target1, fill1, target2, fill2, target3, fill3 )

% glPresentFrameDualFillNV  Interface to OpenGL function glPresentFrameDualFillNV
%
% usage:  glPresentFrameDualFillNV( video_slot, minPresentTime, beginPresentTimeId, presentDurationId, type, target0, fill0, target1, fill1, target2, fill2, target3, fill3 )
%
% C function:  void glPresentFrameDualFillNV(GLuint video_slot, GLuint64EXT minPresentTime, GLuint beginPresentTimeId, GLuint presentDurationId, GLenum type, GLenum target0, GLuint fill0, GLenum target1, GLuint fill1, GLenum target2, GLuint fill2, GLenum target3, GLuint fill3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=13,
    error('invalid number of arguments');
end

moglcore( 'glPresentFrameDualFillNV', video_slot, minPresentTime, beginPresentTimeId, presentDurationId, type, target0, fill0, target1, fill1, target2, fill2, target3, fill3 );

return
