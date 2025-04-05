#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <webp/decode.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "input_handler.h"

void remove_new_line(char string[]);
void format_file_path(char file_path[]);
void deal_with_webp(char file_path[]);

void get_input(char file_path[]) 
{
    printf("Insert file path here: ");
    fgets(file_path, INPUT_PATH_SIZE, stdin);
    remove_new_line(file_path);
    printf("\n");

    if (file_path == NULL) {
        perror("Error can't access file.\n");
    }

    format_file_path(file_path);

    // check if webp, change it to png with a system call
    if (strstr(file_path, ".webp") != NULL) 
    {
        deal_with_webp(file_path);
    }

    
}

/* Since stbi_load only accepts native linux paths, modify the given path to fit*/
void format_file_path(char file_path[])
{
    // change bck slasehs \ into /
    for (int i = 0; file_path[i] != '\0'; i++) 
    {
        if (file_path[i] == '\\')
        {
            file_path[i] = '/';
        }
    }

    // check if a windows directory
    if (file_path[1] == ':') 
    {
        // from which drive
        const char drive = tolower(file_path[0]);
        int length = strlen(file_path);

        // now replace the start "C:" with "/mnt/c"
        memmove(file_path + 4, file_path, length + 1);
        file_path[0] = '/';
        file_path[1] = 'm';
        file_path[2] = 'n';
        file_path[3] = 't';
        file_path[4] = '/';
        file_path[5] = drive;

    }

    // check if from a remote host or something and remove that prefix
    // find firt occurrence of home directory
    char *result = strstr(file_path, getenv("HOME"));
    if (result != NULL)
    {
        int index = result - file_path;

        /* move into first position (file_path), from starting of home dir (file_path + index), 
        move the whole string, including \0, except the first part we are trying to remove (strlen(file_path) - index + 1) */
        memmove(file_path, file_path + index, strlen(file_path) - index + 1);
    }
}

void remove_new_line(char string[]) 
{
    int stringLen = strlen(string);
    if (stringLen > 0 && string[stringLen - 1] == '\n') {
        string[stringLen - 1] = '\0';
    }

    return;
}

// stb does not support webp, so use libwebp to convert it to png
void deal_with_webp(char file_path[]) 
{
    // Open file
    FILE* file = fopen(file_path, "rb");
    if (!file) 
    {
        printf("Error: Unable to open WebP file.\n");
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // Read file into memory
    unsigned char* webp_data = (unsigned char*)malloc(file_size); // malloc because unknow til runtime
    fread(webp_data, file_size, 1, file);
    fclose(file);

    // Decode WebP to raw RGBA
    int width, height;
    uint8_t* decoded_data = WebPDecodeRGBA(webp_data, file_size, &width, &height);
    free(webp_data);

    if (!decoded_data) {
        printf("Error: Failed to decode WebP image.\n");
        return;
    }

    // get the output path
    const char *occurrence = strstr(file_path, ".webp"); // pointer to first occurrence
    const char *extension = "-copy.png";
    int index = occurrence - file_path; // index at first occurrence  
    
    
    //memmove(file_path + index + strlen(extension), file_path + index + strlen(".webp"), strlen(file_path) - index - strlen(".webp") + 1);
    //memcpy(file_path + index, extension, strlen(extension));

    //TODO: null termiuhhbn
    // Replace ".webp" with "-copy.png"
    memmove(file_path + index, extension, strlen(extension) + 1);

    printf("PNG FILE: %s\n", file_path);

    // Save as PNG
    if (!stbi_write_png(file_path, width, height, 4, decoded_data, width * 4)) {
    printf("Error: Failed to write PNG file.\n");
    } else {
        printf("Successfully converted.\n");
    }

    // Free the decoded image data
    WebPFree(decoded_data);
}