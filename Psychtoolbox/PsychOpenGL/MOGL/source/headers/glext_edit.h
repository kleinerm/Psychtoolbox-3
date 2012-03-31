/*
	Copyright:  (c) 1999-2005 by Apple Computer, Inc., all rights reserved.
*/
/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.1 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
**
** http://oss.sgi.com/projects/FreeB
**
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
**
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
**
** Additional Notice Provisions: This software was created using the
** OpenGL(R) version 1.2.1 Sample Implementation published by SGI, but has
** not been independently verified as being compliant with the OpenGL(R)
** version 1.2.1 Specification.
*/

extern GLboolean glIsRenderbufferEXT(GLuint renderbuffer);
extern void glBindRenderbufferEXT(GLenum target, GLuint renderbuffer);
extern void glDeleteRenderbuffersEXT(GLsizei n, const GLuint *renderbuffers);
extern void glGenRenderbuffersEXT(GLsizei n, GLuint *renderbuffers);
extern void glRenderbufferStorageEXT(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
extern void glGetRenderbufferParameterivEXT(GLenum target, GLenum pname, GLint *params);
extern GLboolean glIsFramebufferEXT(GLuint framebuffer);
extern void glBindFramebufferEXT(GLenum target, GLuint framebuffer);
extern void glDeleteFramebuffersEXT(GLsizei n, const GLuint *framebuffers);
extern void glGenFramebuffersEXT(GLsizei n, GLuint *framebuffers);
extern GLenum glCheckFramebufferStatusEXT(GLenum target);
extern void glFramebufferTexture1DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
extern void glFramebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
extern void glFramebufferTexture3DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
extern void glFramebufferRenderbufferEXT(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
extern void glGetFramebufferAttachmentParameterivEXT(GLenum target, GLenum attachment, GLenum pname, GLint *params);
extern void glGenerateMipmapEXT(GLenum target);
extern void glSampleMaskSGIS (GLclampf, GLboolean);
extern void glSamplePatternSGIS (GLenum);
