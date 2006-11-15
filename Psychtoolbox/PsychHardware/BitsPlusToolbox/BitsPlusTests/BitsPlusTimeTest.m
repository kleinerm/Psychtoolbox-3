theClut = (0:1/255:1)' * ones(1, 3);
t = GetSecs;
row = BitsPlusEncodeClutRow(theClut);
t = GetSecs - t;
fprintf('Total time %.1f ms.\n', t*1000);
