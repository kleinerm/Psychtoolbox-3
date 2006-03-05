function glmClose

% glmClose  Close the screen
% 
% usage:  glmClose

% 09-Dec-2005 -- created (RFM)

% ---protected---

% close figure that catches keypresses
glmGetChar('close');

% show mouse cursor
glmSetMouse([],1);

% clear front and back buffers
global GL
glClearColor(0,0,0,1);
glClear(mor(GL.COLOR_BUFFER_BIT,GL.DEPTH_BUFFER_BIT,GL.ACCUM_BUFFER_BIT,GL.STENCIL_BUFFER_BIT));
glmSwapBuffers;
glClear(mor(GL.COLOR_BUFFER_BIT,GL.DEPTH_BUFFER_BIT,GL.ACCUM_BUFFER_BIT,GL.STENCIL_BUFFER_BIT));
glmSwapBuffers;

% close screen
moglcore('glmClose');

% restart update process
glmSetUpdate(1);

return
