function MeasureWithColorCal2(zeroCalibrate)

  PsychDefaultSetup(1);
  if nargin < 1 || isempty(zeroCalibrate) || zeroCalibrate > 0
    ColorCal2('ZeroCalibration');
  end

  cMatrix = ColorCal2('ReadColorMatrix')
  RestrictKeysForKbCheck(KbName('RightControl'));

  while 1
    KbStrokeWait;
    s = ColorCal2('MeasureXYZ');
    correctedValues = cMatrix(1:3,:) * [s.x s.y s.z]';
    v = XYZToxyY(correctedValues);
    [cr, cy, L] = deal(v(1), v(2), v(3))
    fprintf('\n');
  end
end
