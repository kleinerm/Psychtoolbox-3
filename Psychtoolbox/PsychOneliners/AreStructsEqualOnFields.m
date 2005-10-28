function areEqual = AreStructsEqualOnFields(struct1,struct2,theFields)
% areEqual = AreStructsEqualOnFields(struct1,struct2,theFields)
%
% Returns 1 if two structs share the same value on each of the passed
% fields, 0 otherwise.
%
% Handles numeric and string field values

nFields = length(theFields);
areEqual = 0;

% Loop over fields.  Return on any indicator of non-equality.
% If we make it out the bottom, then set returne alue to 1,.
for i = 1:nFields
	% If either struct is missing the passed field, we say not equal.
	if (~isfield(struct1,theFields{i}) | ~isfield(struct2,theFields{i}))
		return;
	end

	% Snag the field
	field1 = getfield(struct1,theFields{i});
	field2 = getfield(struct2,theFields{i});
	
	% If fields aren't same type, forget it.
	if (ischar(field1) & ~ischar(field2))
		return;
	end
	if (ischar(field2) & ~ischar(field1))
		return;
	end

	% They are the same type
	if (ischar(field1))
		if (~streq(field1,field2))
			return;
		end
	else
		if (field1 ~= field2)
			return;
		end
	end
	
end

% If we arrive here, they are equal
areEqual = 1;
