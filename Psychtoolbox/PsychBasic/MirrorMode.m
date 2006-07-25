% MirrorMode -- Issues regarding mirror mode on dual display setups.
%
% If you run your system in a dual-display or multi-display configuration,
% you usually have the option of either running the displays as separate
% entities or in mirror-mode (aka clone mode). In mirror mode, all displays
% show the same visual content. They run at the same resolution and color
% depths.
%
% There are reasons pro- and against using mirror mode while running
% Psychtoolbox. You'll have to choose your trade-off.
%
% Good reason to use mirror mode: Better timing, higher performance
%
% In mirror mode, all physical displays are driven by the same framebuffer,
% the one controlled and occupied by the Psychtoolbox onscreen window.
% This means that Psychtoolbox is in full control of the graphics hardware,
% all VRAM video memory and all hardware ressources are devoted to visual
% stimulus generation and presentation. This allows to cache the maximum
% amount of textures in the onboard VRAM memory of the gfx card for maximum
% image drawing speed and most reliable timing, because all/most textures
% can be prefetched and kept resident in the fast VRAM, and even if PTB
% needs to fetch textures from system memory, it can do so at maximum speed,
% because the AGP or PCI-Express bus is allocted solely for Psychtoolbox
% needs. You can also run the highest display resolutions and Anti-Aliasing
% multisample levels in this configurations.
%
% Good reason to use desktop spanning mode or separate displays: Display of
% binocular stimuli on two different physical displays (binocular rivalry,
% stereo stimuli, ...): Still high performance and good timing.
%
% If you open a single stereo onscreen window in desktop spanning mode or
% two/multiple separate Psychtoolbox onscreen windows on separate non-mirrored
% displays, timing and performance will still be good, because Psychtoolbox
% is still in full exclusive control of the graphics hardware. This allows it
% to manage, and possibly share, the ressources between the different windows
% in an intelligent way, trying to preserve as much texture caching as possible.
%
% Bad reason to use non-mirror mode: Having one display with the Matlab window or
% user interface, the other for Psychtoolbox.
%
% The graphics hardware now has to maintain at least two separate framebuffers, one
% for each display. It also has to maintain different memory buffers for textures,
% 3D geometry and context state for each single application that is visible: At
% least one OpenGL context for the Matlab window, at least one context for the Aqua
% user interface, at least one context for each other visible application, at least
% one for Psychtoolbox. All these buffers take away precious VRAM memory that could
% be used by Psychtoolbox for more efficient stimulus drawing or image caching.
% Bus bandwidth needs to be shared between Psychtoolbox and all other applications,
% hardware computation time is splitted (and thereby taken away from PTB) between all
% running OpenGL applications. There is also significant overhead when multiple
% applications are simultaneously updating the display and the rendering engine has
% to switch between the different clients in a round-robin timesharing fashion.
% All in all this can significantly reduce drawing performance and introduce
% timing jitter in the order of multiple (dozens) milliseconds.
%
% The severity of this degradation and timing jitter depends strongly on the exact
% type of script run, the type and size of stimuli, what kind of applications are
% running in parallel and on the speed and amount of VRAM of your graphics hardware.
%
% Some studies may not be seriously affected, others will be seriously impaired.
%
% Another reason for not using mirror mode could be because you must! Some laptops
% seem to be incapable of properly running an external display with proper synchronization
% when switched to mirror-mode. In that case it may be still a good idea to open a
% onscreen window on the "unused" display, even if it does not show any stimulus. This
% way, Psychtoolbox at least keeps full control over the gfx-hardware, minimizing
% interference from other applications.
%
% Choose your poison, live is full of tradeoffs ;-)

% History:
% 17.6.2006 Written (MK).
