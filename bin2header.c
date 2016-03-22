//COWTODO: Add header.
//COWTODO: Add the info files...


//std
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>


////////////////////////////////////////////////////////////////////////////////
// Types                                                                      //
////////////////////////////////////////////////////////////////////////////////
typedef enum { false, true } bool;
typedef unsigned char UCHAR;

typedef struct _options_t
{
    bool verbose;

    const char *in_filename;
    const char *out_filename;

    int block_size;
    int indent_size;

    const char *array_name;

} options_t;


////////////////////////////////////////////////////////////////////////////////
// Function Declarations                                                      //
////////////////////////////////////////////////////////////////////////////////
//Prints
void print_help();
void print_version();

//Helpers
size_t get_file_size(FILE *file);

//Parse
void parse_cmd_options(int argc, const char *argv[], options_t *options);

//Write
void write_header             (FILE *file, options_t *options);
void write_include_guard_open (FILE *file, options_t *options);
void write_include_guard_close(FILE *file, options_t *options);
void write_array_open         (FILE *file, options_t *options);
void write_array_close        (FILE *file, options_t *options);
void write_array_constant     (FILE *file, options_t *options, size_t size);
void write_byte               (FILE *file, UCHAR value);


////////////////////////////////////////////////////////////////////////////////
// Main                                                                       //
////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char *argv[])
{
    options_t options;
    parse_cmd_options(argc, argv, &options);

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

    write_header(fout, &options);
    write_include_guard_open(fout, &options);
    write_array_open(fout, &options);


    int    fin_size = get_file_size(fin);
    UCHAR *buf      = (UCHAR *)malloc(sizeof(UCHAR) * options.block_size);

    size_t read_count = 0;
    while((read_count = fread(buf, sizeof(unsigned char), options.block_size, fin)) > 0)
    {
        for(int i = 0; i < read_count; ++i)
            write_byte(fout, buf[i]);
    }

    write_array_close(fout, &options);
    write_array_constant(fout, &options, fin_size);
    write_include_guard_close(fout, &options);
}


////////////////////////////////////////////////////////////////////////////////
// Function Definitions                                                       //
////////////////////////////////////////////////////////////////////////////////
//Prints
void print_help()
{
    //COWTODO: Implement.
}
void print_version()
{
    //COWTODO: Implement.
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

    char *header_str = (char *)malloc(sizeof(char *) * (fmt_str_size + array_name_size + 1));
    sprintf(header_str, fmt_str, options->array_name);

    //COWTODO: Make the header_str upper case.

    return header_str;
}


//Parse
void parse_cmd_options(int argc, const char *argv[], options_t *options)
{

    //COWTODO: Implement the parsing...
    options->in_filename  = "image.png";
    options->out_filename = "out.txt";

    options->block_size  = 32;
    options->indent_size = 4;

    options->array_name = "array";
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
    fprintf(file, "#endif \n");
}

void write_array_open(FILE *file, options_t *options)
{
    fprintf(file, "unsigned char %s[] = {\n", options->array_name);
}

void write_array_close(FILE *file, options_t *options)
{
    fprintf(file, "};\n\n");
}

void write_array_constant(FILE *file, options_t *options, size_t size)
{
    fprintf(file, "static const int kSize_%s = %zu;\n\n", options->array_name, size);
}

void write_byte(FILE *file, UCHAR value)
{
    //COWTODO: Implement this correctly... (Get the indent_size, don't let it pass the 80 cols, etc.)
    static int v = 0;
    if(v == 8)
    {
        fprintf(file, "\n   ");
        v = 0;
    }

    fprintf(file, "\\x%02x, ", value);
    ++v;
}
