//----------------------------------------------------------------------------//
//               █      █                                                     //
//               ████████                                                     //
//             ██        ██                                                   //
//            ███  █  █  ███        bin2header.c                              //
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
//        and must not be misrepresented as being the original software.      //
//     4. This notice may not be removed or altered from any source           //
//        distribution.                                                       //
//     5. Most important, you must have fun. ;)                               //
//                                                                            //
//      Visit opensource.amazingcow.com for more open-source projects.        //
//                                                                            //
//                                  Enjoy :)                                  //
//----------------------------------------------------------------------------//

//COWTODO: #1 - Add a option to specify the max columns to write \
//              If we can write all bytes in only one line the file size is reduced.
//COWTODO: #2 - The verbose mode is very nonsense today - Add more util output.

//std
#include <ctype.h>
#include <getopt.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
//stdcow
#include "stdcow.h"
#include "cowlog.h"
#include "cowmalloc.h"
#include "cowpath.h"

////////////////////////////////////////////////////////////////////////////////
// MACROS                                                                     //
////////////////////////////////////////////////////////////////////////////////
#define ONLY_IN_VERBOSE(_code_) \
    if(options.verbose) {       \
        { _code_ }              \
    }


////////////////////////////////////////////////////////////////////////////////
// Constants                                                                  //
////////////////////////////////////////////////////////////////////////////////
#define kBufferSize_ErrorMessage 1024
#define kVersion_Major    0
#define kVersion_Minor    0
#define kVersion_Revision 2


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
//Options init / clean
void options_init    (options_t *options);
void options_validate(options_t *options);
void options_clean   (options_t *options);


//Prints
void print_help   ();
void print_version();
void print_error  (const char *msg, ...);

//Helpers
size_t get_file_size(FILE *file);

//Parse
void parse_cmd_options(int argc, char *argv[], options_t *options);

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

    //Init / Parse / Validate.
    options_init(&options);
    parse_cmd_options(argc, (char **)argv, &options);
    options_validate(&options);

    ONLY_IN_VERBOSE(
        COW_PRINT("Run info:");
        COW_PRINT("  verbose      : %d", options.verbose);
        COW_PRINT("  block size   : %d", options.block_size);
        COW_PRINT("  indent size  : %d", options.indent_size);
        COW_PRINT("  in filename  : %s", options.in_filename);
        COW_PRINT("  out filename : %s", options.out_filename);
        COW_PRINT("  array name   : %s", options.array_name);
    );

    //Init the input and output files...
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
    UCHAR *buf      = COW_MALLOC(sizeof(UCHAR) * options.block_size);

    //Read the binary file and write the info to header...
    size_t read_count = 0;
    while((read_count = fread(buf, sizeof(UCHAR), options.block_size, fin)) > 0)
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
//Options init / clean
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
    //User didn't pass the output filename.
    //So copy the input filename and replace the
    //"possible" extension with a .h extension
    if(!options->out_filename)
    {
        char *filename;
        char *ext;
        int   dot_index;

        dot_index = cow_path_splitext(options->in_filename,
                                      &filename,
                                      &ext);

        //Check if output filename isn't a directory.
        int filename_size = strlen(filename);
        if(filename[filename_size - 1] == '/')
        {
            //Don't bother with the filename and ext string
            //We gonna fail anyway, don't need dealloc them...
            print_error("output filename cannot be a directory (%s)", filename);
        }

        //The 3 is for the .h\0
        options->out_filename = COW_MALLOC(sizeof(char) * filename_size + 3);
        strcpy(options->out_filename, filename);
        strcat(options->out_filename, ".h");

        COW_FREE_NULL(filename);
        COW_SAFE_FREE_NULL(ext);
    }

    //Check if output filename isn't a directory.
    if(options->out_filename[strlen(options->out_filename) -1] == '/')
    {
        print_error("output filename cannot be a directory (%s)",
                    options->out_filename);
    }

    /* Get the full path for the output */
    char *out_fullpath_tmp = cow_path_canonizepath(options->out_filename);
    COW_FREE_NULL(options->out_filename); /* Free the previous buffer */
    options->out_filename = strdup(out_fullpath_tmp);
    COW_FREE_NULL(out_fullpath_tmp); /* Free the temp buffer */


    //User didn't pass the array name
    //So copy the output filename and gets only the filename part.
    if(!options->array_name)
    {
        char *head;
        char *tail;
        char *root;
        char *ext;

        cow_path_split(options->out_filename, &head, &tail);
        cow_path_splitext(tail, &root, &ext);

        options->array_name = strdup(root);

        COW_SAFE_FREE_NULL(head);
        COW_SAFE_FREE_NULL(tail);
        COW_SAFE_FREE_NULL(root);
        COW_SAFE_FREE_NULL(ext);
    }

    //Clean up the array name to make it contains only valid
    //characters in C identifier.
    for(int i = 0; i < strlen(options->array_name); ++i)
    {
        char c = options->array_name[i];
        if(c != '_' && !isalnum(c))
            options->array_name[i] = '_';
    }
}

void options_clean(options_t *options)
{
    COW_FREE_NULL(options->in_filename);
    COW_FREE_NULL(options->out_filename);
    COW_FREE_NULL(options->array_name);
}


//Prints
void print_help(int exit_code)
{
    printf("Usage:   \n \
  bin2header -h | -v \n \
  bin2header [-V] [-ib <size>] [-a <name>] [-o <filename>] <input-filename> \n\
\n\
Options: \n\
  *-h --help    : Show this screen. \n\
  *-v --version : Show app version and copyright. \n\
\n\
   -V -verbose : Enable Verbose mode. \n\
\n\
   -i --indent-size <size> : Indentation in spaces. \n\
   -b --block-size  <size> : How many block will be read each time. \n\
                             Greater block size increases speed in \n\
                             expense of memory. \n\
\n\
   -a --array-name <name> : The name of resulting array. \n\
                            If not set will be the same of output filename. \n\
   -o --output <filename> : The name of output file. \n\
                            If not set will be the same of input filename  \n\
                            with the '.h' extension. \n\
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
    printf("bin2header - %d.%d.%d - N2OMatt <n2omatt@amazingcow.com> \n\
Copyright (c) 2016 - Amazing Cow \n\
This is a free software (GPLv3) - Share/Hack it \n\
Check opensource.amazingcow.com for more :)\n\n",
    kVersion_Major,
    kVersion_Minor,
    kVersion_Revision);

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
    char *header_str = COW_MALLOC(sizeof(char) *
                                 (fmt_str_size + array_name_size + 1));
    sprintf(header_str, fmt_str, options->array_name);

    //Make uppercase.
    for(int i = 0; i < strlen(header_str); ++i)
        header_str[i] = toupper(header_str[i]);

    return header_str;
}


//Parse
void parse_cmd_options(int argc, char *argv[], options_t *options)
{
    // Short |        Long | Description
    //-----------------------------------
    //     h | help        | help.
    //     v | version     | version.
    //     V | verbose     | verbose.
    //     a | array-name  | array name.
    //     o | output      | output file.
    //     b | block-size  | block size.
    //     i | indent-size | indent size.

    const char *options_str = "hvVa:o:b:i:";

    static struct option long_options[] = {
           {"help",        no_argument,       NULL,  'h' },
           {"version",     no_argument,       NULL,  'v' },
           {"verbose",     no_argument,       NULL,  'V' },
           {"array-name",  required_argument, NULL,  'a' },
           {"output",      required_argument, NULL,  'o' },
           {"block-size",  required_argument, NULL,  'b' },
           {"indent-size", required_argument, NULL,  'i' },
           {0,             0,                 0,      0  }
    };


    //Set the defaults.
    options->verbose     = false;
    options->block_size  = 32;
    options->indent_size = 4;

    //Parse the options...
    int  opt_index;
    char curr_opt;
    while(true)
    {
        curr_opt = getopt_long(argc, argv,
                               options_str,
                               long_options,
                               &opt_index);
        if(curr_opt == -1)
            break;

        switch(curr_opt)
        {
            //Help and Version.
            case 'h': print_help   (0); break;
            case 'v': print_version(0); break;

            //Verbose.
            case 'V' : options->verbose = true; break;

            //Block and Indent size.
            case 'b' :
                if(!cow_atoi_checked(optarg, &options->block_size))
                {
                    print_error("error while parsing (%s) flag - invalid number: %s",
                                "-b | --block-size", optarg);
                }
                if(options->block_size <= 0)
                {
                    print_error("error while parsing (%s) flag - block-size must be > 0",
                                "-b | --block-size");
                }
                break;
            case 'i' :
                if(!cow_atoi_checked(optarg, &options->indent_size))
                {
                     print_error("error while parsing (%s) flag - invalid number %s",
                                 "-i | --indent-size", optarg);
                }
                break;

            //Array and Output names.
            case 'a' : options->array_name   = strdup(optarg); break;
            case 'o' : options->out_filename = strdup(optarg); break;

            //Invalid options...
            default :
                print_help(1);
        }
    }

    //Check if user gave the input file.
    if(optind >= argc)
        print_error("missing input binary filename.");

    //Set the input filename.
    //Copy the in_filename from argv.
    options->in_filename = strdup(argv[optind]);
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
    static const char *value_fmt_str      = "'\\x%02x',";
    static const int   value_fmt_str_size = 7; //'\xFF',[SPACE]
    static const int   columns_size       = 80;
    const int  max_value_count  = ((columns_size - options->indent_size)
                                   / value_fmt_str_size);

    static int curr_value_count = 0; //Static to preserve the state between calls.

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
