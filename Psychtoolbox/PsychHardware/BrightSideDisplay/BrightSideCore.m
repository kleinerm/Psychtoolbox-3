function BrightSideCore(varargin)
% BrightSideCore -- Matlab MEX file for low-level interfacing with
% BrightSide technologies HDR display controller library.
%
% Don't call this file directly, but use the user friendly BrightSideHDR()
% command instead!!
%
% Low-Level command codes:
%
% BrightSideCore(0, configpath, configfile);
% - Initialize core, read config file 'configfile' located in path
% 'configpath'.
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
% BrightSideCore(5, clampingenabled);
% - Enable or disable clamping of colors in the OpenGL pipeline, depending
% if 'clampingenabled' is 1 or 0. 0 == Disable clamping is what one usually
% wants for drawing of HDR content.

% History:
% October 2006  Written by Mario Kleiner (MPIK) and Oguz Ahmet Akyuz - Dept. of
% Computer Science, University of Central Florida.
persistent alreadywarned;

if IsWin
    error('The BrightSideCore MEX file seems to be either invalid, not loadable or missing in your setup!');
else
    if isempty(alreadywarned)
        alreadywarned = 1;
        warning('BrightSide HDR support not available on non-Windows platforms. Will use (imperfect) emulation.');
    end    
end

return
