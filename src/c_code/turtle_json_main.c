#include "turtle_json.h"

//KEY PROCESSING
//---------------
//In order to improve processing we key TAC and parent information
//processing in the key's value. For simple values such as numbers or
//strings we have relatively little to do. For complex values (objects 
//and arrays) we log the key as a parent node which will have subsequent 
//children.
//
//number, string, null, true, false - store TAC, 
//array - store parent info
//
//When closing, we need to call different comma processors or closers.


// parent type - use pointers

//TODO: Allow string input to function

//TODO: store initial and final allocation sizes for each type
//TODO: Create method for creating scalar and saving into struct - for above TODO


//TODO: Build in if statements on keys


//DEBUGGING
#define ERROR_DEPTH goto S_ERROR_DEPTH;

#define PRINT_CURRENT_POSITION mexPrintf("Current Position: %d\n",CURRENT_INDEX);
#define PRINT_CURRENT_CHAR  mexPrintf("Current Char: %c\n",CURRENT_CHAR);

//=========================================================================
//              Data Allocation and index advancement
//=========================================================================

#define INCREMENT_DATA_INDEX \
    ++current_data_index; \
	if (current_data_index > data_size_index_max){ \
        ++n_data_allocations; \
        data_size_allocated = ceil(1.5*data_size_allocated); \
        data_size_index_max = data_size_allocated-1; \
        \
        types = mxRealloc(types,data_size_allocated); \
        d1 = mxRealloc(d1,data_size_allocated*sizeof(int)); \
        d2 = mxRealloc(d2,data_size_allocated*sizeof(int)); \
    }

//-----------------   Key Memory Management -------------------------------    
#define ALLOCATE_KEY_DATA \
    unsigned char **key_p = mxMalloc(key_size_allocated * sizeof(unsigned char *)); \
    int *key_sizes =  mxMalloc(key_size_allocated * sizeof(int));
            
#define INCREMENT_KEY_INDEX \
    ++current_key_index; \
    if (current_key_index > key_size_index_max) { \
        ++n_key_allocations; \
        key_size_allocated = ceil(1.5*key_size_allocated); \
        key_size_index_max = key_size_allocated - 1; \
        key_p = mxRealloc(key_p,key_size_allocated * sizeof(unsigned char *)); \
        key_sizes = mxRealloc(key_sizes,key_size_allocated * sizeof(int)); \
    }
    
#define TRUNCATE_KEY_DATA \
    key_p = mxRealloc(key_p,(current_key_index + 1)*sizeof(unsigned char *)); \
    key_sizes = mxRealloc(key_sizes,(current_key_index + 1) * sizeof(int));
    
//-----------------   String Memory Management ----------------------------    
#define ALLOCATE_STRING_DATA \
    unsigned char **string_p = mxMalloc(string_size_allocated * sizeof(unsigned char *)); \
    int *string_end_indices = mxMalloc(string_size_allocated * sizeof(int)); \
    int *string_start_indices = mxMalloc(string_size_allocated * sizeof(int));
    
#define INCREMENT_STRING_INDEX \
    ++current_string_index; \
    if (current_string_index > string_size_index_max) { \
        ++n_string_allocations; \
        string_size_allocated = ceil(1.5*string_size_allocated); \
        string_size_index_max = string_size_allocated - 1; \
        string_p = mxRealloc(string_p,string_size_allocated * sizeof(unsigned char *)); \
        string_end_indices = mxRealloc(string_end_indices,string_size_allocated * sizeof(int)); \
        string_start_indices = mxRealloc(string_start_indices,string_size_allocated * sizeof(int)); \
    }
    
#define TRUNCATE_STRING_DATA \
    string_p = mxRealloc(string_p,(current_string_index + 1)*sizeof(unsigned char *)); \
	string_end_indices = mxRealloc(string_end_indices,(current_string_index + 1) * sizeof(int)); \
 	string_start_indices = mxRealloc(string_start_indices,(current_string_index + 1) * sizeof(int));
    
//-----------------   Numeric Memory Management ---------------------------
#define ALLOCATE_NUMERIC_DATA unsigned char **numeric_p = mxMalloc(numeric_size_allocated * sizeof(unsigned char *));  

//TODO: This is named inconsistently vs strings and keys and data, choose a consistent name
#define EXPAND_NUMERIC_CHECK \
    ++current_numeric_index; \
    if (current_numeric_index > numeric_size_index_max) { \
        ++n_numeric_allocations; \
        numeric_size_allocated = ceil(1.5*numeric_size_allocated); \
        numeric_size_index_max = numeric_size_allocated - 1; \
        numeric_p = mxRealloc(numeric_p,numeric_size_allocated * sizeof(unsigned char *)); \
    }
    
#define TRUNCATE_NUMERIC_DATA numeric_p = mxRealloc(numeric_p,(current_numeric_index + 1)*sizeof(unsigned char *));


//Things for opening    =====    [ { :    =================================
#define INCREMENT_PARENT_SIZE parent_sizes[current_depth] += 1

#define SET_TYPE(x) types[current_data_index] = x;            
            
//TODO: The size isn't needed for keys - we could call two different macros ...
#define INITIALIZE_PARENT_INFO(x) \
        ++current_depth; \
        if (current_depth > 200){\
            goto S_ERROR_DEPTH_EXCEEDED; \
        }\
        parent_types[current_depth] = x; \
        parent_indices[current_depth] = current_data_index; \
        parent_sizes[current_depth] = 0;
            
//=========================================================================
//=================          Processing      ==============================
//=========================================================================
// The main code is essentially a state machine. States within the machine
// have the following rough layout:
//
//      1) key or array specific initialization code
//      2) common processing
//      3) navigation to the next state
//        
//  Code below is for #2, the processing that is commen to the type
//  regardless of whether or not it is in a key or array
        
#define PROCESS_OPENING_OBJECT \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_OBJECT); \
    INITIALIZE_PARENT_INFO(TYPE_OBJECT);
    
#define PROCESS_OPENING_ARRAY \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_ARRAY); \
    INITIALIZE_PARENT_INFO(TYPE_ARRAY); \

#define PROCESS_STRING \
    INCREMENT_STRING_INDEX; \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_STRING); \
    temp_p = CURRENT_POINTER; \
    /* +1 to point past the opening quote */ \
    string_p[current_string_index] = CURRENT_POINTER + 1; \
    string_start_indices[current_string_index] = n_string_chars; \
    /* + 1 for Matlab indexing */ \
    d1[current_data_index] = current_string_index + 1; \
    seek_string_end(CURRENT_POINTER,&CURRENT_POINTER); \
    n_string_chars += CURRENT_POINTER - string_p[current_string_index]; \
    string_end_indices[current_string_index] = n_string_chars;

#define PROCESS_KEY \
    INCREMENT_KEY_INDEX; \
    INCREMENT_DATA_INDEX; \
    /* This step is now done in some key values */ \
    /* //INITIALIZE_PARENT_INFO(TYPE_KEY); */ \
    SET_TYPE(TYPE_KEY); \
    /* We want to skip the opening quotes */ \
    key_p[current_key_index] = CURRENT_POINTER + 1; \
    /* Index into key arrays */ \
    d1[current_data_index]   = current_key_index + 1; \
    seek_string_end(CURRENT_POINTER,&CURRENT_POINTER); \
    /* We won't count the closing quote, but we would normally add 1 to */ \
    /* be inclusive on a count, so they cancel out */ \
    key_sizes[current_key_index] = CURRENT_POINTER - key_p[current_key_index]; \
    n_key_chars += key_sizes[current_key_index];
                
#define PROCESS_NUMBER \
    EXPAND_NUMERIC_CHECK; \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_NUMBER); \
    numeric_p[current_numeric_index] = CURRENT_POINTER; \
    /* Add 1 for Matlab indexing */ \
    d1[current_data_index] = current_numeric_index + 1; \
    string_to_double_no_math(CURRENT_POINTER, &CURRENT_POINTER);    
    
#define PROCESS_NULL \
    EXPAND_NUMERIC_CHECK; \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_NULL); \
    numeric_p[current_numeric_index] = 0; \
    d1[current_data_index] = current_numeric_index; \
    /*TODO: Add null check ... */ \
	ADVANCE_POINTER_BY_X(3)    
           
#define PROCESS_TRUE \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_TRUE); \
    /*TODO: Add true check ... */ \
	ADVANCE_POINTER_BY_X(3);
            
#define PROCESS_FALSE \
    INCREMENT_DATA_INDEX; \
    SET_TYPE(TYPE_FALSE); \
    /*TODO: Add false check ... */ \
	ADVANCE_POINTER_BY_X(4);
                
//==============      Things for closing      =============================
//+1 to next element
//+1 for Matlab 1 based indexing
#define STORE_TAC_OF_OBJECT_OR_ARRAY d2[current_parent_index] = current_data_index + 2;
    
//This is called before the simple value, so we need to advance to the simple
//value and then do the next value (i.e the token after close)
//Note that we're working with the current_data_index since we haven't
//advanced it yet and don't need to rely on a parent index (which hasn't even
//been set since the value is simple)
#define STORE_TAC_KEY_SIMPLE d2[current_data_index] = current_data_index + 3;  
    
#define STORE_TAC_KEY_COMPLEX d2[current_parent_index] = current_data_index + 2;    
            
#define STORE_SIZE d1[current_parent_index] = parent_sizes[current_depth];
            
#define MOVE_UP_PARENT_INDEX --current_depth;

#define IS_NULL_PARENT_INDEX current_depth == 0     
            
#define PARENT_TYPE parent_types[current_depth]            
//=========================================================================
  
//================      Navigation       ==================================
//=========================================================================      
#define CURRENT_CHAR *p
#define CURRENT_POINTER p
#define CURRENT_INDEX p - js
#define ADVANCE_POINTER_AND_GET_CHAR_VALUE *(++p)
#define DECREMENT_POINTER --p 
#define ADVANCE_POINTER_BY_X(x) p += x;
#define REF_OF_CURRENT_POINTER &p;
    
#define INIT_LOCAL_WS_CHARS \
    const __m128i whitespace_characters = _mm_set1_epi32(0x090A0D20);

//TODO: Describe what this does
#define ADVANCE_TO_NON_WHITESPACE_CHAR  \
    /* This might fail alot on newlines :/ */ \
    /* we might do an OR with \n => == ' ' OR = '\n' */ \
    if (*(++p) == ' '){ \
        ++p; \
    } \
    if (*p <= ' '){ \
        chars_to_search_for_ws = _mm_loadu_si128((__m128i*)p); \
        ws_search_result = _mm_cmpistri(whitespace_characters, chars_to_search_for_ws, SIMD_SEARCH_MODE); \
        p += ws_search_result; \
        if (ws_search_result == 16) { \
            while (ws_search_result == 16){ \
                chars_to_search_for_ws = _mm_loadu_si128((__m128i*)p); \
                ws_search_result = _mm_cmpistri(whitespace_characters, chars_to_search_for_ws, SIMD_SEARCH_MODE); \
                p += ws_search_result; \
            } \
        } \
    } \

            
#define DO_KEY_JUMP   goto *key_jump[CURRENT_CHAR]
     
#define DO_ARRAY_JUMP goto *array_jump[CURRENT_CHAR]
                
#define NAVIGATE_AFTER_OPENING_OBJECT \
	ADVANCE_TO_NON_WHITESPACE_CHAR; \
    switch (CURRENT_CHAR) { \
        case '"': \
            goto S_PARSE_KEY; \
        case '}': \
            goto S_CLOSE_OBJECT; \
        default: \
            goto S_ERROR_OPEN_OBJECT; \
    }            
                        
#define PROCESS_END_OF_ARRAY_VALUE \
	ADVANCE_TO_NON_WHITESPACE_CHAR; \
	switch (CURRENT_CHAR) { \
        case ',': \
            ADVANCE_TO_NON_WHITESPACE_CHAR; \
            DO_ARRAY_JUMP; \
        case ']': \
            goto S_CLOSE_ARRAY; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_ARRAY; \
	}
    
//This is for values following a key that are simple such as:
//number, string, null, fales, true    
    
#define PROCESS_END_OF_KEY_VALUE_SIMPLE \
    ADVANCE_TO_NON_WHITESPACE_CHAR; \
	switch (CURRENT_CHAR) { \
        case ',': \
            ADVANCE_TO_NON_WHITESPACE_CHAR; \
            if (CURRENT_CHAR == '"') { \
                goto S_PARSE_KEY; \
            } \
            else { \
                goto S_ERROR_BAD_TOKEN_FOLLOWING_OBJECT_VALUE_COMMA; \
            } \
        case '}': \
            goto S_CLOSE_OBJECT; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_KEY; \
	}          
            
#define PROCESS_END_OF_KEY_VALUE_COMPLEX \
    ADVANCE_TO_NON_WHITESPACE_CHAR; \
	switch (CURRENT_CHAR) { \
        case ',': \
            current_parent_index = parent_indices[current_depth]; \
            STORE_TAC_KEY_COMPLEX; \
            MOVE_UP_PARENT_INDEX; \
            ADVANCE_TO_NON_WHITESPACE_CHAR; \
            if (CURRENT_CHAR == '"') { \
                goto S_PARSE_KEY; \
            } \
            else { \
                goto S_ERROR_BAD_TOKEN_FOLLOWING_OBJECT_VALUE_COMMA; \
            } \
        case '}': \
            goto S_CLOSE_KEY_COMPLEX_AND_OBJECT; \
        default: \
            goto S_ERROR_END_OF_VALUE_IN_KEY; \
	}
    
#define NAVIGATE_AFTER_OPENING_ARRAY \
    ADVANCE_TO_NON_WHITESPACE_CHAR; \
    if (CURRENT_CHAR == ']'){ \
       goto S_CLOSE_ARRAY; \
    }else{ \
       DO_ARRAY_JUMP; \
    }
    
#define NAVIGATE_AFTER_CLOSING_COMPLEX \
	if (IS_NULL_PARENT_INDEX) { \
		goto S_PARSE_END_OF_FILE; \
	} else if (PARENT_TYPE == TYPE_KEY) { \
        PROCESS_END_OF_KEY_VALUE_COMPLEX; \
    } else { \
        PROCESS_END_OF_ARRAY_VALUE; \
    }    
    
//=========================================================================
const int SIMD_SEARCH_MODE = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_NEGATIVE_POLARITY | _SIDD_BIT_MASK;
__m128i chars_to_search_for_ws;
int ws_search_result;            

void setStructField(mxArray *s, void *pr, const char *fieldname, mxClassID classid, mwSize N)
{
    //This is a helper function for setting the field in the output struct
    //
    //This should be used for storing dynamically allocated memory
    //
    //  See Also
    //  --------
    //  setIntScalar
        
    mxArray *pm;
    
    pm = mxCreateNumericArray(0, 0, classid, mxREAL);
    mxSetData(pm, pr);
    mxSetM(pm, 1);
    mxSetN(pm, N);
    mxAddField(s,fieldname);
    mxSetField(s,0,fieldname,pm);
}

void setIntScalar(mxArray *s, const char *fieldname, int value){

    //This function allows us to hold onto integer scalars
    //We need to make an allocation to grab a value off the stack
    
    mxArray *pm;
    
    int *temp_value = mxMalloc(sizeof(double));
    
    *temp_value = value;
    
    pm = mxCreateNumericArray(0, 0, mxINT32_CLASS, mxREAL);
    mxSetData(pm, temp_value);
    mxSetM(pm, 1);
    mxSetN(pm, 1);
    mxAddField(s,fieldname);
    mxSetField(s,0,fieldname,pm);    
    
}

//-------------------------------------------------------------------------
void string_to_double_no_math(unsigned char *p, unsigned char **char_offset) {

    //In this approach we look for math like characters. We parse for
    //validity at a later point in time.
    
    //These are all the valid characters in a number
    //-+0123456789.eE
    const __m128i digit_characters = _mm_set_epi8('0','1','2','3','4','5','6','7','8','9','.','-','+','e','E','0');
    
    __m128i chars_to_search_for_digits;
    
    int digit_search_result;
    
    chars_to_search_for_digits = _mm_loadu_si128((__m128i*)p);
    digit_search_result = _mm_cmpistri(digit_characters, chars_to_search_for_digits, SIMD_SEARCH_MODE);
    p += digit_search_result;
    if (digit_search_result == 16){
        chars_to_search_for_digits = _mm_loadu_si128((__m128i*)p);
        digit_search_result = _mm_cmpistri(digit_characters, chars_to_search_for_digits, SIMD_SEARCH_MODE);
        p += digit_search_result;
        //At this point we've traversed 32 characters
        //This code is easily rewriteable if in reality we need more
        //TODO: I should explicitly describe the max here
        //####.#####E###
        if (digit_search_result == 16){
        	mexErrMsgIdAndTxt("jsmn_mex:too_long_math", "too many digits when parsing a number");
        }
    }
    
    *char_offset = p;    
}

//-------------------------------------------------------------------------
void seek_string_end(unsigned char *p, unsigned char **char_offset){

    //advance past initial double-quote character
    ++p;
    
STRING_SEEK:    
    //Old code - strchr apparently will check for null, but currently
    //we are padding to ensure we only need to look for '"'
    //p = strchr(p+1,'"');
    
    //TODO: We could try more complicated string instructions
    //Ideally we could have a switch on this for:
    //1) User options - which to use
    //2) Keys vs string values
    
    while (*p != '"'){
      ++p;    
    }
    
    //Back up to verify
    if (*(--p) == '\\'){
        //See documentation on the buffer we've added to the string
        if (*(--p) == 0){
            mexErrMsgIdAndTxt("turtle_json:unterminated_string", "JSON string is not terminated with a double-quote character");
        }
        //At this point, we either have a true end of the string, or we've
        //escaped the escape character
        //
        //for example:
        //1) "this is a test\"    => so we need to keep going
        //2) "testing\\"          => all done
        //
        //This of course could keep going ...
        
        //Adding on one last check to try and avoid the loop
        if (*p == '\\'){
            //Then we need to keep looking, we might have escaped this character
            //we'll go into a loop at this point
            //
            // This is true if the escape character is really an escape
            //character, rather than escaping the double quote
            bool double_quote_is_terminating = true;
            unsigned char *next_char = p + 3; 
            while (*(--p) == '\\'){
                double_quote_is_terminating = !double_quote_is_terminating;
            }
            if (double_quote_is_terminating){
               *char_offset = next_char-1; 
            }else{
                p = next_char;
                //mexPrintf("Char2: %c\n",*(p-2));
                goto STRING_SEEK;
            }
        }else{
            //   this_char   \     "     next_char
            //     p         1     2     3
            p+=3;
            //mexPrintf("Char1: %c\n",*(p-2));
            goto STRING_SEEK;
        }        
    }else{
        *char_offset = p+1;
    } 
}

//=========================================================================
//              Parse JSON   -    Parse JSON    -    Parse JSON
//=========================================================================
void parse_json(unsigned char *js, size_t string_byte_length, mxArray *plhs[],Options *options) {
    
    //TODO: Check string_byte_length - can't be zero ...
    
    //This apparently needs to be done locally for intrinsics ...
    INIT_LOCAL_WS_CHARS;
    
    //Note, this occurs after we've opened an array
    //or a comma. In the case of opening an array, we've already verified
    //that we aren't closing right away. We can't put this here as
    //it wouldn't be correct following a comma:
    //[] ok
    //["value",]
    const void *array_jump[256] = {
        [0 ... 33]  = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [34]        = &&S_PARSE_STRING_IN_ARRAY,            // "
        [35 ... 44] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [45]        = &&S_PARSE_NUMBER_IN_ARRAY,            // -
        [46 ... 47] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [48 ... 57] = &&S_PARSE_NUMBER_IN_ARRAY,            // #
        [58 ... 90] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [91]        = &&S_OPEN_ARRAY_IN_ARRAY,              // [
        [92 ... 101]  = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [102]         = &&S_PARSE_FALSE_IN_ARRAY,           // false
        [103 ... 109] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [110]         = &&S_PARSE_NULL_IN_ARRAY,            // null
        [111 ... 115] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [116]         = &&S_PARSE_TRUE_IN_ARRAY,            // true
        [117 ... 122] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY,
        [123]         = &&S_OPEN_OBJECT_IN_ARRAY,           // {
        [124 ... 255] = &&S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY};
        
    const void *key_jump[256] = {
        [0 ... 33]  = &&S_ERROR_TOKEN_AFTER_KEY,
        [34]        = &&S_PARSE_STRING_IN_KEY,      // "
        [35 ... 44] = &&S_ERROR_TOKEN_AFTER_KEY,
        [45]        = &&S_PARSE_NUMBER_IN_KEY,      // -
        [46 ... 47] = &&S_ERROR_TOKEN_AFTER_KEY,    
        [48 ... 57] = &&S_PARSE_NUMBER_IN_KEY,      // 0-9
        [58 ... 90] = &&S_ERROR_TOKEN_AFTER_KEY,
        [91]        = &&S_OPEN_ARRAY_IN_KEY,        // [
        [92 ... 101]  = &&S_ERROR_TOKEN_AFTER_KEY,
        [102]         = &&S_PARSE_FALSE_IN_KEY,   //false
        [103 ... 109] = &&S_ERROR_TOKEN_AFTER_KEY,
        [110]         = &&S_PARSE_NULL_IN_KEY,    // null
        [111 ... 115] = &&S_ERROR_TOKEN_AFTER_KEY,
        [116]         = &&S_PARSE_TRUE_IN_KEY,    // true
        [117 ... 122] = &&S_ERROR_TOKEN_AFTER_KEY,
        [123]         = &&S_OPEN_OBJECT_IN_KEY,   // {
        [124 ... 255] = &&S_ERROR_TOKEN_AFTER_KEY};        
    
    unsigned char *p = js;    
    unsigned char *temp_p;
    
    DEFINE_TIC(parsed_data_logging);
    
    //clock_t parsed_data_logging;
    
    //---------------------------------------------------------------------
    const int MAX_DEPTH = 200;
    int parent_types[201];
    //This needs to be indices instead of pointers because
    //we might resize (resize types, d1, d2) and the pointers would become 
    //invalid
    int parent_indices[201];
    int parent_sizes[201];
    int current_parent_index;
    int current_depth = 0;
    
    //---------------------------------------------------------------------
    int data_size_allocated;
    int n_data_allocations = 1;
    //TODO: Implement chars per token
    if (options->n_tokens){
        data_size_allocated = options->n_tokens;
    }else{
        data_size_allocated = ceil((double)string_byte_length/4);
    }
    int data_size_index_max = data_size_allocated - 1;
    int current_data_index = -1;
    
    //TODO: Move this to a macro next to the increment
    uint8_t *types = mxMalloc(data_size_allocated);            
    int *d1 = mxMalloc(data_size_allocated * sizeof(int));
    int *d2 = mxMalloc(data_size_allocated * sizeof(int));
    //---------------------------------------------------------------------
    
    int n_key_chars = 0;
    int n_key_allocations  = 1; //Not yet implemented
    
    int key_size_allocated;
    if (options->n_keys){
        key_size_allocated = options->n_keys;
    }else{
        key_size_allocated = ceil((double)string_byte_length/20);
    }
    int key_size_index_max = key_size_allocated-1;
    int current_key_index = -1;
    ALLOCATE_KEY_DATA 
    
    int n_string_chars = 0;
    int n_string_allocations = 1;
    int string_size_allocated;
    if (options->n_strings){
        string_size_allocated = options->n_strings;
    }else{
        string_size_allocated = ceil((double)string_byte_length/20);
    }
    int string_size_index_max = string_size_allocated-1;
    int current_string_index = -1;
    ALLOCATE_STRING_DATA
   
    int n_numeric_allocations = 1;
    int numeric_size_allocated;
    if (options->n_numbers){
        numeric_size_allocated = options->n_numbers;
    }else{
        numeric_size_allocated = ceil((double)string_byte_length/4);
    }
    int numeric_size_index_max = numeric_size_allocated - 1;
    int current_numeric_index = -1;
    ALLOCATE_NUMERIC_DATA
    
    
//Start of the parsing ====================================================
    DECREMENT_POINTER;
	ADVANCE_TO_NON_WHITESPACE_CHAR;

	switch (CURRENT_CHAR) {
        case '{':
        	PROCESS_OPENING_OBJECT;
            NAVIGATE_AFTER_OPENING_OBJECT;
        case '[':
            PROCESS_OPENING_ARRAY;
            NAVIGATE_AFTER_OPENING_ARRAY;
        default:
            mexErrMsgIdAndTxt("turtle_json:invalid_start", "Starting token needs to be an opening object or array");
	}

//    [ {            ======================================================
S_OPEN_OBJECT_IN_ARRAY:
    
    
    INCREMENT_PARENT_SIZE;
    PROCESS_OPENING_OBJECT;
    NAVIGATE_AFTER_OPENING_OBJECT;
    

//   "key": {        ====================================================== 
S_OPEN_OBJECT_IN_KEY:
    
    INITIALIZE_PARENT_INFO(TYPE_KEY);
    PROCESS_OPENING_OBJECT;
    NAVIGATE_AFTER_OPENING_OBJECT;
  
//=============================================================
S_CLOSE_KEY_COMPLEX_AND_OBJECT:    
    //Update tac and parent
        
    current_parent_index = parent_indices[current_depth];    
    STORE_TAC_KEY_COMPLEX;
    MOVE_UP_PARENT_INDEX;

    //Fall Through ------
S_CLOSE_OBJECT:
    
    current_parent_index = parent_indices[current_depth];
    STORE_TAC_OF_OBJECT_OR_ARRAY;
    STORE_SIZE;
    MOVE_UP_PARENT_INDEX;
    
    NAVIGATE_AFTER_CLOSING_COMPLEX
    
//=============================================================
S_OPEN_ARRAY_IN_ARRAY:
	
    //mexPrintf("Opening array in array: %d\n",CURRENT_INDEX);
    
    INCREMENT_PARENT_SIZE;
    PROCESS_OPENING_ARRAY;   
    NAVIGATE_AFTER_OPENING_ARRAY;
    
//=============================================================
S_OPEN_ARRAY_IN_KEY:
        
    //mexPrintf("Opening array in key: %d\n",CURRENT_INDEX);
    
    INITIALIZE_PARENT_INFO(TYPE_KEY);
    PROCESS_OPENING_ARRAY;
	NAVIGATE_AFTER_OPENING_ARRAY;
            
//=============================================================
S_CLOSE_ARRAY:
    
    current_parent_index = parent_indices[current_depth];
    STORE_TAC_OF_OBJECT_OR_ARRAY;
    STORE_SIZE;
    MOVE_UP_PARENT_INDEX;
    
    NAVIGATE_AFTER_CLOSING_COMPLEX;

//=============================================================
S_PARSE_KEY:
        
	INCREMENT_PARENT_SIZE;
    
    PROCESS_KEY;
    
    if (ADVANCE_POINTER_AND_GET_CHAR_VALUE == ':'){
        ADVANCE_TO_NON_WHITESPACE_CHAR;
        DO_KEY_JUMP;    
    }else{
        DECREMENT_POINTER;
        ADVANCE_TO_NON_WHITESPACE_CHAR;

        if (CURRENT_CHAR == ':') {
            ADVANCE_TO_NON_WHITESPACE_CHAR;
            DO_KEY_JUMP;
        }
        else {
            goto S_ERROR_MISSING_COLON_AFTER_KEY;
        }
    }

//=============================================================
S_PARSE_STRING_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    PROCESS_STRING
	PROCESS_END_OF_ARRAY_VALUE;

//=============================================================
S_PARSE_STRING_IN_KEY:

    STORE_TAC_KEY_SIMPLE;
    PROCESS_STRING;
	PROCESS_END_OF_KEY_VALUE_SIMPLE


//=============================================================
S_PARSE_NUMBER_IN_KEY:
    

    STORE_TAC_KEY_SIMPLE;

    PROCESS_NUMBER;
    
    //TODO: Check for a comma?
    DECREMENT_POINTER;
	PROCESS_END_OF_KEY_VALUE_SIMPLE;

//=============================================================
S_PARSE_NUMBER_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    PROCESS_NUMBER;
   
    //This normally happens, trying to optimize progression of #s in array
    if (CURRENT_CHAR == ','){
        ADVANCE_TO_NON_WHITESPACE_CHAR;
        DO_ARRAY_JUMP;
    }else{
        DECREMENT_POINTER;
        PROCESS_END_OF_ARRAY_VALUE;
    }

//=============================================================
S_PARSE_NULL_IN_KEY:
    
    STORE_TAC_KEY_SIMPLE;

    PROCESS_NULL;
    
	PROCESS_END_OF_KEY_VALUE_SIMPLE;

//=============================================================
S_PARSE_NULL_IN_ARRAY:

	INCREMENT_PARENT_SIZE;
    
    PROCESS_NULL
    
	PROCESS_END_OF_ARRAY_VALUE;

//=============================================================
S_PARSE_TRUE_IN_KEY:
    
    STORE_TAC_KEY_SIMPLE;
    
    PROCESS_TRUE
    
	PROCESS_END_OF_KEY_VALUE_SIMPLE


S_PARSE_TRUE_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    
    PROCESS_TRUE;
    
    PROCESS_END_OF_ARRAY_VALUE;

    
S_PARSE_FALSE_IN_KEY:
    
    STORE_TAC_KEY_SIMPLE;
    
    PROCESS_FALSE
    
    PROCESS_END_OF_KEY_VALUE_SIMPLE;

S_PARSE_FALSE_IN_ARRAY:
    
	INCREMENT_PARENT_SIZE;
    
    PROCESS_FALSE
    
	PROCESS_END_OF_ARRAY_VALUE;

	//=============================================================
S_PARSE_END_OF_FILE:
	ADVANCE_TO_NON_WHITESPACE_CHAR

		if (!(CURRENT_CHAR == '\0')) {
			mexErrMsgIdAndTxt("turtle_json:invalid_end", "non-whitespace characters found after end of root token close");
		}

	goto finish_main;


//===============       ERRORS   ==========================================
//=========================================================================
//TODO: This is going to be redone 
  
S_ERROR_BAD_TOKEN_FOLLOWING_OBJECT_VALUE_COMMA:
    // {"key": value, #ERROR
    //  e.g.
    // {"key": value, 1
    //
	mexPrintf("Position %d\n",CURRENT_INDEX); \
	mexErrMsgIdAndTxt("turtle_json:no_key", "Key or closing of object expected"); \
    
S_ERROR_DEPTH_EXCEEDED:
    mexErrMsgIdAndTxt("turtle_json:depth_exceeded", "Max depth was exceeded");
    
S_ERROR_OPEN_OBJECT:
	mexErrMsgIdAndTxt("turtle_json:invalid_token", "S_ERROR_OPEN_OBJECT");

S_ERROR_MISSING_COLON_AFTER_KEY:
	mexErrMsgIdAndTxt("turtle_json:invalid_token", "S_ERROR_MISSING_COLON_AFTER_KEY");

//TODO: Describe when this error is called    
S_ERROR_END_OF_VALUE_IN_KEY:
	mexErrMsgIdAndTxt("turtle_json:invalid_token", "Token of key must be followed by a comma or a closing object ""}"" character");

//This error comes when we have a comma in an array that is not followed
// by a valid value => i.e. #, ", [, {, etc.
S_ERROR_TOKEN_AFTER_COMMA_IN_ARRAY:
    mexPrintf("Current character: %c\n",CURRENT_CHAR);
    mexPrintf("Current position in string: %d\n",CURRENT_INDEX);
	mexErrMsgIdAndTxt("turtle_json:invalid_token", "Invalid token found after a comma in an array");
	//mexErrMsgIdAndTxt("turtle_json:no_primitive","Primitive value was not found after the comma");
   
//TODO: Open array points here now too
S_ERROR_TOKEN_AFTER_KEY:
	mexErrMsgIdAndTxt("turtle_json:invalid_token", "S_ERROR_TOKEN_AFTER_KEY");
	//mexErrMsgIdAndTxt("turtle_json:no_primitive","Primitive value was not found after the comma");    
    

S_ERROR_END_OF_VALUE_IN_ARRAY:  
    //TODO: Print the character
	mexPrintf("Current position: %d\n", CURRENT_INDEX);
	mexErrMsgIdAndTxt("turtle_json:invalid_token", "Token in array must be followed by a comma or a closing array ""]"" character ");    

S_ERROR_DEPTH:
    mexErrMsgIdAndTxt("turtle_json:depth_limit","Max depth exceeded");
    
S_ERROR_DEBUG:
    mexErrMsgIdAndTxt("turtle_json:debug_error","Debug error");
    
finish_main:
    
    //The end ...
    //parsed_data_logging = clock();
    
    START_TIC(parsed_data_logging);
    
    //This wasn't working for some reason ...     
    //TIC(parsed_data_logging);
    types = mxRealloc(types,(current_data_index + 1));
    setStructField(plhs[0],types,"types",mxUINT8_CLASS,current_data_index + 1);
    
    d1 = mxRealloc(d1,((current_data_index + 1)*sizeof(int)));
    setStructField(plhs[0],d1,"d1",mxINT32_CLASS,current_data_index + 1);
    
    d2 = mxRealloc(d2,((current_data_index + 1)*sizeof(int)));
    setStructField(plhs[0],d2,"d2",mxINT32_CLASS,current_data_index + 1);
     
    //Meta data storage
    //--------------------
    setIntScalar(plhs[0],"n_key_chars",n_key_chars);
    setIntScalar(plhs[0],"n_string_chars",n_string_chars);
    //This information can be used to tell how efficient we were
    //relative to the allocation
    setIntScalar(plhs[0],"n_tokens_allocated",data_size_allocated);
    setIntScalar(plhs[0],"n_key_allocations",key_size_allocated);
    setIntScalar(plhs[0],"n_string_allocations",string_size_allocated);
    setIntScalar(plhs[0],"n_numeric_allocations",numeric_size_allocated);
    
    
    //TODO: This is only correct on 64 bit systems ...
    TRUNCATE_KEY_DATA
    setStructField(plhs[0],key_p,"key_p",mxUINT64_CLASS,current_key_index + 1);
    setStructField(plhs[0],key_sizes,"key_sizes",mxINT32_CLASS,current_key_index + 1);
    
    TRUNCATE_STRING_DATA
    setStructField(plhs[0],string_p,"string_p",mxUINT64_CLASS,current_string_index + 1);
    setStructField(plhs[0],string_end_indices,"string_end_indices",mxINT32_CLASS,current_string_index + 1);
    setStructField(plhs[0],string_start_indices,"string_start_indices",mxINT32_CLASS,current_string_index + 1);
    
    TRUNCATE_NUMERIC_DATA
    //Note, it seems the class type may only be needed for viewing in Matlab
    //Internally it is just bytes (assuming sizeof is the same)
    setStructField(plhs[0],numeric_p,"numeric_p",mxDOUBLE_CLASS,current_numeric_index + 1);

    TOC_AND_LOG(parsed_data_logging,parsed_data_logging_time);

	return;
    
}