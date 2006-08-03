function areEqual = AreStructsEqualOnFields(struct1,struct2,theFields)
% areEqual = AreStructsEqualOnFields(struct1,struct2,theFields)
%
% Returns 1 if two structs share the same value on each of the passed
% fields, 0 otherwise.  Only checks numeric and string fields.
% For fields that are cell or struct, it does check that they are
% of same type, but doesn't descend and check the values.
%
% 5/1/05	dhb, jmk	Handle cell and struct fields, a little bit.

nFields = length(theFields);
areEqual = 0;

% Loop over fields.  Return on any indicator of non-equality.
% If we make it out the bottom, then set returne alue to 1,.
for i = 1:nFields
    % If either struct is missing the passed field, we say not equal.
    if (~isfield(struct1,theFields{i}) | ~isfield(struct2,theFields{i}))
        return;
    end

    % Snag the fields
    field1 = getfield(struct1,theFields{i});
    field2 = getfield(struct2,theFields{i});

    % If both fields are cell arrays, then we don't complain but
    % we don't check either.
    if (iscell(field1) & iscell(field2))

        % If both fields are structs, then we don't complain or check.
    elseif (isstruct(field1) & isstruct(field2))
    else

        % If either field is a cell array here, then one is and the
        % otheri isn't, so the structs are not equal
        if (iscell(field1) | iscell(field2))
            return;
        end

        % If either field is a struct here, then one is and the other
        % isn't, so the structs are not equal.
        if (isstruct(field1) | isstruct(field2))
            return;
        end

        % OK. Now we're down to numeric and string types.
        % If fields aren't same type, forget it.
        if (ischar(field1) & ~ischar(field2))
            return;
        end
        if (ischar(field2) & ~ischar(field1))
            return;
        end

        % They are the same type so check as appropriate.
        if (ischar(field1))
            if (~streq(field1,field2))
                return;
            end
        else
            if (any(field1 ~= field2))
                return;
            end
        end
    end
end

% If we arrive here, they are equal
areEqual = 1;
