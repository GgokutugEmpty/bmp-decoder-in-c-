//Sample BMP Viewer In C
//A.Goktug
//20/08/2024
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPHeader;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPixPerMeter;
    int32_t  biYPixPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPInfoHeader;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} RGB24;

#pragma pack(pop)

int loadBMP(const char *filename, RGB24 **imageData, int *width, int *height) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    BMPHeader header;
    BMPInfoHeader infoHeader;

    fread(&header, sizeof(header), 1, file);
    if (header.bfType != 0x4D42) { // 'BM' in little-endian
        printf("Not a BMP file.\n");
        fclose(file);
        return -1;
    }

    fread(&infoHeader, sizeof(infoHeader), 1, file);
    if (infoHeader.biBitCount != 24) {
        printf("Only 24-bit BMP files are supported.\n");
        fclose(file);
        return -1;
    }

    *width = infoHeader.biWidth;
    *height = infoHeader.biHeight;

    int padding = (4 - (*width * 3) % 4) % 4;

    *imageData = malloc(*width * *height * sizeof(RGB24));
    if (!*imageData) {
        perror("Error allocating memory");
        fclose(file);
        return -1;
    }

    fseek(file, header.bfOffBits, SEEK_SET);

    for (int y = *height - 1; y >= 0; y--) {
        for (int x = 0; x < *width; x++) {
            fread(&(*imageData)[y * (*width) + x], sizeof(RGB24), 1, file);
        }
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
    return 0;
}

void displayImage(SDL_Renderer *renderer, RGB24 *imageData, int width, int height) {
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, width, height);
    if (!texture) {
        printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
        return;
    }

    SDL_UpdateTexture(texture, NULL, imageData, width * sizeof(RGB24));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);
}

int main(int argc, char *argv[])
{
 printf("\nCredits:A.Goktug");
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <BMP file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("BMP Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    RGB24 *imageData;
    int width, height;

    if (loadBMP(argv[1], &imageData, &width, &height) != 0) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_SetWindowSize(window, width, height);
    displayImage(renderer, imageData, width, height);

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
    }

    free(imageData);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

