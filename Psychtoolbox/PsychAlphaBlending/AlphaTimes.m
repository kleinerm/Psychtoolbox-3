function matProduct=AlphaTimes(mat, alpha)

% matProduct=AlphaTimes(mat1, alpha)
%
% Calculate the product of an image matrix and alpha as would Screen Alpha
% blending: Pointwise multiply and round.  Argument 'alpha' may
% be either a scaler or the alpha plane.   If an alpha plane, it must have
% the same x y dimensions of argument 'mat'.
%
% see also: AlphaSum, PsychAlphaBlending

% HISTORY
% 
% mm/dd/yy
% 
%  2/11/05  awi wrote it.



alphaDims=size(alpha);
matDims=size(mat);
alphaFraction=  alpha/255; 

%multiply, expanding alpha to the dimenstionality of mat for pointwise
%multiplication
if  length(alphaDims) == 2
    if alphaDims(1) * alphaDims(2)==1
        matProduct=mat * alphaFraction;
    else
        expandedAlphaPlane=repmat(alphaFraction,[1,1,matDims(3)]);
        matProduct=mat .* expandedAlphaPlane;
    end
elseif length(alphaDims) == 3
    matProduct=mat .* alphaFraction;
end

%matProduct=round(matProduct);






% alphaDims=size(alpha);
% matDims=size(mat);
% 
% %multiply, expanding alpha to the dimenstionality of mat for pointwise
% %multiplication
% if  length(alphaDims) == 2
%     if alphaDims(1) * alphaDims(2)==1
%         matProduct=mat * alpha;
%     else
%         expandedAlphaPlane=repmat(alpha,[1,1,matDims(3)]);
%         matProduct=mat .* expandedAlphaPlane;
%     end
% elseif length(alphaDims) == 3
%     matProduct=mat .* alpha;
% end
% 
% %matProduct=round(matProduct);
% matProduct=bitshift(matProduct,-8);


