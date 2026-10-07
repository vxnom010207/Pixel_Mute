#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include "tinyfiledialogs.h"

// Simplified state machine: Options screens and intermediate steps removed
typedef enum AppScreen { MAIN, ENCODE_INPUT, DECODE, DECODE_RESULT } AppScreen;

//logic 

bool EncodeVoronoi(const char* message, const char* outputPath) {
    int width = 512;
    int height = 512;
    int cellSize = 64; // 64 char , 8 bt 8 grid
    
    Image img = GenImageColor(width, height, BLANK);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color* pixels = (Color*)img.data;
    
    struct Seed { int x, y; Color c; };
    std::vector<Seed> seeds(64);
    
    int msgLen = strlen(message);
    
    // map char to cells 
    for (int i = 0; i < 64; ++i) {
        int gridX = i % 8;
        int gridY = i / 8;
        
        char c = (i < msgLen) ? message[i] : '\0';
        
        // split ASCII into 4 bit nibble , scale by 3 for spacing
        int offsetX = (c & 0x0F) * 3; 
        int offsetY = ((c >> 4) & 0x0F) * 3;
        
        seeds[i].x = (gridX * cellSize) + offsetX + 8;
        seeds[i].y = (gridY * cellSize) + offsetY + 8;
        seeds[i].c = ColorFromHSV((float)GetRandomValue(0, 360), 0.7f, 0.9f);
    }
    
    // rendering vonoroi diagram
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float minDist = 999999.0f;
            int closestSeed = 0;
            
            for (int i = 0; i < 64; ++i) {
                float dx = x - seeds[i].x;
                float dy = y - seeds[i].y;
                float dist = sqrt(dx*dx + dy*dy);
                if (dist < minDist) {
                    minDist = dist;
                    closestSeed = i;
                }
            }
            
            Color baseColor = seeds[closestSeed].c;
            int shade = 255 - (int)(minDist * 1.5f);
            if (shade < 60) shade = 60;
            
            Color finalColor = {
                (unsigned char)((baseColor.r * shade) / 255),
                (unsigned char)((baseColor.g * shade) / 255),
                (unsigned char)((baseColor.b * shade) / 255),
                255
            };
            
            if (x == seeds[closestSeed].x && y == seeds[closestSeed].y) {
                finalColor.a = 254; 
            }
            
            pixels[y * width + x] = finalColor;
        }
    }
    
    ExportImage(img, outputPath);
    UnloadImage(img);
    return true;
}


// decoding
std::string DecodeVoronoi(const char* inputPath) {
    Image img = LoadImage(inputPath);
    if (img.data == nullptr) return "ERROR: Failed to load image.";
    
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color* pixels = (Color*)img.data;
    
    int cellSize = 64;
    std::string message = "";
    
    for (int i = 0; i < 64; ++i) {
        int cellStartX = (i % 8) * cellSize;
        int cellStartY = (i / 8) * cellSize;
        
        bool found = false;
        char decodedChar = '\0';
        
        for (int y = cellStartY; y < cellStartY + cellSize; ++y) {
            for (int x = cellStartX; x < cellStartX + cellSize; ++x) {
                if (x >= img.width || y >= img.height) continue;
                
                if (pixels[y * img.width + x].a == 254) {
                    int offsetX = x - cellStartX - 8;
                    int offsetY = y - cellStartY - 8;
                    
                    int nibble1 = offsetX / 3;
                    int nibble2 = offsetY / 3;
                    decodedChar = (char)(nibble1 | (nibble2 << 4));
                    
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        
        if (!found || decodedChar == '\0') break; 
        message += decodedChar;
    }
    
    UnloadImage(img);
    return message;
}



// GUI
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "PIXEL-MUTE");
    MaximizeWindow();
    SetTargetFPS(60);
    
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
    
    AppScreen currentWindow = MAIN;
    
    char imagePath[512] = "";
    char secretMessage[512] = "";
    bool textBoxEditMode = false;
    std::string decodedResult = "";

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        if (currentWindow == MAIN) {
            int titleWidth = MeasureText("PIXEL-MUTE", 80);
            DrawText("PIXEL-MUTE", (GetScreenWidth() / 2) - (titleWidth / 2), 200, 80, DARKGRAY);
            
            if (GuiButton(Rectangle{float(GetScreenWidth() / 2) - 100, 310, 200, 70}, "ENCODER")) {
                memset(secretMessage, 0, sizeof(secretMessage)); // Clear old text
                currentWindow = ENCODE_INPUT;
            }
            if (GuiButton(Rectangle{float(GetScreenWidth() / 2) - 100, 390, 200, 70}, "DECODER")) {
                currentWindow = DECODE;
            }
        }
        
        else if (currentWindow == ENCODE_INPUT) {
            if (GuiButton(Rectangle{30, 30, 120, 70}, "BACK")) currentWindow = MAIN;
            
            DrawText("Enter secret message (Max 64 chars):", GetScreenWidth() / 2 - MeasureText("Enter secret message (Max 64 chars):", 30) / 2, 200, 30, DARKGRAY);
            
            if (GuiTextBox(Rectangle{float(GetScreenWidth() / 2) - 300, 250, 600, 60}, secretMessage, 64, textBoxEditMode)) {
                textBoxEditMode = !textBoxEditMode;
            }
            
            if (GuiButton(Rectangle{float(GetScreenWidth() / 2) - 150, 350, 300, 70}, "GENERATE & SAVE")) {
                const char* filter[1] = { "*.png" };
                const char* savePath = tinyfd_saveFileDialog("Save File", "output.png", 1, filter, "PNG File");
                
                if (savePath) {
                    if (EncodeVoronoi(secretMessage, savePath)) {
                        tinyfd_messageBox("Success", "Voronoi artwork successfully generated and saved!", "ok", "info", 0);
                        currentWindow = MAIN;
                    } else {
                        tinyfd_messageBox("Error", "Generation failed.", "ok", "error", 0);
                    }
                }
            }
        }

        else if (currentWindow == DECODE) {
            if (GuiButton(Rectangle{30, 30, 120, 70}, "BACK")) currentWindow = MAIN;
            
            const char* droptext = "DROP VORONOI IMAGE HERE......";
            int droptextwidth = MeasureText(droptext, 75);
            DrawRectangleLines(200, 170, GetScreenWidth() - 400, GetScreenHeight() - 400, BLACK);
            DrawRectangle(201, 171, GetScreenWidth() - 402, GetScreenHeight() - 402, LIGHTGRAY);
            DrawText(droptext, float(GetScreenWidth() / 2) - float(droptextwidth / 2), float(GetScreenHeight() / 2) - float(75 / 2), 75, GRAY);
            
            if (IsFileDropped()) {
                FilePathList droppedFiles = LoadDroppedFiles();
                if (droppedFiles.count > 0) {
                    strcpy(imagePath, droppedFiles.paths[0]);
                    decodedResult = DecodeVoronoi(imagePath);
                    currentWindow = DECODE_RESULT;
                }
                UnloadDroppedFiles(droppedFiles);
            }

            if (GuiButton(Rectangle{float(GetScreenWidth() / 2) - 150, float(GetScreenHeight()) - 220, 300, 60}, "BROWSE FILES")) {
                const char* filterPattern[4] = { "*.png", "*.jpg", "*.jpeg", "*.bmp" };
                const char* selectedFile = tinyfd_openFileDialog("Select Image", "", 4, filterPattern, "Images", 0);
                if (selectedFile) {
                    strcpy(imagePath, selectedFile);
                    decodedResult = DecodeVoronoi(imagePath);
                    currentWindow = DECODE_RESULT;
                }
            }
        }
        
        else if (currentWindow == DECODE_RESULT) {
            if (GuiButton(Rectangle{30, 30, 120, 70}, "MAIN MENU")) currentWindow = MAIN;
            
            DrawText("Extracted Message:", GetScreenWidth() / 2 - MeasureText("Extracted Message:", 30) / 2, 200, 30, DARKGRAY);
            DrawText(decodedResult.c_str(), GetScreenWidth() / 2 - MeasureText(decodedResult.c_str(), 25) / 2, 280, 25, BLUE);
        }

        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

extern "C" int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* pCmdLine, int nCmdShow) {
    return main();
}