#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <cstring>
#include "tinyfiledialogs.h"

typedef enum AppScreen { MAIN , ENCODE, DECODE ,ENCODE_OPTIONS,DECODE_OPTIONS} AppScreen;
int main(){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Project");

    MaximizeWindow();

    SetTargetFPS(60);
    GuiSetStyle(DEFAULT,TEXT_ALIGNMENT,TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT,TEXT_SIZE,25);
    AppScreen currentWindow = MAIN ;


    char imagePath[512] = "Drop an image here ...." ;
    bool imageLoaded = false ;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (currentWindow == MAIN)
        {
            const char* maintext = "PIXEL-MUTE" ;
            int maintextwidth = MeasureText(maintext,80);

            DrawText("PIXEL-MUTE",(GetScreenWidth()/2) - (maintextwidth/2),300,80, DARKGRAY);
            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-100,410,200,70},"ENCODER"))
            {
                currentWindow = ENCODE;
            }

            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-100,490,200,70},"DECODER"))
            {
                currentWindow = DECODE;
            }
            
        }

        if (currentWindow == ENCODE)
        {
            if (GuiButton(Rectangle{30,30,120,70},"BACK"))
            {
                currentWindow = MAIN;
            }
            const char* droptext = "DROP YOUR IMAGE HERE......";
            int droptextwidth = MeasureText(droptext,75);
            DrawRectangleLines(200,170,GetScreenWidth()-400,GetScreenHeight()-400,BLACK);
            DrawRectangle(201,171,GetScreenWidth()-402,GetScreenHeight()-402,LIGHTGRAY);
            DrawText(droptext,float(GetScreenWidth()/2)-float(droptextwidth/2),float(GetScreenHeight()/2)-float(75/2),75,GRAY);
            if (IsFileDropped())
            {
            FilePathList droppedFiles =  LoadDroppedFiles() ;

            if (droppedFiles.count > 0)
            {
                strcpy(imagePath,droppedFiles.paths[0]);
                imageLoaded=true ;
            }
            UnloadDroppedFiles(droppedFiles);
            currentWindow = ENCODE_OPTIONS ;
            }

            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-150,float(GetScreenHeight())-220,300,60},"BROWSE FILES"))
            {
                const char* filterPattern[4] = { "*.png" , "*.jpg" , "*.jpeg" , "*.bmp"};
                
                const char* selectedFilePath = tinyfd_openFileDialog(
                    "Select and Image",
                    "",
                    4,
                    filterPattern,
                    "Image Files",
                    0
                );

                if (selectedFilePath != NULL)
                {
                    strcpy(imagePath,selectedFilePath);
                    imageLoaded = true;
                    currentWindow = ENCODE_OPTIONS;
                }
                
            }
        }

        if (currentWindow == DECODE)
        {
            if (GuiButton(Rectangle{30,30,120,70},"BACK"))
            {
                currentWindow = MAIN;
            }
            const char* droptext = "DROP YOUR IMAGE HERE......";
            int droptextwidth = MeasureText(droptext,75);
            DrawRectangleLines(200,170,GetScreenWidth()-400,GetScreenHeight()-400,BLACK);
            DrawRectangle(201,171,GetScreenWidth()-402,GetScreenHeight()-402,LIGHTGRAY);
            DrawText(droptext,float(GetScreenWidth()/2)-float(droptextwidth/2),float(GetScreenHeight()/2)-float(75/2),75,GRAY);
            if (IsFileDropped())
            {
            FilePathList droppedFiles =  LoadDroppedFiles() ;

            if (droppedFiles.count > 0)
            {
                strcpy(imagePath,droppedFiles.paths[0]);
                imageLoaded=true ;
            }
            UnloadDroppedFiles(droppedFiles);
            currentWindow = DECODE_OPTIONS;
            }

            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-150,float(GetScreenHeight())-220,300,60},"BROWSE FILES"))
            {
                const char* filterPattern[4] = { "*.png" , "*.jpg" , "*.jpeg" , "*.bmp"};
                
                const char* selectedFilePath = tinyfd_openFileDialog(
                    "Select and Image",
                    "",
                    4,
                    filterPattern,
                    "Image Files",
                    0
                );

                if (selectedFilePath != NULL)
                {
                    strcpy(imagePath,selectedFilePath);
                    imageLoaded = true;
                    currentWindow = ENCODE_OPTIONS;
                }
                
            }
            
        }

        if (currentWindow == ENCODE_OPTIONS)
        {
            if (GuiButton(Rectangle{30,30,120,70},"BACK"))
            {
                currentWindow = MAIN;
            }
            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-200,425,400,70},"LSB (Least Significant Bit)"))
            {
                //
            }
            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-200,505,400,70},"EOF (End of File)"))
            {
                //
            }
            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-200,585,400,70},"Palette / Alpha Channel Hiding"))
            {
                //
            }

        }

        if (currentWindow == DECODE_OPTIONS)
        {
            if (GuiButton(Rectangle{30,30,120,70},"BACK"))
            {
                currentWindow = MAIN;
            }
            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-200,385,400,70},"LSB (Least Significant Bit)"))
            {
                //
            }
            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-200,465,400,70}," EOF (End of File)"))
            {
                //
            }
            if (GuiButton(Rectangle{float(GetScreenWidth()/2)-200,545,400,70},"Palette / Alpha Channel Hiding"))
            {
                //
            }
        }
        
        
        
        EndDrawing();
    }
    CloseWindow();
    return 0;

}

// 220