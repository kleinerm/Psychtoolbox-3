function BrightSideCore(varargin)
% BrightSideCore -- Matlab MEX file for low-level interfacing with
% BrightSide technologies HDR display controller library.
%
% Don't call this file directly, but use the user friendly BrightSideHDR()
% command instead!!
%
% Low-Level command codes:
%
% BrightSideCore(0, configpath, configfile, texid, fboid);
% - Initialize core, read config file 'configfile' located in path
% 'configpath'. Use OpenGL texture 'texid' as input texture and framebuffer
% 'fboid' for HDR --> LDR conversion blit.
%
% BrightSideCore(1);
% - Shut down brightside system, release all ressources.
%
% BrightSideCore(2);
% - Convert HDR image content of input texture to an image suitable for
% driving the HDR display, blit it into the real LDR framebuffer.
%
% BrightSideCore(3, texid, fboid);
% - Change source texture to 'texid' and target framebuffer to 'fboid' for conversion.
%
% BrightSideCore(4, ledintensity);
% - Change output multiplication factor for intensity of the LED array.
%

% History:
% October 2006  Written (MK).
error('The BrightSideCore MEX file seems to be either invalid, not loadable or missing in your setup!');
return
