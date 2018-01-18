function json_checker
%
%   json_tests.json_checker();
%
%   http://www.json.org/JSON_checker/

fprintf('Running json_tests.json_checker\n');

temp_path = fileparts(which('json_tests.json_checker'));
data_root = fullfile(temp_path,'data');

t = cell(33,2);
%This could be relaxed ... (would require changes to the parser)
t(1,:) = {'"A JSON payload should be an object or array, not a string."',0};


t(2,:) = {'["Unclosed array"',0};
t(3,:) = {'{unquoted_key: "keys must be quoted"}',0};
t(4,:) = {'["extra comma",]',0};
t(5,:) = {'["double extra comma",,]',0};
t(6,:) = {'[   , "<-- missing value"]',0};
t(7,:) = {'["Comma after the close"],',0};
t(8,:) = {'["Extra close"]]',0};
t(9,:) = {'"Extra comma": true,}',0};
t(10,:) = {'{"Extra value after close": true} "misplaced quoted value"',0};
t(11,:) = {'{"Illegal expression": 1 + 2}',0};
t(12,:) = {'{"Illegal invocation": alert()}',0};

%I need to look into this one ...
t(13,:) = {'{"Numbers cannot have leading zeroes": 013}',1};

t(14,:) = {'{"Numbers cannot be hex": 0x14}',0};
t(15,:) = {'["Illegal backslash escape: \x15"',0};
t(16,:) = {'[\naked]',0};
t(17,:) = {'["Illegal backslash escape: \017"]',0};

%Not really too deep ...
t(18,:) = {'[[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]]',1};

t(19,:) = {'{"Missing colon" null}',0};
t(20,:) = {'{"Double colon":: null}',0};
t(21,:) = {'{"Comma instead of colon", null}',0};
t(22,:) = {'["Colon instead of comma": false]',0};
t(23,:) = {'["Bad value", truth]',0}; %TODO: Let's do trueasdsf
t(24,:) = {'[''single quote'']',0};

%Apparently you need to escape control characters?
%Perhaps we can make this optional ????
t(25,:) = {'["	tab	character	in	string	"]',1};

t(26,:) = {'["tab\   character\   in\  string\  "]',0};

%Note the sprintf
%Eventually we could make a strict mode which checks for these strings
t(27,:) = {sprintf('["line\nbreak"]'),1};
t(28,:) = {sprintf('["line\\\nbreak"]'),0};

t(29,:) = {'[0e]',0};
t(30,:) = {'[0e+]',0};
t(31,:) = {'[0e+-1]',0};
t(32,:) = {'{"Comma instead if closing brace": true,',0};
t(33,:) = {'["mismatch"}',0};

fr = @(x)fileread(fullfile(data_root,x));
t(34,:) = {fr('pass1.json'),1};

%This doesn't look like it is being parsed correctly ...
t(35,:) = {fr('pass2.json'),1};


t(36,:) = {fr('pass3.json'),1};


n_tests = size(t,1);
for iTest = 1:n_tests
    %fprintf('-------------- JSON checker test: %d\n',iTest);
    cur_test_string = t{iTest,1};
    should_pass = t{iTest,2};
    passed = true;
    try
        jt = json.parse(cur_test_string);
        %fprintf('Test %d passed as expected\n',iTest);
    catch ME
        passed = false;
        if should_pass
            ME
            error_string = sprintf('Test #%d should have not thrown an error but did',iTest);
            error(error_string);
        end
        %fprintf('Test %d failed as expected with message:\n         %s\n',iTest,ME.message);
    end
    if passed && ~should_pass
        error_string = sprintf('Test #%d should have thrown an error but didn''t',iTest);
        error(error_string);
    else
        fprintf('#%d succeeded\n',iTest);
    end
    %fprintf('Clearing result\n');
    clear jt
end