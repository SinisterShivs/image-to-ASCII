#ifndef INPUT_HANDLER_H // include guard
#define INPUT_HANDLER_H

// 4096 is the limit for linux, 256 is for windows.
#define INPUT_PATH_SIZE 4096

/* User paste in path to file and this method gets it */
// TODO: potentially change unsupported format to supported ones (webp -> png)
// TODO: potentially turn a video into series of frames to be displayed
void get_input();

#endif