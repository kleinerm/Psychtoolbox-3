% ConserveVRAMSettings: Workaround for flawed hardware and drivers
%
% The command Screen('Preference', 'ConserveVRAM', mode); can be used to
% enable a couple of special work-arounds inside Screen to work around
% broken operating systems, graphics drivers or graphics hardware, or to
% work around ressource limitations of graphics hardware.
%
% You define the requested workaround by setting the parameter 'mode' to a
% sum of the following values:
%
% Allowerd summands (flags) for 'mode' and their effect:
%
% 1 == kPsychDisableAUXBuffers: A setting of 1 asks Psychtoolbox to not
% allocate any OpenGL AUXiliary buffers when opening a new onscreen window.
% AUX buffers are only needed if you want to run the Screen('Flip') command
% with the optional argument 'dontclear = 1' and you are not using the
% imaging pipeline, or if you want to use stereomode 2 or 3 without using
% the imaging pipeline. If you do use the imaging pipeline or don't use any
% of the above, there's no need for AUX buffers.
%
% This setting is mostly meant to save a bit of VRAM on graphics hardware
% that only has very small amounts of VRAM, e.g., only 16 MB or 8 MB VRAM.
%
%
% 2 == kPsychDontCacheTextures: A setting of 2 asks Psychtoolbox not to
% cache used textures in the graphics hardware local VRAM memory. This will
% save some VRAM memory at the expense of lower drawing performance. Only
% useful on gfx-hardware with low amounts of VRAM and only works on
% MacOS/X. The flag is silently ignored on Windows and Linux.
%
%
% 4 == kPsychOverrideWglChoosePixelformat: This is a workaround for broken
% MS-Windows graphics drivers: Ask Screen to not use the
% wglChoosePixelFormat() command when creating a new onscreen window. This
% can prevent crashes on such broken setups, but it will disable OpenGL
% multisampling for anti-aliasing, ie., the 'multisample' parameter of
% Screen('OpenWindow') will be ignored. In the future, other special
% capabilities will be disabled as well.
%
%
% 8 == kPsychDisableContextIsolation: This is a workaround for broken
% MS-Windows graphics drivers: Do not create separate isolated OpenGL
% rendering contexts for Screen and MOGL when using low level OpenGL 3D
% graphics commands with OpenGL for Matlab. This prevents crashes on broken
% setups, but debugging of your own 3D code may become much harder. Its
% better to upgrade to the latest fixed drivers.
%
%
% 16 == kPsychDontAttachStencilToFBO: Do not attach stencil buffer
% attachments to OpenGL framebuffer objects when using OpenGL 3D graphics
% in conjunction with the Psychtoolbox imaging pipeline. This is again a
% workaround for some broken MS-Windows graphics drivers to make the 3D +
% imaging combo work at least when no stencil buffer is needed.
%
%
% --> It's always better to update your graphics drivers with fixed
% versions or buy proper hardware than using these workarounds. They are
% meant as a last ressort, e.g., if you need to get something going quickly
% or can't get access to bug-fixed drivers.
%
