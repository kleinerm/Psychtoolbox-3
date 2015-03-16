function resultFlag = IsWayland
% resultFlag = IsWayland
%
% Returns true if the script is running on a Wayland display server.
%
% Some scripts need to behave differently when running under
% Wayland. The main advantage of Wayland is that visual presentation
% timing and timestamping should be accurate, robust and reliably
% under all conditions, ie., also for non-fullscreen "windowed" windows,
% half-transparent windows, GUI windows etc.

% History:
% 31-Dec-2014   mk Written.

persistent rc;

if isempty(rc)
  % If the built-in variable OCTAVE_VERSION exists,
  % then we are running under GNU/Octave, otherwise not.
  if ~isempty(getenv('WAYLAND_DISPLAY'))
    rc = 1;
  else
    rc = 0;
  end
end

resultFlag = rc;
return;
