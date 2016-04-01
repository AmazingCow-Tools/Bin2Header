//----------------------------------------------------------------------------//
//               █      █                                                     //
//               ████████                                                     //
//             ██        ██                                                   //
//            ███  █  █  ███        bin2header.cpp                            //
//            █ █        █ █        Bin2Header                                //
//             ████████████                                                   //
//           █              █       Copyright (c) 2016                        //
//          █     █    █     █      AmazingCow - www.AmazingCow.com           //
//          █     █    █     █                                                //
//           █              █       N2OMatt - n2omatt@amazingcow.com          //
//             ████████████         www.amazingcow.com/n2omatt                //
//                                                                            //
//                  This software is licensed as GPLv3                        //
//                 CHECK THE COPYING FILE TO MORE DETAILS                     //
//                                                                            //
//    Permission is granted to anyone to use this software for any purpose,   //
//   including commercial applications, and to alter it and redistribute it   //
//               freely, subject to the following restrictions:               //
//                                                                            //
//     0. You **CANNOT** change the type of the license.                      //
//     1. The origin of this software must not be misrepresented;             //
//        you must not claim that you wrote the original software.            //
//     2. If you use this software in a product, an acknowledgment in the     //
//        product IS HIGHLY APPRECIATED, both in source and binary forms.     //
//        (See opensource.AmazingCow.com/acknowledgment.html for details).    //
//        If you will not acknowledge, just send us a email. We'll be         //
//        *VERY* happy to see our work being used by other people. :)         //
//        The email is: acknowledgment_opensource@AmazingCow.com              //
//     3. Altered source versions must be plainly marked as such,             //
//        and must notbe misrepresented as being the original software.       //
//     4. This notice may not be removed or altered from any source           //
//        distribution.                                                       //
//     5. Most important, you must have fun. ;)                               //
//                                                                            //
//      Visit opensource.amazingcow.com for more open-source projects.        //
//                                                                            //
//                                  Enjoy :)                                  //
//----------------------------------------------------------------------------//

//std
#include <ctype.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


////////////////////////////////////////////////////////////////////////////////
// MACROS                                                                     //
////////////////////////////////////////////////////////////////////////////////
#define B2H_SAFE_FREE(_ptr_) do { \
    if(_ptr_) {                   \
        free((void*)_ptr_);       \
        _ptr_ = NULL;             \
    }                             \
} while(0);


////////////////////////////////////////////////////////////////////////////////
// Constants                                                                  //
////////////////////////////////////////////////////////////////////////////////
#define kBufferSize_ErrorMessage 1024


////////////////////////////////////////////////////////////////////////////////
// Types                                                                      //
////////////////////////////////////////////////////////////////////////////////
typedef enum { false, true } bool;
typedef unsigned char UCHAR;

typedef struct _options_t
{
    bool verbose;

    char *in_filename;
    char *out_filename;

    int block_size;
    int indent_size;

    char *array_name;

} options_t;


////////////////////////////////////////////////////////////////////////////////
// Function Declarations                                                      //
////////////////////////////////////////////////////////////////////////////////
//Options init/clean
void options_init (options_t *options);
void options_validate(options_t *options);
void options_clean(options_t *options);


//Prints
void print_help();
void print_version();
void print_error(const char *msg, ...);

//Helpers
size_t get_file_size(FILE *file);

//Parse
void parse_cmd_options(int argc, char *argv[], options_t *options);
bool b2h_atoi(const char *str, int *value_ptr);

//Write
void write_header             (FILE *file, options_t *options);
void write_include_guard_open (FILE *file, options_t *options);
void write_include_guard_close(FILE *file, options_t *options);
void write_array_open         (FILE *file, options_t *options);
void write_array_close        (FILE *file, options_t *options);
void write_array_constant     (FILE *file, options_t *options, size_t size);
void write_byte               (FILE *file, options_t *options, UCHAR value);


////////////////////////////////////////////////////////////////////////////////
// Main                                                                       //
////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char *argv[])
{
    options_t options;
    parse_cmd_options(argc, (char **)argv, &options);

    printf("verbose      : %d\n", options.verbose);
    printf("block size   : %d\n", options.block_size);
    printf("indent size  : %d\n", options.indent_size);
    printf("in filename  : %s\n", options.in_filename);
    printf("out filename : %s\n", options.out_filename);
    printf("array name   : %s\n", options.array_name);

    return 0;
    FILE *fin  = NULL;
    FILE *fout = NULL;

    if(!(fin = fopen(options.in_filename, "rb")))
    {
        perror(options.in_filename);
        exit(1);
    }

    if(!(fout = fopen(options.out_filename, "w")))
    {
        perror(options.out_filename);
        exit(1);
    }

    //Write the start info...
    write_header            (fout, &options);
    write_include_guard_open(fout, &options);
    write_array_open        (fout, &options);


    size_t fin_size = get_file_size(fin);
    UCHAR *buf      = (UCHAR *)malloc(sizeof(UCHAR) * options.block_size);

    //Read the binary file and write the info to header...
    size_t read_count = 0;
    while((read_count = fread(buf, sizeof(unsigned char), options.block_size, fin)) > 0)
    {
        for(int i = 0; i < read_count; ++i)
            write_byte(fout, &options, buf[i]);
    }

    //Write the end info...
    write_array_close        (fout, &options);
    write_array_constant     (fout, &options, fin_size);
    write_include_guard_close(fout, &options);

    //Clean up.
    options_clean(&options);

    //Close the FILEs.
    fclose(fin);
    fclose(fout);
}


////////////////////////////////////////////////////////////////////////////////
// Function Definitions                                                       //
////////////////////////////////////////////////////////////////////////////////
//Options init/clean
void options_init(options_t *options)
{
    options->verbose = false;

    options->in_filename  = NULL;
    options->out_filename = NULL;

    options->block_size   = -1;
    options->indent_size  = -1;

    options->array_name = NULL;
}
void options_validate(options_t *options)
{
    /* COWTODO:
       Bit of shame - I do not know
       how to make path manipulations on C
       like in os.path and os packages of python
       Since I haven't internet connection now
       Let it for later */
}
void options_clean(options_t *options)
{
    B2H_SAFE_FREE(options->in_filename);
    B2H_SAFE_FREE(options->out_filename);
    B2H_SAFE_FREE(options->array_name);
}


//Prints
void print_help(int exit_code)
{
    printf("Usage: \n \
  bin2header [-hv] [-V] [-ib <size>] [-a <name>] [-o <filename>] <input-filename> \n\
\n\
Options: \n\
  *-h            : Show this screen. \n\
  *-v            : Show app version and copyright. \n\
   -V            : Enable Verbose mode. \n\
   -i <size>     : Indentation in spaces. \n\
   -b <size>     : How many block will be read each time. \n\
                   Greater block size increases speed in expense of memory. \n\
   -a <name>     : The name of resulting array. \n\
                   If not set will be the same of output filename. \n\
   -o <filename> : The name of output file. \n\
                   If not set will be the same of input filename with the '.h' extension. \n\
\n\
Notes:\n\
  In (-a <name>) all characters that doesn't make a valid C identifier will \n\
  be replaced with '_' (underscore) character.\n\
\n\
  Options marked with * are exclusive, i.e. the bin2header will run that \n\
  and exit successfully after the operation.\n\n");

    if(exit_code >= 0)
        exit(exit_code);
}
void print_version(int exit_code)
{
    printf("bin2header - 0.1.0 - N2OMatt <n2omatt@amazingcow.com> \n\
Copyright (c) 2016 - Amazing Cow \n\
This is a free software (GPLv3) - Share/Hack it \n\
Check opensource.amazingcow.com for more :)\n\n");

    if(exit_code >= 0)
        exit(exit_code);
}

void print_error(const char *msg, ...)
{
    char buffer[kBufferSize_ErrorMessage];

    /* Reset the buffer */
    memset(buffer, 0, kBufferSize_ErrorMessage);

    /* Build the buffer with the variadic args list */
    va_list ap;
    va_start(ap, msg);
    vsnprintf(buffer, kBufferSize_ErrorMessage, msg, ap);
    va_end(ap);

    fprintf(stderr, "bin2header: %s\n", buffer);
    exit(1);
}



//Helpers
size_t get_file_size(FILE *file)
{
    struct stat st;
    fstat(fileno(file), &st);

    return st.st_size;
}

const char *create_header_str(options_t *options)
{
    const char *fmt_str = "_AMAZINGCOW_TOOLS_BIN2HEADER_GENERATED__%s_";

    size_t fmt_str_size    = strlen(fmt_str);
    size_t array_name_size = strlen(options->array_name);

    //Create the header_str with the format and array name.
    char *header_str = (char *)malloc(sizeof(char) * (fmt_str_size + array_name_size + 1));
    sprintf(header_str, fmt_str, options->array_name);

    //Make uppercase.
    for(int i = 0; i < strlen(header_str); ++i)
        header_str[i] = toupper(header_str[i]);

    return header_str;
}


//Parse
void parse_cmd_options(int argc, char *argv[], options_t *options)
{
    //h - help.
    //v - version.
    //V - verbose.
    //a - array name.
    //o - output file.
    //b - block size.
    //i - indent size.
    const char *options_str = "hvVa:o:b:i:";

    //Set the defaults.
    options->verbose     = false;
    options->block_size  = 32;
    options->indent_size = 4;

    //Parse the options...
    char curr_opt;
    while((curr_opt = getopt(argc, argv, options_str)) != -1)
    {
        switch(curr_opt)
        {
            //Help and Version.
            case 'h': print_help(0);    break;
            case 'v': print_version(0); break;

            //Verbose.
            case 'V' : options->verbose = true; break;

            //Block and Indent size.
            case 'b' :
                if(!b2h_atoi(optarg, &options->block_size))
                {
                    print_error("error while parsing -b flag - invalid number %s",
                                optarg);
                }
                break;
            case 'i' :
                if(!b2h_atoi(optarg, &options->indent_size))
                {
                     print_error("error while parsing -i flag - invalid number %s",
                                 optarg);
                }
                break;

            //Array and Output names.
            case 'a' :
                options->array_name = malloc(sizeof(char) * strlen(optarg));
                strcpy(options->array_name, optarg);
                break;

            case 'o' :
                options->out_filename = malloc(sizeof(char) * strlen(optarg));
                strcpy(options->out_filename, optarg);
                break;

            //Invalid options...
            default :
                print_error("invalid flag %c", curr_opt);
                break;
        }
    }

    //Check if user gave the input file.
    if(optind >= argc)
        print_error("missing input binary filename.");

    //Set the input filename.
    options->in_filename = malloc(sizeof(char) * strlen(argv[optind]));
    strcpy(options->in_filename, argv[optind]);
}

bool b2h_atoi(const char *str, int *value_ptr)
{
    int s = strlen(str);
    for(int i = 0; i < s; ++i)
    {
        if(!isdigit(str[i]))
            return false;
    }

    *value_ptr = atoi(str);
    return true;
}


//Write
void write_header(FILE *file, options_t *options)
{
    fprintf(file, "/*\n");
    fprintf(file, "   AmazingCow - Bin2Header - www.amazingcow.com \n");
    fprintf(file, "   In  file: %s\n", options->in_filename);
    fprintf(file, "   Out file: %s\n", options->out_filename);
    fprintf(file, "*/\n\n");
}

void write_include_guard_open(FILE *file, options_t *options)
{
    const char *header_str = create_header_str(options);

    fprintf(file, "\n#ifndef %s\n",  header_str);
    fprintf(file, "#define %s\n\n",  header_str);

    free((void*)header_str);
}

void write_include_guard_close(FILE *file, options_t *options)
{
    const char *header_str = create_header_str(options);

    fprintf(file, "#endif // defined( %s ) //\n", header_str);

    free((void *)header_str);
}

void write_array_open(FILE *file, options_t *options)
{
    fprintf(file, "unsigned char %s[] = {\n", options->array_name);
}

void write_array_close(FILE *file, options_t *options)
{
    fprintf(file, "\n}; // %s\n\n", options->array_name);
}

void write_array_constant(FILE *file, options_t *options, size_t size)
{
    fprintf(file, "static const int kSize_%s = %zu;\n\n", options->array_name, size);
}

void write_byte(FILE *file, options_t *options, UCHAR value)
{
    //Helper constants.
    const char * value_fmt_str      = "\\x%02x, ";
    const int    value_fmt_str_size = 6;
    const int    columns_size       = 80;
    const int    max_value_count    = ((columns_size - value_fmt_str_size) / options->indent_size) + 1;

    static int curr_value_count = 0; //Static to preseve the state between calls.

    //Print the new lines when we reach the right most column of file.
    if(curr_value_count >= max_value_count)
    {
        fprintf(file, "\n");
        curr_value_count = 0;
    }

    //Print the indentation.
    if(curr_value_count == 0)
    {
        char spaces_str[options->indent_size + 2];
        memset(spaces_str, ' ', options->indent_size);
        spaces_str[options->indent_size] = '\0';

        fprintf(file, "%s", spaces_str);
    }


    fprintf(file, value_fmt_str, value);

    ++curr_value_count;
}
