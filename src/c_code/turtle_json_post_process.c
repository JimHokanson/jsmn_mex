#include "turtle_json.h"

#define IS_CONTINUATION_BYTE *p >> 6 == 0b10

//Issues
//------

//This code is not curently being used, but may at some point be used
//when optimizing the code
//
//http://stackoverflow.com/questions/18847833/is-it-possible-return-cell-array-that-contains-one-instance-in-several-cells
//--------------------------------------------------------------------------
struct mxArray_Tag_Partial {
    void *name_or_CrossLinkReverse;
    mxClassID ClassID;
    int VariableType;
    mxArray *CrossLink;
    size_t ndim;
    unsigned int RefCount; /* Number of sub-elements identical to this one */
};

mxArray *mxCreateReference(const mxArray *mx)
{
    struct mxArray_Tag_Partial *my = (struct mxArray_Tag_Partial *) mx;
    ++my->RefCount;
    return (mxArray *) mx;
}
//-------------------------------------------------------------------------


//Values for Integer portion of number
//------------------------------------
//Note that I'm avoiding subtracting 0 which makes these arrays rather large ...
//TODO: test time difference on subtracting
//
//This code allows us to do double_value = ple#[current_character]
//
//Subtracting 0 would mean we would do double_value = ple#[current_character - '0']
const double p1e0[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9};
const double p1e1[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,30,40,50,60,70,80,90};
const double p1e2[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,200,300,400,500,600,700,800,900};
const double p1e3[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000,2000,3000,4000,5000,6000,7000,8000,9000};
const double p1e4[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000,20000,30000,40000,50000,60000,70000,80000,90000};
const double p1e5[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000,200000,300000,400000,500000,600000,700000,800000,900000};
const double p1e6[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000,2000000,3000000,4000000,5000000,6000000,7000000,8000000,9000000};
const double p1e7[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000,20000000,30000000,40000000,50000000,60000000,70000000,80000000,90000000};
const double p1e8[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000,200000000,300000000,400000000,500000000,600000000,700000000,800000000,900000000};
const double p1e9[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000,2000000000,3000000000,4000000000,5000000000,6000000000,7000000000,8000000000,9000000000};
const double p1e10[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000000,20000000000,30000000000,40000000000,50000000000,60000000000,70000000000,80000000000,90000000000};
const double p1e11[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000000,200000000000,300000000000,400000000000,500000000000,600000000000,700000000000,800000000000,900000000000};
const double p1e12[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000000,2000000000000,3000000000000,4000000000000,5000000000000,6000000000000,7000000000000,8000000000000,9000000000000};
const double p1e13[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10000000000000,20000000000000,30000000000000,40000000000000,50000000000000,60000000000000,70000000000000,80000000000000,90000000000000};
const double p1e14[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100000000000000,200000000000000,300000000000000,400000000000000,500000000000000,600000000000000,700000000000000,800000000000000,900000000000000};
const double p1e15[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1000000000000000,2000000000000000,3000000000000000,4000000000000000,5000000000000000,6000000000000000,7000000000000000,8000000000000000,9000000000000000};

//Values for fractional component
//-------------------------------
//Note, it is unclear how accurate we can really be with this ...
//and accordingly, how far out we should really populate ...
//
//I've gone out to the maximum of double (I think)
//
//i.e. 2.0123456789012345 ?
const double p1e_1[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
const double p1e_2[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09};
const double p1e_3[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.001,0.002,0.003,0.004,0.005,0.006,0.007,0.008,0.009};
const double p1e_4[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0001,0.0002,0.0003,0.0004,0.0005,0.0006,0.0007,0.0008,0.0009};
const double p1e_5[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00001,0.00002,0.00003,0.00004,0.00005,0.00006,0.00007,0.00008,0.00009};
const double p1e_6[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000001,0.000002,0.000003,0.000004,0.000005,0.000006,0.000007,0.000008,0.000009};
const double p1e_7[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000001,0.0000002,0.0000003,0.0000004,0.0000005,0.0000006,0.0000007,0.0000008,0.0000009};
const double p1e_8[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000001,0.00000002,0.00000003,0.00000004,0.00000005,0.00000006,0.00000007,0.00000008,0.00000009};
const double p1e_9[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000001,0.000000002,0.000000003,0.000000004,0.000000005,0.000000006,0.000000007,0.000000008,0.000000009};
const double p1e_10[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000001,0.0000000002,0.0000000003,0.0000000004,0.0000000005,0.0000000006,0.0000000007,0.0000000008,0.0000000009};
const double p1e_11[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000001,0.00000000002,0.00000000003,0.00000000004,0.00000000005,0.00000000006,0.00000000007,0.00000000008,0.00000000009};
const double p1e_12[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000001,0.000000000002,0.000000000003,0.000000000004,0.000000000005,0.000000000006,0.000000000007,0.000000000008,0.000000000009};
const double p1e_13[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000001,0.0000000000002,0.0000000000003,0.0000000000004,0.0000000000005,0.0000000000006,0.0000000000007,0.0000000000008,0.0000000000009};
const double p1e_14[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000001,0.00000000000002,0.00000000000003,0.00000000000004,0.00000000000005,0.00000000000006,0.00000000000007,0.00000000000008,0.00000000000009};
const double p1e_15[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000001,0.000000000000002,0.000000000000003,0.000000000000004,0.000000000000005,0.000000000000006,0.000000000000007,0.000000000000008,0.000000000000009};
const double p1e_16[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000001,0.0000000000000002,0.0000000000000003,0.0000000000000004,0.0000000000000005,0.0000000000000006,0.0000000000000007,0.0000000000000008,0.0000000000000009};
const double p1e_17[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000001,0.00000000000000002,0.00000000000000003,0.00000000000000004,0.00000000000000005,0.00000000000000006,0.00000000000000007,0.00000000000000008,0.00000000000000009};
const double p1e_18[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000000000000001,0.000000000000000002,0.000000000000000003,0.000000000000000004,0.000000000000000005,0.000000000000000006,0.000000000000000007,0.000000000000000008,0.000000000000000009};
const double p1e_19[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.0000000000000000001,0.0000000000000000002,0.0000000000000000003,0.0000000000000000004,0.0000000000000000005,0.0000000000000000006,0.0000000000000000007,0.0000000000000000008,0.0000000000000000009};
const double p1e_20[58] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00000000000000000001,0.00000000000000000002,0.00000000000000000003,0.00000000000000000004,0.00000000000000000005,0.00000000000000000006,0.00000000000000000007,0.00000000000000000008,0.00000000000000000009};

//*p++                  js[parser_position++]
//*p   ==========       js[parser_position]
//++p  ==========       ++parser_position

void string_to_double(double *value_p, char *p, int i, int *error_p, int *error_value) {
    
    /*
     *  I found atof to be the main bottleneck in this code. It was
     *  suggested to me by some internet searching that a custom algorithm
     *  would be close enough in 99.99 of the cases. I found a couple
     *  basic ones online that used loops and multiplation. This was my
     *  attempt to make something as fast possible. I welcome a faster
     *  approach!
     *
     *  I think that it would be worthwhile to investigate trying
     *  to populate integer and fraction arrays and then add them together
     *  using intrinsics
     */
    
    double value = 0;
    double exponent_value;
    bool negate;
    int64_t n_numeric_chars_plus_1;
    char *number_start;
    
    if (*p == '-'){
        ++p;
        negate = true;
    }else{
        negate = false;
    }
    
    number_start = p;
    while (isdigit(*p++)) {}
    
    n_numeric_chars_plus_1 = p-number_start; //# of numeric characters (off by 1 due to ++)
    
    //reset p so that we can increment our way along the number
    p = number_start;
    //Now that we know how many #s we have (off by 1 due to ++)
    //we can start at the beginning and add each to its correct place
    //e.g:
    //  4086 =
    //  4000 +
    //   0   +
    //    80 +
    //     6
    
    //I'm curious if you could take advantage of something like SSE
    //if each case was written out explicitly rather than via fall through
    switch (n_numeric_chars_plus_1) {
        case 17:
            value += p1e15[*p++];
        case 16:
            value += p1e14[*p++];
        case 15:
            value += p1e13[*p++];
        case 14:
            value += p1e12[*p++];
        case 13:
            value += p1e11[*p++];
        case 12:
            value += p1e10[*p++];
        case 11:
            value += p1e9[*p++];
        case 10:
            value += p1e8[*p++];
        case 9:
            value += p1e7[*p++];
        case 8:
            value += p1e6[*p++];
        case 7:
            value += p1e5[*p++];
        case 6:
            value += p1e4[*p++];
        case 5:
            value += p1e3[*p++];
        case 4:
            value += p1e2[*p++];
        case 3:
            value += p1e1[*p++]; //1e1 == 10, 2 #s plus the off by 1
        case 2:
            value += p1e0[*p++]; //1e0 == 1, an unfortunate mismatch of exponent and scalar
            break;
        case 1:
            
            *error_p = i+1;
            *error_value = 0;
            return;
            //TODO: This will likely cause a problem since we have multiple threads
            //mexErrMsgIdAndTxt("turtle_json:no_number","No numbers were found, at position: %d");
        default:
            *error_p = i+1;
            *error_value = 1;
            return;
            //mexErrMsgIdAndTxt("turtle_json:too_many_integers","The integer component of the number had too many digits");
    }
    
    //This would be written neater as a while loop that updates a pointer
    //to the various arrays but I don't think it would be as fast
    //
    //Unlike the integer part, we know how to interpret each
    //numeric value as we are parsing it
    //
    //0.1234 =
    //0.1 +
    //0.02 +
    //0.003 +
    //0.0004
    
    if (*p == '.') {
        ++p;
        //TODO: Is no digit ok? => no, this needs to be handled
        if(isdigit(*p)){
            value += p1e_1[*p++];
            if(isdigit(*p)){
                value += p1e_2[*p++];
                if(isdigit(*p)){
                    value += p1e_3[*p++];
                    if(isdigit(*p)){
                        value += p1e_4[*p++];
                        if(isdigit(*p)){
                            value += p1e_5[*p++];
                            if(isdigit(*p)){
                                value += p1e_6[*p++];
                                if(isdigit(*p)){
                                    value += p1e_7[*p++];
                                    if(isdigit(*p)){
                                        value += p1e_8[*p++];
                                        if(isdigit(*p)){
                                            value += p1e_9[*p++];
                                            if(isdigit(*p)){
                                                value += p1e_10[*p++];
                                                if(isdigit(*p)){
                                                    value += p1e_11[*p++];
                                                    if(isdigit(*p)){
                                                        value += p1e_12[*p++];
                                                        if(isdigit(*p)){
                                                            value += p1e_13[*p++];
                                                            if(isdigit(*p)){
                                                                value += p1e_14[*p++];
                                                                if(isdigit(*p)){
                                                                    value += p1e_15[*p++];
                                                                    if(isdigit(*p)){
                                                                        value += p1e_16[*p++];
                                                                        if(isdigit(*p)){
                                                                            value += p1e_17[*p++];
                                                                            if(isdigit(*p)){
                                                                                value += p1e_18[*p++];
                                                                                if(isdigit(*p)){
                                                                                    value += p1e_19[*p++];
                                                                                    if(isdigit(*p)){
                                                                                        value += p1e_20[*p++];
                                                                                        if(isdigit(*p)){
                                                                                            *error_p = i+1;
                                                                                            *error_value = 3;
                                                                                            return;
                                                                                            //mexErrMsgIdAndTxt("turtle_json:too_many_decimals","The fractional component of the number had too many digits");
                                                                                            
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }else{
            //No number following the period
            *error_p = i+1;
            *error_value = 2;
            return;
        }
    }
    //End of if '.'
    if (negate) {
        value = -value;
    }
    
    if (*p == 'E' || *p == 'e') {
        ++p;
        switch (*p){
            case '-':
                ++p;
                negate = true;
                break;
            case '+':
                ++p;
            default:
                negate = false;
        }
        
        number_start = p;
        while (isdigit(*p++)) {}
        
        exponent_value = 0;
        n_numeric_chars_plus_1 = p-number_start;
        //reset p so that we can increment our way down
        p = number_start;
        switch (n_numeric_chars_plus_1) {
            case 4:
                exponent_value += p1e2[*p++];
            case 3:
                exponent_value += p1e1[*p++];
            case 2:
                exponent_value += p1e0[*p++];
                break;
            case 1:
                mexErrMsgIdAndTxt("turtle_json:empty_exponent","An exponent was given with no numeric value");
            default:
                //TODO: Give error location in string
                mexErrMsgIdAndTxt("turtle_json:large_exponent","There were more than 3 digits in a numeric exponent");
        }
        if (negate){
            exponent_value = -exponent_value;
        }
        value *= pow(10.0, exponent_value);
    }
    
    //TODO: I need to check for invalid values at the end of the string
    //With our string skipping we skip all parts of the number so
    //something like this would get by:
    //  1.2345E123E
    //  as would
    //  1.2345.123432
    //
    //  valid characters at the end would be:
    //  space } ] , 
    //  comma is probably the most likely
    
    *value_p = value;
}

void parse_numbers(unsigned char *js,mxArray *plhs[]) {
    //
    //  numeric_p - this array starts as a set of pointers
    //  to locations in the json_string that contain numbers.
    //  For example, we might have numeric_p[0] point to the following
    //  location:
    //
    //      {"my_value": 1.2345}
    //                   ^   
    //
    //  Some of these pointers may be null, indicating that a "null"
    //  JSON value occurred at that index in the array.
    //
    //  I am currently assuming that a pointer is 64 bits, which means
    //  that I recycle the memory to store the array of doubles
    
    TIC(number_parse);
    
    mxArray *temp = mxGetField(plhs[0],0,"numeric_p");
    
    //Casting for input handling
    unsigned char **numeric_p = (unsigned char **)mxGetData(temp);
    //Casting for output handling (recycling of memory)
    double *numeric_p_double = (double *)mxGetData(temp);
    
    //Note, we are making an assumption here about the max size
    //of doubles
    int n_numbers = mxGetN(temp);
    
    int *error_locations;
    int *error_values;
    
    int n_threads = omp_get_max_threads();
    
    // Use omp_get_num_threads instead?????
    error_locations = mxMalloc(n_threads*sizeof(int));
    error_values    = mxMalloc(n_threads*sizeof(int));
    
    const double MX_NAN = mxGetNaN();
    
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        
        int error_location = 0;
        int error_value;

        #pragma omp for
        for (int i = 0; i < n_numbers; i++){
            if (numeric_p[i]){
                string_to_double(&numeric_p_double[i],numeric_p[i],i,&error_location,&error_value);
            }else{
                numeric_p_double[i] = MX_NAN;
            }
        }  
        
        *(error_locations + tid) = error_location;
        *(error_values + tid) = error_value;
        
    }
    
    //Error processing
    //--------------------------------------
    for (int i = 0; i < n_threads; i++){
        if (*error_locations){
            int error_index = *error_locations - 1;
            //Note that we hold onto the pointer in cases of an error
            //It is not overidden with a double
            unsigned char *first_char_of_bad_number = numeric_p[error_index];
            // p - js
            //
            // numeric_p[**=
            
            //TODO: This is a bit confusing since this pointer doesn't
            //move but the other one does ...
            //TODO: Ideally we would pass these error messages into
            //a handler that would handle the last bit of formatting
            //and also provide context in the string
            //We would need the string length ...
            switch (*(error_values + i))
            {
                case 0:
                    //I don't think this can run based on how our parser works ...
                    //TODO: Change this to a code error?
                    mexErrMsgIdAndTxt("turtle_json:no_integer_component", \
                            "No integer component was found for a number (#%d in the file, at position %d)", \
                            error_index+1,first_char_of_bad_number-js+1);
                    break;
                case 1:
                    mexErrMsgIdAndTxt("turtle_json:integer_component_too_large",
                            "The integer component of the number had too many digits (#%d in the file, at position %d)",
                            error_index+1,first_char_of_bad_number-js+1);
                    break;
                case 2:
                    mexErrMsgIdAndTxt("turtle_json:no_fractional_numbers","A number had a period, followed by no numbers (#%d in the file, at position %d)",error_index+1,first_char_of_bad_number-js+1);
                case 3:
                    mexErrMsgIdAndTxt("turtle_json:fractional_component_too_large","The fractional component of a number had too many digits");
                case 4:
                    mexErrMsgIdAndTxt("turtle_json:no_exponent_numbers","A number had an exponent symbol (e or E) followed by no digits");
                case 5:
                    mexErrMsgIdAndTxt("turtle_json:exponent_component_too_large","The fractional component of the number had too many digits");
                default:
                    mexErrMsgIdAndTxt("turtle_json:internal_code_error","Internal code error");   
            }
        }
        ++error_locations;
    }
    
    TOC_AND_LOG(number_parse,number_parsing_time);
    
}

void all_key_check(unsigned char *js,mxArray *plhs[]){
    
//Same object has:
//1) Same # of children
//2) Could keep track of # of characters - this could be post processed
//3) Each string has the same length
//4) final comparison of the actual strings - this could be done in parallel
//with a potential cleanup of those that turned out not to be the same
//Note, this could get messy as we run into comparing many to many
//
//Do we do this based on the same parent or the same depth?
//
//Same parent may not be optimum, but it probably is sufficient
//- on not being the same, we could fall back to some alternative strategy
//Parents would need to be post processed
//
//[ {'a':1,'b':2},{'a':1,'b':2}]
//  o1            o2              <= both objects are the same, create a struct array
//
//[ {'a':1,'b':2},{'a':1,'b':2},{'a':1,'b':2,'c':3}]
//  o1            o2            o3      <= objects are not the same cell array of structs
    
    
}


void parse_char_data(unsigned char *js,mxArray *plhs[], bool is_key){
    //
    //  Parses string or key characters into Matlab strings
    //  
    //  This includes:
    //  1) Initializaton of cell arrays and string Matlab objects
    //  2) Processing of character escapes => \n to newline
    //  3) Unicode escapes
    //
    //
    
    const uint16_t escape_values[256] = {
        [34] = '"',
        [47] = '/',
        [92] = '\\',
        [98] = '\b',
        [102] = '\f',
        [110] = '\n',
        [114] = '\r',
        [116] = '\t'};
        

    //Input character
    //Output, numerical value to add, unless invalid then -1
    //e.g. a => 10
    //     C => 12
    const int hex_numerical_values[256] = {
        [0 ... 47] = -1,
        [48] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // '0 ... 9'
        [58 ... 64] = -1,
        [65] = 10, 11, 12, 13, 14, 15,      // 'A - F'
        [71 ... 96] = -1,
        [97] = 10, 11, 12, 13, 14, 15, 
        [103 ... 255] = -1};

        
    
    mxArray *temp;
    unsigned char **char_p;
    int n_entries;
    int *start_indices;
    int *end_indices;
    
    //These need to be outside the if statement, as they include declarations
    
    
    if (is_key){
        temp = mxGetField(plhs[0],0,"key_p");
        
        char_p = (unsigned char **)mxGetData(temp);
        n_entries = mxGetN(temp);
    
        temp = mxGetField(plhs[0],0,"key_start_indices");
        start_indices = (int *)mxGetData(temp);
    
        temp = mxGetField(plhs[0],0,"key_end_indices");
        end_indices = (int *)mxGetData(temp);
        
    }else{
        temp = mxGetField(plhs[0],0,"string_p");
        
        char_p = (unsigned char **)mxGetData(temp);
        n_entries = mxGetN(temp);
    
        temp = mxGetField(plhs[0],0,"string_start_indices");
        start_indices = (int *)mxGetData(temp);
    
        temp = mxGetField(plhs[0],0,"string_end_indices");
        end_indices = (int *)mxGetData(temp);
    }
    
    TIC(string_memory_allocation);
    //Initial allocation of memory
    //------------------------------------------------------
    int n_chars_max_in_string;
    mxArray *cell_array = mxCreateCellMatrix(1,n_entries);
    mxArray *temp_mx_array;
    uint16_t *cell_data;
    uint16_t **all_cells_data = mxMalloc(n_entries*sizeof(cell_data));
    
    //TODO: Implement matching of strings to preallocated strings
    //and use reference count updating method
    //http://stackoverflow.com/questions/18847833/is-it-possible-return-cell-array-that-contains-one-instance-in-several-cells
    for (int i = 0; i < n_entries; i++){
        
        n_chars_max_in_string = end_indices[i] - start_indices[i];
        
        //String initialization
        //--------------------------------------
        //mxCreateUninitNumericMatrix is an option but it is
        //relatively new so we'll avoid it(2015a or newer :/)
        temp_mx_array = mxCreateNumericMatrix(1,0,mxCHAR_CLASS,mxREAL);
        cell_data = mxMalloc(n_chars_max_in_string*2);
        //This approach avoids initialization of the data
        mxSetData(temp_mx_array,cell_data);
        mxSetN(temp_mx_array,n_chars_max_in_string);
        
        //Hold onto for populating the actual data (next loop)
        all_cells_data[i] = cell_data;
        
        //Put the string in the cell array
        mxSetCell(cell_array,i,temp_mx_array);
        
    } 
    
    if (is_key){
            //TOC_AND_LOG(key_parse,key_parsing_time);
            //mxAddField(plhs[0],"keys");
            //mxSetField(plhs[0],0,"keys",cell_array);     
    }else{
        TOC_AND_LOG(string_memory_allocation,string_memory_allocation_time);
//             mxAddField(plhs[0],"strings");
//             mxSetField(plhs[0],0,"strings",cell_array);        
    }
    
    
    TIC(key_parse);
    TIC(string_parse);
    
    //Parsing of the string into proper UTF-8
    //---------------------------------------------------------------------
    unsigned char *p;
    int cur_index;
    bool shrink_string; //Set true when we reduce the # of characters in 
    //the string due to escapes or UTF-8 conversion
    uint16_t *output_data;
    int parse_status;
    int hex_numerical_value; //TODO: This could be uint8_t
    uint16_t unicode_char_value;
    uint32_t utf8_value;
    int escape_value;
    int hex_multipliers[4] = {4096, 256, 16, 1};
    uint16_t escape_char;
    
    for (int i = 0; i < n_entries; i++){
        
        p = char_p[i];
        output_data = all_cells_data[i];
        
        cur_index = -1;
        shrink_string = false;
        parse_status = 0;
        
        
        //This is where we put in the state machine
        //for now let's keep it simple ...
        //
        //Depending on length, we may want to try SIMD
        
        //parse_status
        //---------------------------
        //0 - not done
        //1 - done
        //2 - invalid escape char
        //3 - invalid hex 
        
        //TODO: This code would benefit from being broken up into functions ...
        
        while (!parse_status) {
            
            if (*p == '"'){
                parse_status = 1;
            }else if(*p == '\\'){
                ++p; //Move onto the next character that is escaped
                
                //Process a unicode escape value e.g. \u00C8 => �
                if (*p == 'u'){
                    shrink_string = true;
                    
                    unicode_char_value = 0;
                    for (int iHex = 0; iHex < 4; iHex++){
                        //Go from hex char to numerical value, e.g. from f to 15
                        hex_numerical_value = hex_numerical_values[*(++p)];
                        //-1 is internal value for not valid 
                        //  (i.e. not 0-9,a-f, or A-F)
                        // Couldn't use 0, as 0 is a valid value
                        if (hex_numerical_value == -1){
                            parse_status == 3;
                            break;
                        }else{
                            unicode_char_value = (unicode_char_value << 4) + hex_numerical_value;
                        }
                    }
                    
                    output_data[++cur_index] = unicode_char_value;
                }else{
                    escape_char = escape_values[*p];
                    //Here 0 represents an in invalid
                    if(escape_char == 0){
                        parse_status == 2;
                    }else{
                        shrink_string = true;
                        output_data[++cur_index] = escape_char;
                    }
                }
            }else if(*p > 127){
                shrink_string = true;
                //Inspect first byte to get # of bytes
                //2 bytes 110...   >> 5 == 6 - switch to 0b110
                //3 bytes 1110.... >> 4 == 14
                //4 bytes 11110... >> 3 == 30
                //are 5 & 6 valid?
                //
                //errors (parse status # listed)
                //- 10 - invalid first byte (currently no distinction
                //       between 5 & 6 bytes, or other alternatives:
                //       10... or 11111110 or 11111111
                //        
                //- 11 - invalid # of continuation bytes (given first byte)
                //       continuation byte is 10...
                //- 12 - not 2 byte compatible - TODO: Allow warning or 
                //       error, on warning, write value as whatever the
                //       unicode value is for not-representable or whatever
                //       the proper terminology is ..
                
                //TODO: technically we should check for null values
                //in each of the bytes, as this is not valid utf-8 
                //(overlong encoding)
                
                //2 bytes
                if ((*p >> 5) == 0b110){ 
                    
                    utf8_value = *p & 0b11111;
                    ++p;
                    
                    if (IS_CONTINUATION_BYTE){
                        utf8_value = (utf8_value << 6) + (*p & 0b111111);
                        //mexPrintf("2 byte result: %d\n",utf8_value);
                        output_data[++cur_index] = (uint16_t) utf8_value;
                    }else{
                        parse_status = 11;
                    }
                 
                //3 bytes    
                }else if((*p >> 4) == 0b1110){
                
                //bits
                //1 -> 4
                //2 -> 6
                //3 -> 6   => 16 total, fits into 2 bytes
                //This also means that anything that is 4 bytes will
                //never be valid when stored as 2 bytes
                //TODO: We could do all the math with uint16t
                    
                    utf8_value = *p & 0b1111;
                    ++p;
                    
                    if (IS_CONTINUATION_BYTE){
                        utf8_value = (utf8_value << 6) + (*p & 0b111111);
                        ++p;
                        if (IS_CONTINUATION_BYTE){
                            utf8_value = (utf8_value << 6) + (*p & 0b111111);
                            //mexPrintf("2 byte result: %d\n",utf8_value);
                            output_data[++cur_index] = (uint16_t) utf8_value;
                        }else{
                            parse_status = 11;
                        }
                    }else{
                        parse_status = 11;
                    }
                    
                //4 bytes     
                }else if((*p >> 3) == 0b11110){
                    //not 2 byte compatible
                    parse_status = 12;
                }else{
                    //invalid first byte
                    parse_status = 10;
                }
                
            //Just a regular old character to store    
            }else{
                output_data[++cur_index] = *p;    
            }
            
            ++p;
            
        } // End of while statement ...
        
        if (parse_status == 1){
            if (shrink_string){
                mxSetN(mxGetCell(cell_array,i),cur_index+1);
            }
        }else{
            //TODO: This needs to be flushed out with parse_status values
            mexPrintf("error parse status: %d\n",parse_status);
            mexErrMsgIdAndTxt("turtle:json","Error parsing string or key");
            //We have an error 
        }
        
        
    }

    mxFree(all_cells_data);
    
    //Storage of the data for later
    //---------------------------------------------------
    if (is_key){
        TOC_AND_LOG(key_parse,key_parsing_time);
        mxAddField(plhs[0],"keys");
        mxSetField(plhs[0],0,"keys",cell_array);     
    }else{
        TOC_AND_LOG(string_parse,string_parsing_time);
        mxAddField(plhs[0],"strings");
        mxSetField(plhs[0],0,"strings",cell_array);        
    }

}