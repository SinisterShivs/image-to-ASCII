#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"
#include "input_handler.h"

#define IMAGE "superman.png"

#define ASCII_CHARS " .:-=+*#%@"
#define MAX_WIDTH 200
#define MAX_HEIGHT 200



void store_brightness_pixels(int index, int width, unsigned char brightness);
void process_image(char file_path[]);

int main() {
    char file_path[INPUT_PATH_SIZE];
    get_input(file_path);
    process_image(file_path);
}

void process_image(char file_path[])
{
    // Loading the image
    int width, height, channels;
    unsigned char *data = stbi_load(file_path, &width, &height, &channels, 0);

    if (data == NULL)
    {
        printf("Error in loading image.\n");
    }

    printf("Image loaded width %dpx, height %dpx and %d channels.\n", width, height, channels);

    // now scale it down or else too big
    double scale_factor = (double)width / MAX_WIDTH;
    int new_width = MAX_WIDTH;
    int new_height = (int)(height / (scale_factor * 2));

    // if portrait, height is the cap and width adjusts to proportion
    if(height > width) 
    {
        scale_factor = (double) height / MAX_HEIGHT;
        new_width = (int)(width / (scale_factor / 2));
        new_height = MAX_HEIGHT;
    }

    unsigned char *scaled_data = calloc((new_width * new_height * channels), 1);
    if (!scaled_data)
    {
        printf("Memory allocation failed!\n");
    }

    stbir_resize_uint8_srgb(data, width, height, 0, scaled_data, new_width, new_height, 0, channels);

    // 1D array, each pixel takes up int channels number of slots(3 for RGB).
    // combine all channels into a single brightness value
    // total size of 1D array is channel * column * rows
    int totalSize = new_width * new_height * channels;

    // turn a pixel with rgb into greyscale with one value.
    for (int i = 0; i < totalSize; i += channels)
    {
        unsigned char r = scaled_data[i];
        unsigned char g = scaled_data[i + 1];
        unsigned char b = scaled_data[i + 2];

        unsigned char brightness = (r * 0.299) + (g * 0.587) + (b * 0.114);
        store_brightness_pixels(i / channels, new_width, brightness); // i / channels just makes it work
    }

    stbi_image_free(data);
    free(scaled_data);
}

void store_brightness_pixels(int index, int width, unsigned char brightness)
{
    char ascii[] = ASCII_CHARS;
    int num_chars = strlen(ASCII_CHARS);
    int ascii_index = (brightness * num_chars) / 256;

    printf("%c", ascii[ascii_index]); 
    

    // last character, print then new line
    if ((index + 1) % width == 0)
    {
        printf("\n");
    }
}


