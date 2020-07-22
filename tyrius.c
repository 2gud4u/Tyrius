/*
TYRIUS - Open Source FPS game written in C using Raylib
Written by Rabia Alhaffar In 25/April/2020
Last update: 17/July/2020
See LICENSES.txt for 3rd party licenses
NOTES: Most of commented code are for next versions of the game
*/
//Libs imported
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "rlgl.h"
#include "raymath.h"
#include "camera.h"
#define GRASSRECTS 10000
#define RAINBOWRECTS 10000
#define LINES 10000

// Functions
void UnloadResources(int m);
void Splashscreen();
void Menu();
void ModeMenu();
void FreeMode();
void Options();
void SecondOptions();
void Tutorial();
void Training();
void Missions();
void GameOver();
void Credits();
void ControlsMenu();
void DrawEnvironment(int e, int n);
void DrawWeapon(int w);
void CombatLogic();
void CheckCollisionEnvironment(int ce);
void DrawVerstaile();
void GenerateEnvironments();
void Draw3DDefaults();
void GameGUI();
void CameraLogic();
void CheckCollisionWalls();
void CheckCollisionBulletTargets();

//Variables
int screenshots_num = 0;
const char *screenshot_name = "tyrius_screenshot.png";
typedef enum storage { highscore = 0,mission = 0 };
const char *version = "v0.1.9 BETA";
int score = 0;
int bullets = 10;
int reload_bullets = 100;
int bombs = 3;
int scene = 1; // Current scene
int mainfps = 60;
int timer = 0;
bool options_button = false;
bool start_game_button = false;
bool credits_button = false;
bool freemode = false;
bool freemode_enabled = false;
bool training = false;
bool missions = false;
bool tutorial = false;
bool mute = false;
bool exitgame = false;
bool credits = false;
bool explode_bomb = false;
bool destroy_colliders = false;
bool animate_armor = false;
bool armor_started = false;
bool running = false;
float rot_x = 0.0f;
float rot_y = 0.0f;
float rot_z = 0.0f;
float ang_x = 0.0f;
float ang_y = 0.0f;
float ang_z = 0.0f;
int tutorial_targets = 3;
int training_targets = 3;

//Environments
Vector3 grassrects_pos[GRASSRECTS];
float grassrects_sizes[GRASSRECTS];
Vector3 rainbowrects_pos[RAINBOWRECTS];
Color rainbowrects_colors[RAINBOWRECTS];
float rainbowrects_sizes[RAINBOWRECTS];
Vector3 lines_pos[LINES];
Vector3 lines_pos2[LINES];
Color lines_colors[LINES];
bool ObjectsDestroyed[100000];
double health = 100; //In hard combat mode,Turn this to 10
float aimag = 1.5f;
float envflat = 1.0f;
int objects_hitted = 0;

//Resources
Image rayimg;
Image gameimg;
Image point;
Image cross;
Image target_img;
Sound shot;
Sound change;
Sound reload;
Sound walk;
Sound explosion;
Texture2D raylogo;
Texture2D gamelogo;
Texture2D pointer;
Texture2D crosshair;
Texture2D target;
Font f1;
Font f2;
Font f3;
Camera gamecam;  
Color backcolor = RAYWHITE;
Vector3 ppos;


int env;
int nightmode;
bool jumping = false;


double bullet_x = 2.0f;
double bomb_x = 10.0f;
double armorsize = 0.0f;
double explosion_radius = 0.0f;
bool bullet_to_move = false;
bool bomb_to_throw = false;
bool do_bomb_explosion = false;


const char *quotes[18] = {
    "New Game,Cool!!!",
    "Why Dude? Why Modding?",
    "Spin The Mouse Wheel",
    "Ahhhhhhhhhhhhhhh",
    "BOOOOOOOOOOOOOM",
    "Are You Ready?",
    "Heeeeeeeeeeeeeeeeeeeeeeeeelp",
    "Bomb: Throw Me!!!",
    "You Are Not Idiot!!!",
    "Just Shoot And Run",
    "Always Watch Your Back",
    "Follow Me",
    "Enemy Spotted!!!",
    "This Game Is Loading...",
    "Slow And Steady",
    "Go Go Go",
    "Supply Dropped!!!",
    "Anger Level: 100%"
};

const char *tutorial_texts[12] = {
    "Welcome To Tyrius!!!",
    "Move The Mouse To Move Game Camera!!!",
    "Move Using WASD",
    "Shoot Using Left Mouse Button",
    "Reload Using R Key,Try It",
    "Throw Grenade Using Right Mouse Button",
    "Aim With Mouse Wheel Button(Click It)",
    "What Do You See?,A Laser!!!",
    "Press Q Key To Use Armor And Keep It Pressed!!!",
    "Impressing Right?",
    "Use 0 Number Key To Take A Screenshot",
    "So This Is Everything Until Now!!!",
    "Happy Shooting!!!"
};

int tutorial_part = 0;

const char *quote;

int main(void)
{
    // Set flags and enable Antialiasing and VSync
    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    TraceLog(LOG_INFO,"INITIALIZING...\n");
    
    InitWindow(GetScreenWidth(),GetScreenHeight(),"TYRIUS");
    InitAudioDevice();
    TraceLog(LOG_INFO,"LOADING GAME CONTENT...\n");
    env = GetRandomValue(1,4);
    nightmode = GetRandomValue(1,4);
    ppos = (Vector3){ 10.0f,4.0f,40.0f };
    quote = quotes[GetRandomValue(0,17)];
    if(IsWindowReady()) {
        SetWindowPosition(0,0);
        SetTargetFPS(mainfps);
        SetTextureFilter(GetFontDefault().texture, FILTER_POINT); // Fix for HighDPI display problems
        ToggleFullscreen();
        TraceLog(LOG_INFO,"GAME LAUNCHED SUCCESSFULLY!!!\n");
    }
    
    if((!IsWindowReady()) || (IsWindowMinimized())) {
        TraceLog(LOG_ERROR,"GAME FAILED TO LAUNCH,CLOSING GAME...\n");
        CloseAudioDevice();
        UnloadResources(2);
        CloseWindow();
    }
    
    GenerateEnvironments();
    
      
    TraceLog(LOG_INFO,"LOADING RESOURCES...\n");
    
    rayimg = LoadImage("rsc/tex/raylib_logo.png");
    gameimg = LoadImage("rsc/tex/tyrius_logo.png");
    cross = LoadImage("rsc/tex/Crosshair 1.png");
    target_img = LoadImage("rsc/tex/target.png");
    
    raylogo = LoadTextureFromImage(rayimg);
    gamelogo = LoadTextureFromImage(gameimg);
    crosshair = LoadTextureFromImage(cross);
    target = LoadTextureFromImage(target_img); //Here We Go ;)
    
    f1 = LoadFontEx("rsc/ttf/Michroma.ttf",32,0,250);
    f2 = LoadFontEx("rsc/ttf/Jura Bold 700.ttf",32,0,250);
    f3 = LoadFontEx("rsc/ttf/Saira-Bold.ttf",32,0,250);
    
    shot = LoadSound("rsc/aud/380_gunshot_single-mike-koenig.mp3");
    change = LoadSound("rsc/aud/go.wav");
    reload = LoadSound("rsc/aud/387168__rdaly95__reloading-ammo.wav");
    walk = LoadSound("rsc/aud/483924__zeinel__walking-on-grass.mp3");
    explosion = LoadSound("rsc/aud/172334__knova__grenade-knova.wav");
    
    UnloadResources(1);
    TraceLog(LOG_INFO,"RESOURCES LOADED SUCCESSFULLY!!!\n");
    
    TraceLog(LOG_INFO,"CREATING GAME CAMERA...\n");
    Camera camera = { 0 };
    camera.position = ppos;
    camera.target = (Vector3){ 0.0f, 0.0f, 20.0f };
    camera.up = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.fovy = 90.0f;
    camera.type = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    TraceLog(LOG_INFO,"CAMERA CREATED SUCCESSFULLY!!!\n");
    
    
    while(!WindowShouldClose())
    {   
                   
            TraceLog(LOG_DEBUG,"UPDATING THE GAME...\n");
            gamecam = camera;
            UpdateCamera(&camera);
            UpdateCamera(&gamecam);
            ppos = gamecam.position;            
            
            if(scene == 1) Splashscreen();
            if(scene == 2) Menu();
            if(scene == 3) Options();
            if(scene == 4) SecondOptions();
            if(scene == 5) ControlsMenu();
            if(scene == 6) Credits();
            if(scene == 7) ModeMenu();
            if(scene == 8) Tutorial();
            if(scene == 9) Training();
            if(scene == 10) Missions();
            if(scene == 11) FreeMode();
            if(scene == 12) GameOver();
            if(scene == 9 || scene == 10 || scene == 11) HideCursor();
            if(scene > 0) TraceLog(LOG_INFO,"SWITCHING BETWEEN GAME SCENES...\n");
            
            
            if(IsKeyDown(KEY_ZERO)) {
                ChangeDirectory("screenshots");
                TraceLog(LOG_INFO,"TAKING SCREENSHOT...\n");
                if(FileExists(FormatText("tyrius_screenshot_%i.png",screenshots_num))) screenshots_num++;
                if(!FileExists(FormatText("tyrius_screenshot_%i.png",screenshots_num))) {
                    TakeScreenshot(FormatText("tyrius_screenshot_%i.png",screenshots_num));
                    TraceLog(LOG_INFO,"SCREENSHOT TAKEN SUCCESSFULLY!!!\n");                
                    screenshots_num++;
                }
            }
            TraceLog(LOG_DEBUG,"DRAWING FPS ON SCREEN...\n");
    }        
    
    
    TraceLog(LOG_INFO,"CLOSING GAME...\n");
    CloseAudioDevice();
    UnloadResources(2);
    CloseWindow();
    return 0;
}

void UnloadResources(m) {
    TraceLog(LOG_INFO,"UNLOADING RESOURCES...\n");
    if(m == 1) {
        UnloadImage(rayimg);
        UnloadImage(gameimg);
        UnloadImage(cross);
        UnloadImage(target_img);
    }
    if(m == 2) {
        UnloadTexture(raylogo);
        UnloadTexture(gamelogo);
        UnloadTexture(crosshair);
        UnloadTexture(target);
        UnloadSound(shot);
        UnloadSound(change);
        UnloadSound(reload);
        UnloadSound(walk);
        UnloadSound(explosion);
        UnloadFont(f1);
        UnloadFont(f2);
        UnloadFont(f3);
    }
    TraceLog(LOG_INFO,"RESOURCES UNLOADED SUCCESSFULLY!!!\n");
}

void Splashscreen() {
    TraceLog(LOG_INFO,"SPLASHSCREEN STARTED SUCCESSFULLY!!!\n");
    BeginDrawing();
        ClearBackground(backcolor);
        DrawFPS(10,10);
        DrawText("Made With",GetScreenWidth() / 3 + 96,GetScreenHeight() / 3 - 45,48,BLACK);
        DrawTexture(raylogo,GetScreenWidth() / 3 + 120,GetScreenHeight() / 3 + 45,WHITE);
        timer++;
        if(timer > 60) {
            timer = 0;
            scene = 2;
            TraceLog(LOG_DEBUG,"SWITCHING TO MAIN MENU...\n");
            PlaySound(change);
        }
        DrawTextureEx(crosshair,(Vector2){ GetMouseX() - 20,GetMouseY() - 20 },0.0f,0.1f,DARKPURPLE);
    EndDrawing();
}

void Menu() {
    TraceLog(LOG_INFO,"MAIN MENU STARTED SUCCESSFULLY!!!\n");
    UpdateCamera(&gamecam);
    ppos = gamecam.position;
    BeginDrawing();  
        ClearBackground(backcolor);         
        
        BeginMode3D(gamecam);
            
            //DrawBillboard(gamecam,gamelogo,(Vector3){ 0.0f,10.0f,0.0f },64,WHITE);
            //Draw3DDefaults(env,nightmode,ppos);           
            DrawGrid(1000,1);
            TraceLog(LOG_DEBUG,"LOADING GAME WEAPONS...\n");
            DrawWeapon(1);            
            CombatLogic();            
        EndMode3D();
        
        DrawFPS(10,10);
        TraceLog(LOG_DEBUG,"LOADING MAIN MENU GUI...\n");
        start_game_button = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 125,250,100 },"");
        options_button = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 250,250,100 },"");
        exitgame = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 375,250,100 },"");
    
        
        DrawTextEx(f2,"START GAME",(Vector2){ GetScreenWidth() / 3 + 136,GetScreenHeight() / 3 + 157 },32,1.0f,BLACK);
        DrawTextEx(f2,"OPTIONS",(Vector2){ GetScreenWidth() / 3 + 165,GetScreenHeight() / 3 + 282 },32,1.0f,BLACK);
        DrawTextEx(f2,"EXIT",(Vector2){ GetScreenWidth() / 3 + 195,GetScreenHeight() / 3 + 407 },32,1.0f,BLACK);       
        DrawVerstaile();
        

        if(start_game_button) {
            scene = 7;
            TraceLog(LOG_DEBUG,"STARTING THE GAME...\n");
        }
        if(options_button) {
            scene = 3;
            TraceLog(LOG_DEBUG,"SWITCHING TO GAME OPTIONS MENU...\n");
        }
        if(exitgame) {
            TraceLog(LOG_INFO,"CLOSING GAME...\n");
            CloseAudioDevice();
            UnloadResources(2);
            CloseWindow();
        }
        if(start_game_button || options_button) PlaySound(change);    
        TraceLog(LOG_INFO,"MAIN MENU CONTENT LOADED SUCCESSFULLY!!!\n");
    EndDrawing();
}

void ModeMenu() {
    UpdateCamera(&gamecam);
    ppos = gamecam.position;
    BeginDrawing();
        ClearBackground(backcolor);
        TraceLog(LOG_INFO,"MODE MENU STARTED SUCCESSFULLY!!!\n");
        TraceLog(LOG_DEBUG,"LOADING MODE MENU GUI...\n");
        BeginMode3D(gamecam);   
            //Draw3DDefaults(env,nightmode,ppos); 
            DrawGrid(1000,1);          
        EndMode3D();        
        tutorial = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 105,250,80 },"");
        training = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 205,250,80 },"");
        freemode = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 305,250,80 },"");
        missions = GuiButton((Rectangle){ GetScreenWidth() / 3 + 100,GetScreenHeight() / 3 + 405,250,80 },"");              
        DrawTextEx(f2,"TUTORIAL",(Vector2){ GetScreenWidth() / 3 + 156,GetScreenHeight() / 3 + 127 },32,1.0f,BLACK);
        DrawTextEx(f2,"TRAINING",(Vector2){ GetScreenWidth() / 3 + 156,GetScreenHeight() / 3 + 227 },32,1.0f,BLACK);
        DrawTextEx(f2,"FREEMODE",(Vector2){ GetScreenWidth() / 3 + 156,GetScreenHeight() / 3 + 327 },32,1.0f,BLACK);
        DrawTextEx(f2,"MISSIONS",(Vector2){ GetScreenWidth() / 3 + 156,GetScreenHeight() / 3 + 427 },32,1.0f,BLACK);
        DrawTextEx(f2,"BACK: BACKSPACE",(Vector2){ 10.0f,GetScreenHeight() - 80.0f },32,1.0,BLACK);
        TraceLog(LOG_INFO,"MODE MENU GUI LOADED SUCCESSFULLY!!!\n");
        
        DrawFPS(10,10);
        DrawVerstaile();          
        
        if(tutorial) scene = 8;
        if(training) scene = 9;
        if(missions) scene = 10;
        if(freemode) scene = 11;
        if(tutorial || training || missions || freemode){
            PlaySound(change);
            bullets = 10;
            reload_bullets = 100;
            bombs = 3;
            health = 100;
            tutorial_part = 0;
            timer = 0;
            env = GetRandomValue(1,4);
            nightmode = GetRandomValue(1,4);
        }            
        if(IsKeyPressed(KEY_BACKSPACE)) {
            scene = 2;
            freemode_enabled = false;
        }
        
    EndDrawing();
}

void FreeMode() {
    UpdateCamera(&gamecam);
    CameraLogic();
    BeginDrawing();
        ClearBackground(backcolor);         
        
        BeginMode3D(gamecam);
            //DrawBillboard(gamecam,gamelogo,(Vector3){ 0.0f,10.0f,0.0f },64,WHITE);
            Draw3DDefaults(env,nightmode,ppos);
            DrawWeapon(1);            
            CombatLogic();
        EndMode3D();            
        GameGUI();
        DrawFPS(10,10);
        //DrawTextureEx(crosshair,(Vector2){ GetMouseX() - 20,GetMouseY() - 80 },0.0f,0.1f,DARKPURPLE);
        DrawTextEx(f2,"BACK: BACKSPACE",(Vector2){ 10.0f,GetScreenHeight() - 120.0f },32,1.0f,BLACK);
        if(IsKeyPressed(KEY_BACKSPACE)) scene = 2;
    EndDrawing();    
    TraceLog(LOG_INFO,"FREEMODE STARTED SUCCESSFULLY!!!\n");
}

void Options() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
    EndDrawing();
}

void SecondOptions() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
    EndDrawing();

}

void Tutorial() {
    UpdateCamera(&gamecam);
    CameraLogic();
    BeginDrawing();
        ClearBackground(backcolor);         
        
        BeginMode3D(gamecam);
            
            //DrawBillboard(gamecam,gamelogo,(Vector3){ 0.0f,10.0f,0.0f },64,WHITE);
            Draw3DDefaults(env,nightmode,ppos);
            DrawWeapon(1);            
            CombatLogic();
        EndMode3D();            
        GameGUI();
        DrawRectangle(100,10,800,100,BLUE);
        DrawRectangleLines(100,10,800,100,BLACK);      
        DrawFPS(10,10);
        timer++;        
        if(timer == 60) tutorial_part++;
        if(timer >= 150 * tutorial_part) tutorial_part++;
        if(tutorial_part > 13) scene = 2;
        //DrawTextureEx(crosshair,(Vector2){ GetMouseX() - 20,GetMouseY() - 80 },0.0f,0.1f,DARKPURPLE);
        DrawTextEx(f2,"BACK: BACKSPACE",(Vector2){ 10.0f,GetScreenHeight() - 120.0f },32,1.0f,BLACK);
        DrawTextEx(f2,tutorial_texts[tutorial_part],(Vector2){ 140.0f,35.0f },32,1.0f,BLACK);   
        if(IsKeyPressed(KEY_BACKSPACE)) scene = 2;
    EndDrawing();    
    TraceLog(LOG_INFO,"TUTORIAL STARTED SUCCESSFULLY!!!\n");
}

void Training() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
    EndDrawing();
    
}

void Missions() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
    EndDrawing();
    
}

void GameOver() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
    EndDrawing();
}


void Credits() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
    EndDrawing();
    
}

void ControlsMenu() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10,10);
    EndDrawing();
}

void DrawEnvironment(int e,int n) {
    DrawSphere((Vector3){ 80.0f,60.0f,50.0f },10.0f,YELLOW);
    if(n == 1) backcolor = RAYWHITE;
    if(n == 2) backcolor = BLACK;
    if(n == 3) backcolor = GRAY;
    if(n == 4) backcolor = ORANGE;
    if(e == 1) {      
        DrawPlane((Vector3) { 0.0f,-0.4f,0.0f },(Vector2){ 100000.0f,100000.0f },GREEN);
        for(int i = 0;i < GRASSRECTS;i++) {
            if(!ObjectsDestroyed[i]) DrawCube(grassrects_pos[i],0.2f,grassrects_sizes[i],0.2f,LIME);
        }
    }
    if(e == 2) {      
        for(int i = 0;i < RAINBOWRECTS;i++) {
            if(!ObjectsDestroyed[i]) {
                DrawCube(rainbowrects_pos[i],rainbowrects_sizes[i],rainbowrects_sizes[i],rainbowrects_sizes[i],rainbowrects_colors[i]);
                DrawCubeWires(rainbowrects_pos[i],rainbowrects_sizes[i],rainbowrects_sizes[i],rainbowrects_sizes[i],BLACK);
            }
        }
    }
    if(e == 3) {
        for(int i = 0;i < LINES;i++) {
            if(!ObjectsDestroyed[i]) DrawLine3D(lines_pos[i],lines_pos2[i],lines_colors[i]); 
        }
    }
    else { return 0; }
}

void DrawWeapon(int w) {
    TraceLog(LOG_DEBUG,"DRAWING WEAPON...\n");
    if(w == 1) {
        DrawCube(Vector3Add(gamecam.target,(Vector3){ 3.0f,aimag - 0.5f,0.0f }),1.8f,1.0f,1.0f,BLACK);
        DrawCubeWires(Vector3Add(gamecam.target,(Vector3){ 3.0f,aimag - 0.5f,0.0f }),1.8f,1.0f,1.0f,GRAY);
        DrawCube(Vector3Add(gamecam.target,(Vector3){ 2.5f,aimag,0.0f }),3.0f,0.5f,0.5f,DARKPURPLE);
        DrawCubeWires(Vector3Add(gamecam.target,(Vector3){ 2.5f,aimag,0.0f }),3.0f,0.5f,0.5f,BLACK);
    }
}

void CombatLogic() {
    if(IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
        TraceLog(LOG_DEBUG,"AIMING...\n");
        DrawLine3D(Vector3Add(gamecam.target,(Vector3){ 2.0f,aimag,0.0f }),Vector3Add(gamecam.target,(Vector3){ -20.0f,1.5f,0.0f }),RED);
    }
    if(env != 3) DrawCubeWires(ppos,0.1f,0.1f,0.1f,BLACK);
    DrawCube(Vector3Add(gamecam.target,(Vector3){ bullet_x,aimag,0.0f }),1.0f,0.2f,0.2f,RED);
    DrawCubeWires(Vector3Add(gamecam.target,(Vector3){ bullet_x,aimag,0.0f }),1.0f,0.2f,0.2f,BLACK);
    DrawSphere(Vector3Add(gamecam.target,(Vector3){ bomb_x,aimag,0.0f }),0.5f,BLACK); //Bomb
    DrawCube(Vector3Add(ppos,(Vector3){ -5.0f,0.0f,0.0f }),armorsize,1.0f,armorsize,DARKPURPLE);
    DrawCubeWires(Vector3Add(ppos,(Vector3){ -5.0f,0.0f,0.0f }),armorsize,1.0f,armorsize,BLACK);
    if(bullet_to_move) bullet_x -= 1.0f;
    if(bomb_to_throw) bomb_x -= 3.0f;
    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && bullets > 0 && bullet_x == 2.0f) {
        PlaySound(shot);
        TraceLog(LOG_DEBUG,"SHOOTING...\n");
        if(!(scene == 2 || scene == 3)) bullets -= 1;
        bullet_to_move = true;   
    }
    if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && bombs > 0 && bomb_x == 10.0f) {
        TraceLog(LOG_DEBUG,"THROWING GRENADE...\n");
        if(!(scene == 2 || scene == 3)) bombs--;
        bomb_to_throw = true;
    }
    if(bullet_x <= -10.0f) {
        TraceLog(LOG_DEBUG,"RESETTING BULLET POSITION...\n");
        bullet_to_move = false;
        bullet_x = 2.0f;
    }
    if(bullets <= 0 && reload_bullets > 0) {
        
        PlaySound(reload);
        TraceLog(LOG_DEBUG,"RELOADING...\n");
        bullets += 10;
        reload_bullets -= 10;
    }
    
    if(IsKeyPressed(KEY_R) && reload_bullets <= 100 && !reload_bullets <= 0) {
        PlaySound(reload);
        TraceLog(LOG_DEBUG,"RELOADING...\n");
        reload_bullets -= (10 - bullets);
        bullets = 10;
    }
    
    if(bomb_x <= -20.0f) {
        TraceLog(LOG_DEBUG,"EXPLOSION OCCURED...\n");
        bomb_x = 10.0f;
        PlaySound(explosion);
        bomb_to_throw = false;
        do_bomb_explosion = true;
    }
    if(do_bomb_explosion) {
        if(explosion_radius >= 10.0f) {
            do_bomb_explosion = false;
            explosion_radius = 0.0f;
        }
        DrawSphere(Vector3Add(gamecam.target,(Vector3){ bomb_x - 50.0f,0.0f,0.0f }),explosion_radius,YELLOW); //Bomb Explosion
        explosion_radius += 1.0f;
    }
    
    if(bullet_to_move || bullet_x <= -100.0f && env != 4) CheckCollisionEnvironment(env);
    
    if(IsKeyDown(KEY_Q)) armor_started = true;
    if(armor_started) animate_armor = true;
    if(animate_armor) {
        armorsize += 0.3f;
        if(armorsize >= 3.0f && !IsKeyDown(KEY_Q)) {
            armor_started = false;
            animate_armor = false;
            armorsize = 0.0f;
        }
        if(armorsize < 0.0f) armorsize = 0.0f;
        if(armorsize >= 3.0f && IsKeyDown(KEY_Q)) armorsize = 3.0f;
    }
    if(IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D)) {
        timer++;
        if(timer >= 120) {
            if(env == 1) PlaySound(walk);
            timer = 0;
        }
    }
}




//https://gamedev.stackexchange.com/questions/60505/how-to-check-for-cube-collisions/60514
void CheckCollisionEnvironment(int ce) {
    //Grass
    if(ce == 1) {
        for(int c = 0;c < 100000;c++) {
            //Bullet
            if(abs(grassrects_pos[c].x - Vector3Add(gamecam.target,(Vector3){ bullet_x,1.5f,0.0f }).x) < 0.2f + 1.0f) {
                if(abs(grassrects_pos[c].y - 1.5f) < grassrects_sizes[c] + 0.2f) {
                    if(abs(grassrects_pos[c].z - 0.0f) < 0.2f + 0.2f) {
                        if(destroy_colliders && bullet_x <= -30.0f) {
                            ObjectsDestroyed[c] = true;
                            // Soon...
                        }
                        return true;
                    }
                }       
            }       
        }
        
    }
    
    if(ce == 2) {
        for(int c = 0;c < 100000;c++) {
            if(abs(rainbowrects_pos[c].x - Vector3Add(gamecam.target,(Vector3){ bullet_x,1.5f,0.0f }).x) < rainbowrects_sizes[c] + 1.0f) {
                if(abs(rainbowrects_pos[c].y - 1.5f) < rainbowrects_sizes[c] + 0.2f) {
                    if(abs(rainbowrects_pos[c].z - 0.0f) < rainbowrects_sizes[c] + 0.2f) {
                        if(destroy_colliders) {
                            ObjectsDestroyed[c] = true;
                            // Soon...
                        }
                        return true;
                    }
                }       
            }       
        }
    }
}

void DrawVerstaile() {
    DrawTextureEx(gamelogo,(Vector2){ GetScreenWidth() / 7,10.0f },0.0f,0.75,WHITE);
    DrawText(version,GetScreenWidth() - 200,GetScreenHeight() - 40,32,backcolor);
    DrawText(quote,10,GetScreenHeight() - 40,32,BLACK);
    DrawTextureEx(crosshair,(Vector2){ GetMouseX() - 20,GetMouseY() - 20 },0.0f,0.1f,DARKPURPLE);
}

void GenerateEnvironments() {
    TraceLog(LOG_DEBUG,"GENERATING ENVIRONMENTS...\n");
    for(int i = 0;i < GRASSRECTS;i++) {    
        grassrects_pos[i] = (Vector3) { GetRandomValue(-50.0f,50.0f),2.0f,GetRandomValue(-50.0f,50.0f) };
        grassrects_sizes[i] = GetRandomValue(1,2);           
    }
    for(int i = 0;i < RAINBOWRECTS;i++) {
        rainbowrects_colors[i] = (Color) { GetRandomValue(0,265),GetRandomValue(0,256),GetRandomValue(0,256),255 };
        rainbowrects_pos[i] = (Vector3) { GetRandomValue(-1000.0f,1000.0f),2.0f,GetRandomValue(-1000.0f,1000.0f) };
        rainbowrects_sizes[i] = GetRandomValue(1,10);
    }
    for(int i = 0;i < LINES;i++) {
        lines_colors[i] = (Color) { GetRandomValue(0,265),GetRandomValue(0,256),GetRandomValue(0,256),255 };
        lines_pos[i] = (Vector3) { GetRandomValue(-1000.0f,1000.0f),GetRandomValue(-1000.0f,1000.0f),GetRandomValue(-1000.0f,1000.0f) };
        lines_pos2[i] = (Vector3) { GetRandomValue(-1000.0f,1000.0f),GetRandomValue(-1000.0f,1000.0f),GetRandomValue(-1000.0f,1000.0f) };
    }
    for(int t = 0;t < 100000;t++) ObjectsDestroyed[t] = false;
    TraceLog(LOG_INFO,"ENVIRONMENTS GENERATED SUCCESSFULLY!!!\n");
}

void Draw3DDefaults(int e,int n,Vector3 p) {
    TraceLog(LOG_DEBUG,"STARTING 3D CONTENT...\n");
    DrawGrid(1000,1);
    TraceLog(LOG_DEBUG,"DRAWING GRID...\n");
    DrawEnvironment(e,n);
    TraceLog(LOG_DEBUG,"STARTING ENVIRONMENT CONTENT...\n");
    DrawCube(p,1.0f,1.0f,1.0f,BLACK);           
    TraceLog(LOG_DEBUG,"LOADING GAME WEAPONS...\n");
}

void GameGUI() {
    TraceLog(LOG_DEBUG,"DRAWING GAME GUI...\n");
    DrawRectangle(GetScreenWidth() - 340,GetScreenHeight() - 110,200,90,BLUE);
    DrawRectangle(GetScreenWidth() - 120,GetScreenHeight() - 80,100,60,BLUE);
    DrawRectangle(GetScreenWidth() - 120,GetScreenHeight() - 160,100,60,RED);
    DrawRectangle(GetScreenWidth() - 419,10,health * 4,18,GREEN);
    DrawRectangleLines(GetScreenWidth() - 420,10,(health * 4) + 2,20,BLACK);
    DrawRectangleLines(GetScreenWidth() - 340,GetScreenHeight() - 110,200,90,BLACK);
    DrawRectangleLines(GetScreenWidth() - 120,GetScreenHeight() - 80,100,60,BLACK);
    DrawRectangleLines(GetScreenWidth() - 120,GetScreenHeight() - 160,100,60,BLACK);
    DrawTextEx(f1,FormatText("%i",bullets),(Vector2) { GetScreenWidth() - 290.0f,GetScreenHeight() - 120.0f },96,1.0f,RAYWHITE);
    DrawTextEx(f1,FormatText("%i",reload_bullets),(Vector2) { GetScreenWidth() - 112.0f,GetScreenHeight() - 75.0f },48,1.0f,RAYWHITE);
    DrawTextEx(f1,FormatText("%i",bombs),(Vector2) { GetScreenWidth() - 85.0f,GetScreenHeight() - 160.0f },48,1.0f,RAYWHITE);
    TraceLog(LOG_INFO,"GAME GUI DRAWING SUCCESSFULLY!!!\n");
}

void CameraLogic() {
    TraceLog(LOG_DEBUG,"DOING GAME CAMERA LOGIC...\n");
    /*
    gamecam.up.x += ang_x;
    gamecam.up.y += ang_y;
    gamecam.up.z += ang_z;
    gamecam.target.x += rot_x;
    gamecam.target.y += rot_y;
    gamecam.target.z += rot_z;
    if(IsKeyDown(KEY_UP)) { rot_x -= 1.0f; }
    if(IsKeyDown(KEY_DOWN)) { rot_x += 1.0f; }
    if(IsKeyDown(KEY_Q)) { rot_y += 1.0f; }
    if(IsKeyDown(KEY_E)) { rot_y -= 1.0f; }
    if(IsKeyDown(KEY_R)) { rot_z += 1.0f; }
    if(IsKeyDown(KEY_F)) { rot_z -= 1.0f; }
    if(IsKeyDown(KEY_Z)) { ang_x += 1.0f; }
    if(IsKeyDown(KEY_X)) { ang_x -= 1.0f; }
    if(IsKeyDown(KEY_C)) { ang_y += 1.0f; }
    if(IsKeyDown(KEY_V)) { ang_y -= 1.0f; }
    if(IsKeyDown(KEY_G)) { ang_z += 1.0f; }
    if(IsKeyDown(KEY_T)) { ang_z -= 1.0f; }
    */
    TraceLog(LOG_INFO,"GAME CAMERA LOGIC RUNNING SUCCESSFULLY!!!\n");
}

// Soon...
void CheckCollisionWalls() {}
void CheckCollisionBulletTargets() {}