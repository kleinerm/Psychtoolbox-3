function PrintStruct(dataStruct, filePtr, indentChar, numIndentSpaces, numberFormat, nestLevel)
% function PrintStruct(dataStruct, [filePtr], [indentChar], [numIndentSpaces], [numberFormat])
% 
% Display the contents of struct arrays to arbitrary nesting depth.  By
% default PrintStruct prints to the MATLAB command window. If a file
% pointer argument is supplied then it prints the contents of the struct
% to a file.  
%
% Nested fields within the struct are indented numIndentSpaces for each level
% of nesting.  numIndenSpace is equal to one space.  The indentation character 
% by default is the space character.  
%
% Pass the empty vector, "[ ]" for either optional argument to specify
% the default.
%
% PrintStruct was suggested by Marialuisa Martelli as a convenient way to 
% display experiment parameters and results stored in struct. It's still 
% experimental.  The readability of the output could be improved by 
% including array indices in the output along with the struct field  names.
% Communicating that by using indentiation and spacing does not work well
% for complicated structures and large arrays.  
%  

% History
% 
% 05/03/03  awi Wrote it. 
% 07/02/04  awi Cosmetic changes to comments and added caveats at bottom.

% If the user invoked PrintStruct (instead of PrintStruct itself) then 
% print the name of the struct variable, fill in defaults and call yourself in this form:
% PrintStruct(dataStruct, filePtr, indentChar, numIndentSpaces, doubleFormat, nestLevel)
defaultNumberFormat='%f';
defaultFilePtr=1;
defaultIndentChar=' ';
defaultNumIndentSpaces=1;
defaultDoubleFormat='%f';
if (nargin <6)
	nestLevel=1;
	if nargin==5
		if isempty(doubleFormat)
			doubleFormat=defaultNumberFormat;
		end
	end
	if nargin <5 
		numberFormat=defaultNumberFormat;
	end
	if nargin >= 4 
		if isempty(numIndentSpaces)
			numIndentSpaces=defaultNumIndentSpaces;
		end
	end
	if nargin < 4 
		numIndentSpaces=defaultNumIndentSpaces;
	end
	if nargin >=3
		if and(isEmpty(indentChar), ~isChar(indentChar))
			indentChar=defaultIndentChar;
		end
	end
	if nargin < 3
		indentChar=defaultIndentChar;
	end
	if nargin >=2
		if isEmpty(filePtr)
			filePtr=defaultFilePtr;
		end
	end
	if nargin < 2
		filePtr=defaultFilePtr;
	end
	if(nargin < 1)
		error('dataStruct argument required but not provided')
	end
	dataStructName=inputname(1);
	fprintf(filePtr, '%s', dataStructName);
	fprintf(filePtr, '\n');
	PrintStruct(dataStruct, filePtr, indentChar, numIndentSpaces, numberFormat, nestLevel)
end %if


% recursively descend the structure, identifying each field type and dimensions and 
% printing

% recursion terminates when dataStruct is neither a cell array nor a struct. 
% if dataStruct is a double or a string, then display the contents of the string

	
% if this is a struct then loop over the dimensions and loop over the fields 
% indenting, printing the field name, and calling PrintStruct on the content of each field 
if strcmp('struct', class(dataStruct))
	numStructElements=prod(size(dataStruct));
	numStructDims= length(size(dataStruct));
	structDims= size(dataStruct);
	isStructVector=and(numStructDims==2, min(structDims) == 1);
	isStructSingleElement=max(structDims)==1;
	currentNestLevel=nestLevel;
	for ii=1:numStructElements;
		%the formatting  of the index depends on the dimensions of the struct matrix 
		if ~isStructSingleElement
			fprintf(filePtr, '%s', repmat(indentChar, 1, currentNestLevel*numIndentSpaces));
			currentNestLevel=nestLevel + 1; 
			if(isStructVector)
				structIndices=ii; 
				indexString=int2str(ii);
			else 
				structIndices=ReshapeIndex(vectorIndex, structDims);  
				indexString=sprintf([repmat('%d ', 1, length(structIndices) -1) '%d'], structIndices);
			end 
			fprintf(filePtr, '%s%s', 'STRUCT ARRAY INDEX: ', indexString);
			fprintf(filePtr, '\n');
		else
			structIndices=1;
		end
		%loop over the fields, printing the name of each and calling PrintStruct on the contents
		structFieldNames=fieldnames(dataStruct);
		numStructFieldNames=length(structFieldNames);
		for fi=1:numStructFieldNames;
			fprintf(filePtr, '%s', repmat(indentChar, 1, currentNestLevel*numIndentSpaces));
			fprintf(filePtr, '%s', structFieldNames{fi});
			fprintf(filePtr, '\n');
			PrintStruct(getfield(dataStruct, num2cell(structIndices),structFieldNames{fi}), filePtr, indentChar, numIndentSpaces, numberFormat, currentNestLevel+1);
		end
	end
end

% handles character vectors 
if strcmp('char', class(dataStruct)) 
	fprintf(filePtr, '%s', repmat(indentChar, 1, nestLevel*numIndentSpaces));
	fprintf(filePtr, '%s', dataStruct);
	fprintf(filePtr, '\n');
end

% handles matrices	
if strcmp('double', class(dataStruct))
	matDims= size(dataStruct);
	numMatDims=length(matDims);
	if numMatDims > 3
		error('PrintStruct does not know how do dislay matrices with more than 3 dimensions');
	end
	if numMatDims == 2
		matDims = [matDims 1];
	end
	for iZ=1:matDims(3)
		%begin plane
		for iY=1:matDims(1)
			%begin row
			fprintf(filePtr, '%s', repmat(indentChar, 1, nestLevel*numIndentSpaces));
			for iX=1:matDims(2)
				%begin element
				fprintf(filePtr, numberFormat, dataStruct(iX, iY, iZ));
				fprintf(filePtr, ' '); 
				%end element
		    end
			fprintf(filePtr, '\n');
			%end row
		end 
		fprintf(filePtr, '\n');
		%end plane
	end 
	%end matrix
end

% handle cell arrays
	
					
					
	
	

	
