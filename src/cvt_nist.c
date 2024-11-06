#include <stdio.h>
#include <stdlib.h> // strtod and strtold

//#define RUN_ONE_LINE_ONLY
#define INTELLISENSE_COMMENT_VAL

#define MAX_STR_INPUT_SIZE  256

typedef long double OPF_t;
#define LD_t_NAME           "long double"
#define LF_t_NAME           "double"
#define FP_t_NAME           "float"

#define A_MIN_A     ('a' - 'A')

char to_upper(char in){
   return ((in > 'Z') ? in - A_MIN_A : in);
}


void extract_string_values(char *src, char *name, char *val, char *uncert, char *unit);
void write_vals_2_file(FILE *file_out, char *dtype, char *name, char *val, char *uncert, char *uni);

int main(){
    FILE *infd = fopen("../NIST_Data/2022.txt", "r");

    if(!infd){
        perror("Invalid Filename");
        return -1;
    }

    printf("File Opened!\n");

    char string_input[MAX_STR_INPUT_SIZE];

    // Skip header
    fgets(string_input, sizeof(string_input), infd);
    while(string_input[0] != '-'){
        //printf("%s\n", string_input);
        fgets(string_input, sizeof(string_input), infd);
    }

    printf("Skipped Header!\n");


    char unit_name[64];
    char value[32];
    char uncertainty[32];
    char unit[16];

    char *opf_dtype = LD_t_NAME;
    char *opf_name = "NIST_CODATA.h";


    FILE *opf_fd = fopen(opf_name, "w");
    fprintf(opf_fd, "#ifndef NIST_CODATA_h\n#define NIST_CODATA_h\n\n");

#ifndef RUN_ONE_LINE_ONLY
    while(fgets(string_input, sizeof(string_input), infd)){
        
        //printf("%s", string_input);

        char *uname = unit_name;
        char *val = value;
        char *uncert = uncertainty;
        char *un = unit;
        
        extract_string_values(string_input, uname, val, uncert, un);
        write_vals_2_file(opf_fd, opf_dtype, unit_name, value, uncertainty, unit);

    }
#else
    fgets(string_input, sizeof(string_input), infd);
    printf("%s", string_input);

    char *uname = unit_name;
    char *val = value;
    char *uncert = uncertainty;
    char *un = unit;
        
    extract_string_values(string_input, uname, val, uncert, un);
        //void write_vals_2_file(FILE *file_out, char *name, char *value, char *uncert, char *unit);
    write_vals_2_file(testout, unit_name, value, uncertainty, unit);
#endif

    printf("All Done!\n");
    fprintf(opf_fd, "\n#endif");
    fclose(opf_fd);

    fclose(infd);

    return EXIT_SUCCESS;
}

void extract_string_values(char *src, char *name, char *val, char *uncert, char *unit){
    int offset = 0;

    int chars_till_eq = 55;

    // Setup and get the variable name
    *(name++) = 'N';
    *(name++) = 'I';
    *(name++) = 'S';
    *(name++) = 'T';
    *(name++) = '_';

    while (offset < MAX_STR_INPUT_SIZE){
        if(src[offset] == ' ' && src[offset + 1] == ' '){
            //*(name) = '\0';
            offset++;
            break;
        } else {
            // Generic Character
            if( src[offset] == ' ' || 
                src[offset] == '-' || 
                src[offset] == '.' || 
                src[offset] == '/' )
            { 
                *(name++) = '_'; 
                chars_till_eq--;
            }
            
            else if(
                src[offset] == ')' ||
                src[offset] == '(' ||
                src[offset] == ',');
            
            else {
                *(name++) = to_upper(src[offset]);
                chars_till_eq--;    
            }
            offset++;
        }
    }

    // pad spaces in the string
    for(int n = 0; n < chars_till_eq; ++n) *(name++) = ' ';
    *(name) = '\0';

    // Skip spaces
    while(src[++offset] == ' ');

    // Get numerical value
    int has_decimal = 0;
    chars_till_eq = 20;

    while(offset < MAX_STR_INPUT_SIZE){
        if(src[offset] == ' ' && src[offset + 1] == ' '){
            //*(val) = '\0';
            offset++;
            break;
        } else {
            // Generic Numerical Char
            if(src[offset] == ' '); // Do nothing for now
            else {
                if(!has_decimal && src[offset] == '.'){
                    *(val++) = src[offset];
                    has_decimal = 1;
                    chars_till_eq--;
                } else
                if(src[offset] != '.'){
                    *(val++) = src[offset];
                    chars_till_eq--;
                }
            }
            offset++;
        }
    }

    // pad spacing
    for(int n = 0; n < chars_till_eq; ++n) *(val++) = ' ';
    *(val) = '\0';

    // Skip spaces
    while(src[++offset] == ' ');

    // Get numerical value
    while(offset < MAX_STR_INPUT_SIZE){
        if(src[offset] == ' ' && src[offset + 1] == ' '){
            *(uncert) = '\0';
            offset++;
            break;
        } else {
            // Generic Numerical Char
            if(src[offset] == ' '); // Do nothing for now
            else *(uncert++) = src[offset];
            offset++;
        }
    }      
    
    // Skip spaces
    while(src[++offset] == ' ');

    // Get units string
    chars_till_eq = 15;

    while(offset < MAX_STR_INPUT_SIZE){
        if(src[offset] == '\n' || src[offset] == '\r' || (src[offset] == ' ' && src[offset + 1] == ' ')){
            //*(unit) = '\0';
            offset++;
            break;
        } else {
            // Generic Character
            //if(src[offset] == ' ') *(unit++) = '_';
            //else 
            *(unit++) = src[offset];
            offset++;
            chars_till_eq--;
        }
    }

    for(int n = 0; n < chars_till_eq; ++n) *(unit++) = ' ';
    *(unit) = '\0';


}


void write_vals_2_file(FILE *file_out, char *dtype, char *name, char *val, char *uncert, char *uni){
#ifdef INTELLISENSE_COMMENT_VAL
    //fprintf(file_out, "\t// %s %s\n\t%s %s = %s; // %s    Uncertainty: %s\n",
    //        val, uni, dtype, name, val, uni, uncert);

    fprintf(file_out, "\t%s %s = %s;\t// %s%s    Uncertainty: %s\n",
            dtype, name ,val, val, uni, uncert
        );

#else
    fprintf(file_out, "\t%s %s = %s;\t// %s    Uncertainty: %s\n",
            dtype, name ,val, uni, uncert
        );
#endif
}