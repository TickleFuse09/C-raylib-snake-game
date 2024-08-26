#include<raylib.h>
#include<deque> 
#include<raymath.h>
#include<fstream>
#include<vector>

using namespace std;

Color green = Color{173,204,96,255};
Color dark_green = Color{43,51,24,255};

double lastupdatetime = 0;
int cell_size = 30;
int cell_count = 25;
float offset = 75;
int score = 0;

bool overlapbody(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i =0; i<deque.size();i++)
    {
        if(Vector2Equals(deque[i],element))
        {
            return true;
        }
    }
    return false;
}

bool event_trigger(double interval)
{
    double current_time = GetTime();
    if(current_time - lastupdatetime > interval)
    {
        lastupdatetime = current_time;
        return true;
    }
    return false;
}

class Food{

    public:
    Vector2 position;
    Texture2D texture;

    Food(deque<Vector2> snakebody)
    {
        Image image = LoadImage("assets/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = getrandomposition(snakebody);
    }
    ~Food()
    {
        UnloadTexture(texture);
    }

    void draw(){
        DrawTexture(texture,offset+ position.x*cell_size,offset+position.y*cell_size,WHITE);
    }

    Vector2 getrandomcell()
    {
        float x = GetRandomValue(0,cell_count-1);
        float y = GetRandomValue(0,cell_count-1);
        return Vector2{x,y};
    }

    Vector2 getrandomposition(deque<Vector2> snakebody)
    {
        Vector2 position = getrandomcell();
        while(overlapbody(position,snakebody))
        {
            position = getrandomcell();
        }
        return position;
    }
};

class Snake{
    public:

    deque<Vector2> body = {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
    Vector2 direction = {1,0};
    bool addsegment = false;

    void draw()
    {
        for(unsigned int i=0;i<body.size();i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            DrawRectangleRounded(Rectangle{offset+x*cell_size,offset+y*cell_size,(float)cell_size,(float)cell_size},0.5,6,dark_green);
        }
    }

    void update()
    {
        body.push_front(Vector2Add(body[0],direction));
        if(addsegment == true)
        {
            addsegment = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void reset()
    {
        body ={Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        direction = {1,0};
        score = 0;
    }

};

class Game{
public:
    Food food = Food(snake.body);
    Snake snake = Snake();
    bool running = true;

    void draw()
    {
        food.draw();
        snake.draw();
    }

    void update()
    {
        if(running)
        {
            snake.update();
            eatfood();
            collisionwithedges();
            collisionwithtail();
        }
    }

    void eatfood()
    {
        if(Vector2Equals(snake.body[0],food.position))
        {
            food.position = food.getrandomposition(snake.body);
            snake.addsegment = true;
            score++;
        }
    }

    void collisionwithedges()
    {
        if(snake.body[0].x == cell_count || snake.body[0].x == -1)
        {
            Gameover();
        }
        if(snake.body[0].y == cell_count || snake.body[0].y == -1)
        {
            Gameover();
        }
    }

    void Gameover()
    {
        snake.reset();
        food.position = food.getrandomposition(snake.body);
        running = false;
    }

    void collisionwithtail()
    {
        deque<Vector2> headlessbody = snake.body;
        headlessbody.pop_front();
        headlessbody.pop_front();
        if(overlapbody(snake.body[0],headlessbody))
        {
            Gameover(); 
        }
    }
};

int main()
{
    InitWindow(2*offset+cell_size*cell_count,2*offset+cell_size*cell_count,"SnakeGame");
    SetTargetFPS(60);
    
    Game game = Game();

    while(WindowShouldClose() == false)
    {
        
        BeginDrawing();
        //updates
        if( event_trigger(0.15))    
        {
            game.update();
        }
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y !=1)
        {
            game.snake.direction = {0,-1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y !=-1)
        {
            game.snake.direction = {0,1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x !=1)
        {
            game.snake.direction = {-1,0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x !=-1)
        {
            game.snake.direction = {1,0};
            game.running = true;
        }

        //static drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{offset-5,offset-5,(float)cell_size*cell_count+10,(float)cell_size*cell_count+10},5,dark_green);
        DrawText("Retro Snake",offset-5,20,40,dark_green);
        DrawText(TextFormat("%i",score),offset-5,offset+cell_size*cell_count+10,40,dark_green);

        //non-static drawing
        game.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
