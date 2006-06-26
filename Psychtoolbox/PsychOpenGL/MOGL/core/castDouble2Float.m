function rc = castDouble2Float(arg1, arg2)
% rc = castDouble2Float(arg1, arg2)
%
% This is a dummy function, just here as a placeholder for the
% castDouble2Float.oct file which is needed when running under Octave.
% castDouble2Float.m is needed on Matlab, so Matlab does not complain
% about non-existing functions.
if IsOctave
  error('M-File version of castDouble2Float called! castDouble2Float.oct missing!!!!');
else
  rc = arg1;
end;
