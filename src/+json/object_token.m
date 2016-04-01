classdef object_token
    %
    %   Class:
    %   json.object_token
    
    properties
        name
        full_name
        index
        attribute_names
        attribute_indices
    end
    
    properties
        attribute_types
        attribute_sizes
    end
    
    properties (Hidden)
        map
        parse_object
    end
    
    methods
        function obj = object_token(name,full_name,index,parse_object)
            obj.name = name;
            obj.full_name = full_name;
            obj.index = index;
            obj.parse_object = parse_object;
            
            raw_string = parse_object.file_string;
            info = parse_object.info;
            
            n_attributes = info(4,index);
            cur_name_I = index+1;
            a_indices = zeros(1,n_attributes,'int32');
            a_names   = cell(1,n_attributes);
            
            
            
            
            %Value: integer
            %TODO: initialize map
            map = containers.Map;
            for iItem = 1:info(4,index)
                cur_value_I =  cur_name_I + 1;
                a_indices(iItem) = cur_value_I;
                temp_name = h__parse_string(raw_string,info,cur_name_I);
                map(temp_name) = iItem;
                a_names{iItem} = temp_name;
                cur_name_I = info(6,cur_value_I);
            end
            obj.map = map;
            obj.attribute_indices = a_indices;
            obj.attribute_names = a_names;
        end
        function output = get_token(obj,name)
           I = obj.map(name);
           full_name = [obj.full_name '.' name];
           index = obj.attribute_indices(I);
           type = obj.parse_object.info(1,index);
           output = json.array_token(name,full_name,index,obj.parse_object);
        end
    end
    
end

function output_string = h__parse_string(str,j,index)
%TODO: I'm thinking of keeping this in mex
%Currently
output_string = str(j(2,index):j(3,index));
end

