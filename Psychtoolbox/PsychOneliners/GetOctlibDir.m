function octlibdir = GetOctlibDir
% GetOctlibDir -- Get path to Octave runtime libraries
%
% Need this wrapper, as Matlab can't parse M-Files which
% use __octave_config_info__ directly.
    octlibdir = __octave_config_info__.octlibdir;
end
