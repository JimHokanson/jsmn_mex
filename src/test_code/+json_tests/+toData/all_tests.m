function all_tests()
%
%   json_tests.toData.all_tests
%
%   Tries to parse all example files

json_tests.toData.logical_array_tests();
json_tests.toData.numeric_array_tests();
json_tests.toData.object_tests();

json_tests.toData.functions.all_tests();

end