function area = AreaUnderROC(ROC)
% area = AreaUnderROC(ROC)
%
% Compute the area under an ROC curve.
%
% xx/xx/xx  dhb  Wrote it.

[NFA,index] = sort(ROC(:,2));
NROC(:,1) = ROC(index,1);
NROC(:,2) = ROC(index,2);
[m,n] = size(NROC);

FROC = zeros(m+2,n);
FROC(1,:) = [0, 0];
FROC(m+2,:) = [1, 1];
FROC(2:m+1,:) = NROC;

area = 0;
totwidth = 0;
for i = 1:m+1
  meanhgt = (FROC(i,1)+FROC(i+1,1))/2;
  width = FROC(i+1,2)-FROC(i,2);
  area = area+meanhgt*width;
  totwidth = totwidth+width;
end



