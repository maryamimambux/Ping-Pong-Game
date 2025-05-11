//=============================================================

/*
OOP PROJECT - PING PONG GAME

Steps to develop Pong 
1. Create a blank screen & Game loop 
2. Draw the paddles and the ball 
3. Move the ball around 
4. Check for a collision with all edges 
5. Move the player's paddle 
6. Move the CPU paddle with Artificial Intelligence 
7. Check for a collision with the paddles 
8. Add scoring
9. Add sounds
*/

//=============================================================

#include <iostream>
#include <raylib.h>
#include <raymath.h> // raymath.h specifically includes math-related functions used for 2D and 3D graphics programming.

using namespace std;

Color Lighter_Blue = (Color){ 102, 178, 235, 240 }; 
Color Dark_Blue    = (Color){  21,  67,  96, 255 };
Color Light_Blue   = (Color){  41, 128, 185, 255 };   
Color Yellow       = (Color){ 243, 213, 91, 255 }; 
Color White        = (Color){ 255, 255, 255, 150 };


class Ball{
    private:
    float x, y;
    int speed_x, speed_y;
    int radius;

    public:

    // Default and Parameterized Constructor
    Ball (float x = 0.0, float y = 0.0, int speed_x = 0, int speed_y = 0, int radius = 1 ) 
    : x(x), y(y), speed_x(speed_x), speed_y(speed_y), radius(radius) {}

    // Getters
    float getX() { return x; }
    float getY() { return y; }
    int& getSpeedX() { return speed_x; }
    int& getSpeedY() { return speed_y; }
    int getRadius() { return radius; }

    void Draw() {
        // DrawCircle( int X, int Y, float radius, Color color);  
        DrawCircle(x, y, radius, Yellow);
        
    }

    void Update() {
        x += speed_x;
        y += speed_y;

        // Collision with top and bottom walls
        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }
        
    }

    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;
        int speed_choices[2] = { -1, 1 };
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];

        /*
        We have created an array of speed_choices[2] = { -1, 1 }
        that contains speed_choices[0] = -1 and speed_choices[1] = 1

        GetRandomValue(0, 1) choses random value i.e either 1 or 0
        e.g GetRandomValue(0, 1) returns 0 then speed_choices[0] = -1
            that means ball will move in -ve direction.

            GetRandomValue(0, 1) returns 1 then speed_choices[0] =  1
            that means ball will move in +ve direction.
        */
    }

};

class Paddle {
    protected:
    float x, y;
    float height, width;
    int speed;

    void LimitMovement() {
        // to Restrict PADDLE motion within the boundries of Screen
        if (y <= 0) { y = 0; }
        if (y + height >= GetScreenHeight()) { y = GetScreenHeight() - height; }
    }

    public:
    Paddle(float x = 0.0, float y = 0.0, float h = 0.0, float w = 0.0, int s = 0)
        : x(x), y(y), height(h), width(w), speed(s) {}

    // Getters
    float getX() { return x; }
    float getY() { return y; }
    int getHeight() { return height; }
    int getWidth() { return width; }
    int getSpeed() { return speed; }

    void Draw() {

        // DrawRectangleRounded(Rectangle{}, roundness, segment, color);
        DrawRectangleRounded (Rectangle{x, y, width, height}, 0.8, 0, WHITE);
 
    } 
    
    virtual void Update() {
        // to MOVE PADDLE UP and DOWN
        if (IsKeyDown(KEY_UP)) {
            y = y - speed; // subtracted because we want to move the paddle UP
        }
        if (IsKeyDown(KEY_DOWN)) {
            y = y + speed; // to move the paddle DOWN
        }

        LimitMovement();
        
    }
};

class CPU_Paddle : public Paddle {
    public:
    using Paddle::Update;
    CPU_Paddle(float x = 0.0, float y = 0.0, float h = 0.0, float w = 0.0, int s = 0) 
        : Paddle(x, y, h, w, s) {}

    // Using a "Tracking AI Algorithm" Makes CPU player hard to beat because it reacts perfectly.
    void Update(float ball_y) {
        y = ball_y - height / 2; // aligns the center of the paddle with the ball’s Y-position.
        LimitMovement();
    }
};

/*=============================================================

GAME LOOP

1. Check Events -> that happens in game like quitting the game, moving ball or paddle, scoring etc.

2. Update Position -> positions of all the game objects like ball , paddle etc. 

3. Draw the Game Objects in their positions

windowShouldClose() checks if escape key is pressed or
close icon of the window is pressed then it returns true
then the game loop ends, and the window is distroyed.

=============================================================*/

class Game{

    private:

    const float screen_width = 1000;
    const float screen_height = 600;
    int player_score;
    int cpu_score;

    // Object decleration
    Ball ball;
    Paddle player;
    CPU_Paddle cpu;

    // Sound --> Raylib's class
    Sound paddleHitSound;
    Sound wallHitSound;

    public:

    Game() :
        // Setting initial score to 0 for both players
        player_score(0), cpu_score(0),

        // Initializing Objects using Perametrized Constructors
        ball(screen_width / 2, screen_height / 2, 5, 5, 12),
        player(screen_width - 30, screen_height / 2 - 55, 110, 20, 4),
        cpu(10, screen_height / 2 - 55, 110, 20, 4) 
    {

        InitWindow(screen_width, screen_height, "Ping Pong Game - OOP Project - FAST NUCES");

        InitAudioDevice(); // Initialize audio device
        paddleHitSound = LoadSound("sounds/paddle.wav"); // LoadSound("Address of file.wav") Allocates internal buffers and stores sample data.  
        wallHitSound = LoadSound("sounds/wall.wav");

        /*
        It limits the frame rate to 60 frames per second.
        Without SetTargetFPS(no of frames) your game loop will run as fast as the hardware allows.
        */
        SetTargetFPS(60); 

    }

    void Run() { 
        while (!WindowShouldClose()) {
            Update();
            Draw(); 
        }
        CloseWindow(); // to DESTROY the Window
    }

    private:
    void Update() {
        ball.Update();
        player.Update();
        cpu.Update(ball.getY());

        // Checks for collision between a circle (the ball) and a rectangle (the paddle).
        if (CheckCollisionCircleRec(
            // Ball's center position
            Vector2{(float)(ball.getX()), (float)(ball.getY())}, 
            // Radius of the ball
            ball.getRadius(), 
            // Paddle's position and size
            Rectangle{player.getX(), player.getY(), (float)player.getWidth(), (float)player.getHeight()})) 
        {
            ball.getSpeedX() *= -1;
            PlaySound(paddleHitSound);
        }

        // Same logic as above but for CPU's paddle
        if (CheckCollisionCircleRec(
            Vector2{(float)(ball.getX()), (float)(ball.getY())}, 
            ball.getRadius(), 
            Rectangle{cpu.getX(), cpu.getY(), (float)cpu.getWidth(), (float)cpu.getHeight()})) 
        {
            ball.getSpeedX() *= -1;
            PlaySound(paddleHitSound);
        }

        // Ball Hits the Right Wall
        if (ball.getX() + ball.getRadius() >= screen_width) {
                cpu_score++;
                ball.ResetBall();
                PlaySound(wallHitSound);
        }

        // Ball Hits the Left Wall
        if (ball.getX() - ball.getRadius() <= 0) {
                player_score++;
                ball.ResetBall(); // Brings the ball back to the starting position (at the centre of game window)
        }
    }
    void Draw() {
        BeginDrawing(); // Creates a Blank Canvas
        ClearBackground(Dark_Blue); // to erase the trace of previously drawn object

        // Background half
        // DrawRectangle(int X, int Y, int width, int height, Color color);
        DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Light_Blue);

        // DrawCircle( int X, int Y, float radius, Color color);  
        DrawCircle(screen_width / 2, screen_height / 2, 125, Lighter_Blue);

        // DrawLine(int startX, int startY, int endX, int endY, Color color);
        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, White);

        // Draw game objects
        player.Draw();
        cpu.Draw();
        ball.Draw();

        // Draw scores
        // void DrawText((text to draw), int posX, int posY, int fontSize, Color color);
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 15, 20, 70, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 15, 20, 70, WHITE);

        EndDrawing(); // Important to finalize drawing
    }

    public:

    ~Game() {

        /*
        It frees the memory and resources that were allocated when you called LoadSound("file.wav").
        Closes one audio file
        Otherwise can create memory leaks.
        */
        UnloadSound(paddleHitSound);
        UnloadSound(wallHitSound);
        /*
        Shuts down your entire speaker system — nothing will play anymore after this.
        Automatically frees all loaded sounds/music even if you didn’t call UnloadSound().
        After calling it, no sound functions will work until you call InitAudioDevice() again.
        */
        
        CloseAudioDevice();

    }

};


int main() {
    Game pongGame;   
    pongGame.Run();  
    return 0;
}