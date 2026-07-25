#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <cstring>

typedef enum AppScreen { MAIN , ENCODE, DECODE ,ENCODE_OPTIONS,DECODE_OPTIONS} AppScreen;
int main(){
    InitWindow(800,450,"Project");
    SetTargetFPS(60);
    GuiSetStyle(DEFAULT,TEXT_ALIGNMENT,TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT,TEXT_SIZE,25);
    AppScreen currentWindow = MAIN ;

    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    SetWindowSize(monitorWidth,monitorHeight);
    ToggleFullscreen();

    char imagePath[512] = "Drop an image here ...." ;
    bool imageLoaded = false ;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        char imagePath[512] ;
        bool imageLoaded = false ;
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
            DrawRectangleLines(200,200,GetScreenWidth()-400,GetScreenHeight()-400,BLACK);
            DrawRectangle(201,201,GetScreenWidth()-402,GetScreenHeight()-402,LIGHTGRAY);
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
        }

        if (currentWindow == DECODE)
        {
            if (GuiButton(Rectangle{30,30,120,70},"BACK"))
            {
                currentWindow = MAIN;
            }
            const char* droptext = "DROP YOUR IMAGE HERE......";
            int droptextwidth = MeasureText(droptext,75);
            DrawRectangleLines(200,200,GetScreenWidth()-400,GetScreenHeight()-400,BLACK);
            DrawRectangle(201,201,GetScreenWidth()-402,GetScreenHeight()-402,LIGHTGRAY);
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
        }
        
        EndDrawing();
    }
    CloseWindow();
    return 0;

}


/*
            const char* dropbox = "Drag and Drop your image here"; 
            int droptextwidth = MeasureText(dropbox,20);
            DrawRectangleLines(90,400,1800,600,DARKGRAY)
            DrawText(dropbox,900-droptextwidth/2,600-10,10, GRAY);
            EndDrawing();
*/