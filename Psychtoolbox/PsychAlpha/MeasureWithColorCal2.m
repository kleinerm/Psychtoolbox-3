function MeasureWithColorCal2(meterType)

    if nargin < 1 || isempty(meterType)
        meterType = 7;
    end

    PsychDefaultSetup(1);
    RestrictKeysForKbCheck([KbName('RightControl'), KbName('ESCAPE')]);

    % Initialize and if neccessary zero calibrate the colorimeter:
    CMCheckInit(meterType);

    while 1
        [~, keyCode] = KbStrokeWait(-1);
        if keyCode(KbName('ESCAPE'))
            break;
        end

        XYZ = MeasXYZ(meterType);
        v = XYZToxyY(XYZ);
        [cr, cy, L] = deal(v(1), v(2), v(3))
        fprintf('\n');
    end

    RestrictKeysForKbCheck([]);
    CMClose(meterType);
end
