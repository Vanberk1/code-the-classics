#include <Pudu.hpp>

#include <iostream>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;
constexpr uint32_t HALF_WIDTH = WIDTH / 2;
constexpr uint32_t HALF_HEIGHT = HEIGHT / 2;

constexpr float BAT_SPEED = 250;
constexpr float BALL_SPEED = 300;
constexpr float COLLISION_BOUNCE_FACTOR = 0.3f;

constexpr uint32_t MAX_SCORE = 10;

constexpr glm::vec4 MENU_ITEM_DEFAULT_COLOR = { 1, 1, 1, 1 };
constexpr glm::vec4 MENU_ITEM_SELECTED_COLOR = { 1.0f, 0.84f, 0.21f, 1 };
constexpr uint32_t MENU_NUM_ITEMS = 3;
constexpr uint32_t GAME_OVER_NUM_ITEMS = 2;

enum class State
{
    Menu,
    Game,
    GameOver
};

struct Bat
{
    glm::vec2 position = { 0, HALF_HEIGHT };
    glm::vec2 size = { 14, 80 };
    float speed = BAT_SPEED;
};

struct Ball
{
    glm::vec2 position = { HALF_WIDTH, HALF_HEIGHT };
    glm::vec2 size = { 10, 10 };
    glm::vec2 velocity = { 0, 0 };
    float speed = BALL_SPEED;
    bool isDirectionLeft = false;
};

struct PongGame
{  
    PongGame(pudu::Application& app) : app(app) {}

    void startGame()
    {
        leftBatScore = 0;
        rightBatScore = 0;
        
        leftBat.position = { 150, HALF_HEIGHT };
        rightBat.position = { WIDTH - 150, HALF_HEIGHT };

        resetBall();

        playing = true;
    }

    void update(float dt)
    {
        // Check number of players and define bats to be controlled by players or AI
        if(numOfPlayer == 0)
        {
            updateAIBat(leftBat, dt);
            updateAIBat(rightBat, dt);
        }
        else if(numOfPlayer == 1)
        {
            updateLeftPlayerBat(dt);
            updateAIBat(rightBat, dt);
        }
        else
        {
            updateLeftPlayerBat(dt);
            updateRightPlayerBat(dt);
        }

        updateBall(dt);
    }

    void updateBall(float dt)
    {
        ball.position += ball.velocity * ball.speed * dt;

        // Check collision with up and down walls
        if((ball.position.y - ball.size.y < 0) || (ball.position.y + ball.size.y / 2 > HEIGHT))
        {
            ball.velocity.y = -ball.velocity.y;
        }

        // Check collision with left and right bats.
        if((ball.isDirectionLeft && CheckBallBatCollision(leftBat)) || CheckBallBatCollision(rightBat))
        {
            Bat& bat = ball.isDirectionLeft ? leftBat : rightBat;
            
            ball.position.x = ball.isDirectionLeft ? leftBat.position.x + leftBat.size.x : rightBat.position.x - rightBat.size.x;
            ball.velocity.x = -ball.velocity.x;

            // Calculate how far from the center the ball collided to modify 
            // the bounce angle by a factor of this distance.
            float collisionDistanceNormalized = (ball.position.y - bat.position.y) / (bat.size.y / 2.0f);
            ball.velocity.y += collisionDistanceNormalized * COLLISION_BOUNCE_FACTOR;
            ball.velocity = glm::normalize(ball.velocity);
            ball.velocity.y = std::clamp(ball.velocity.y, -0.8f, 0.8f); // Prevents to strong vertical velocities.
            ball.velocity = glm::normalize(ball.velocity);
            
            ball.isDirectionLeft = !ball.isDirectionLeft;
        }

        if(ball.position.x - ball.size.x < 0)
        {
            rightBatScore++;
            resetBall();
        }
        else if(ball.position.x + ball.size.x > WIDTH) 
        {
            leftBatScore++;
            resetBall();
        }
    }

    void updateLeftPlayerBat(float dt)
    {
        if(app.isKeyPressed(pudu::Key::W) && (leftBat.position.y > 0))
        {
            leftBat.position.y -= leftBat.speed * dt;
        }
        else if(app.isKeyPressed(pudu::Key::S) && (leftBat.position.y < HEIGHT))
        {
            leftBat.position.y += leftBat.speed * dt;
        }
    }

    void updateRightPlayerBat(float dt)
    {
        if(app.isKeyPressed(pudu::Key::UP) && (rightBat.position.y > 0))
        {
            rightBat.position.y -= rightBat.speed * dt;
        }
        else if(app.isKeyPressed(pudu::Key::DOWN) && (rightBat.position.y < HEIGHT))
        {
            rightBat.position.y += rightBat.speed * dt;
        }
    }

    void updateAIBat(Bat& bat, float dt)
    {
        float xDistance = std::abs(ball.position.x - bat.position.x);

        float targetY1 = HALF_HEIGHT;
        float targetOffset = pudu::utils::GetRandomFloat(-10.0f, 10.0f);
        float targetY2 = ball.position.y + targetOffset;
        float weight1 = std::min(1.0f, xDistance / HALF_WIDTH);
        float weight2 = 1.0f - weight1;

        float targetY = (weight1 * targetY1) + (weight2 * targetY2);
        float movementFactor = 4.0f;

        bat.position.y += std::min(BAT_SPEED, std::max(-BAT_SPEED, targetY - bat.position.y)) * movementFactor * dt;
    }

    void draw()
    {
        app.drawRect(leftBat.position, leftBat.size);
        app.drawRect(rightBat.position, rightBat.size);
        if(playing)
        {
            app.drawRect(ball.position, ball.size);
        }
    }

    void resetBall()
    {
        ball.position = { HALF_WIDTH, HALF_HEIGHT };
        int randDir = pudu::utils::GetRandomInt(-100, 100);
        ball.isDirectionLeft = randDir < 0;
        ball.velocity = { ball.isDirectionLeft ? -ball.speed : ball.speed, 0 };
        ball.velocity = glm::normalize(ball.velocity);
    }

    bool isGameOver()
    {
        return leftBatScore >= MAX_SCORE || rightBatScore >= MAX_SCORE;
    }

    bool CheckBallBatCollision(const Bat& bat)
    {
        return ((ball.position.x > bat.position.x - bat.size.x / 2) && 
                (ball.position.x < bat.position.x + bat.size.x / 2) &&        
                (ball.position.y > bat.position.y - bat.size.y / 2) && 
                (ball.position.y < bat.position.y + bat.size.y / 2));
    }

    pudu::Application& app;
    uint32_t numOfPlayer = 0;
    bool playing = false;

    uint32_t leftBatScore = 0;
    uint32_t rightBatScore = 0; 

    Bat leftBat = { { 80, HALF_HEIGHT } };
    Bat rightBat = { { WIDTH - 80, HALF_HEIGHT } };
    Ball ball;

};

int main()
{
    pudu::Application app(WIDTH, HEIGHT, "PONG!");
    pudu::Font arialFont24("arial.ttf", 24);
    pudu::Font arialFont64("arial.ttf", 64);
    pudu::Font arialFont128("arial.ttf", 128);

    PongGame game(app);
    
    State state = State::Menu;

    // Current selected item index. This variable is reused for Menu and GameOver states.
    int menuHoveredItem = 0;

    while(app.isRunning())
    {
        app.processInput();

        if(state == State::Menu)
        {
            if(app.isKeyJustPressed(pudu::Key::UP))
            {
                if(--menuHoveredItem < 0)
                {
                    menuHoveredItem = MENU_NUM_ITEMS - 1;
                }
            }
            else if(app.isKeyJustPressed(pudu::Key::DOWN))
            {
                if(++menuHoveredItem >= MENU_NUM_ITEMS)
                {
                    menuHoveredItem = 0;
                }
            }
            
            if(app.isKeyJustPressed(pudu::Key::SPACE) || app.isKeyJustPressed(pudu::Key::ENTER))
            {
                if(menuHoveredItem == MENU_NUM_ITEMS - 1)
                {
                    return 0;
                }
                game.numOfPlayer = menuHoveredItem + 1;
                state = State::Game;
            }
            else if(app.isKeyJustPressed(pudu::Key::NUM1))
            {
                game.numOfPlayer = 1;
                state = State::Game;
            }
            else if(app.isKeyJustPressed(pudu::Key::NUM2))
            {
                game.numOfPlayer = 2;
                state = State::Game;
            }
            else if(app.isKeyJustPressed(pudu::Key::NUM3))
            {
                return 0;
            }
        }
        else if(state == State::Game)
        {
            if(!game.playing)
            {
                game.startGame();
            }
            
            game.update(app.getDeltaTime());
            
            if(game.isGameOver())
            {
                menuHoveredItem = 0;
                game.playing = false;
                state = State::GameOver;
            }
        }
        else if(state == State::GameOver)
        {
            if(app.isKeyJustPressed(pudu::Key::UP))
            {
                if(--menuHoveredItem < 0)
                {
                    menuHoveredItem = GAME_OVER_NUM_ITEMS - 1;
                }
            }
            else if(app.isKeyJustPressed(pudu::Key::DOWN))
            {
                if(++menuHoveredItem >= GAME_OVER_NUM_ITEMS)
                {
                    menuHoveredItem = 0;
                }
            }

            if(app.isKeyJustPressed(pudu::Key::SPACE) || app.isKeyJustPressed(pudu::Key::ENTER))
            {
                if(menuHoveredItem == 0)
                {
                    menuHoveredItem = 0;
                    state = State::Menu;
                }
                else if(menuHoveredItem == 1)
                {
                    state = State::Game;
                }
            }
            else if(app.isKeyJustPressed(pudu::Key::NUM1))
            {
                menuHoveredItem = 0;
                state = State::Menu;
            }
            else if(app.isKeyJustPressed(pudu::Key::NUM2) || app.isKeyJustPressed(pudu::Key::R))
            {
                state = State::Game;
            }
        }
        else
        {
            std::cout << "Invalid GameState: " << static_cast<int>(state) << std::endl;
            return 1;
        }
        
        // Render game states
        app.clearFrame({ 0, 0, 0, 1 });
        
        // TODO: Correct harcoded text positions after implementing function to calculate text pixel size
        if(state == State::Menu)
        {
            app.drawText("PONG!", arialFont128, { HALF_WIDTH - 200, 150 });
            app.drawText("1 - Player vs CPU", arialFont24, { HALF_WIDTH - 130, 400 }, menuHoveredItem == 0 ? MENU_ITEM_SELECTED_COLOR : MENU_ITEM_DEFAULT_COLOR);
            app.drawText("2 - Player vs Player", arialFont24, { HALF_WIDTH - 130, 430 }, menuHoveredItem == 1 ? MENU_ITEM_SELECTED_COLOR : MENU_ITEM_DEFAULT_COLOR);
            app.drawText("3 - Exit", arialFont24, { HALF_WIDTH - 130, 460 }, menuHoveredItem == 2 ? MENU_ITEM_SELECTED_COLOR : MENU_ITEM_DEFAULT_COLOR);
        }
        else if(state == State::Game)
        {
            app.drawText(std::format("{}", game.leftBatScore), arialFont64, { HALF_WIDTH - 150, 100 });
            app.drawText(std::format("{}", game.rightBatScore), arialFont64, { HALF_WIDTH + 130, 100 });
            game.draw();
        }
        else if(state == State::GameOver)
        {
            app.drawText(std::format("{}", game.leftBatScore), arialFont64, { HALF_WIDTH - 150, 100 });
            app.drawText(std::format("{}", game.rightBatScore), arialFont64, { HALF_WIDTH + 130, 100 });
            app.drawText("Game Over", arialFont64, { HALF_WIDTH - 170, HALF_HEIGHT - 100 });

            // TODO: Make win message more intelligent considering num of players. Show if winner is player 1/2 or cpu. 
            bool leftWins = game.leftBatScore > game.rightBatScore;
            app.drawText(std::format("{} Player Wins!", leftWins ? "Left" : "Right"), arialFont64, { 130, HALF_HEIGHT });

            app.drawText("1 - Back to Menu", arialFont24, { HALF_WIDTH - 130, 430 }, menuHoveredItem == 0 ? MENU_ITEM_SELECTED_COLOR : MENU_ITEM_DEFAULT_COLOR);
            app.drawText("2 - Restart Game", arialFont24, { HALF_WIDTH - 130, 460 }, menuHoveredItem == 1 ? MENU_ITEM_SELECTED_COLOR : MENU_ITEM_DEFAULT_COLOR);
        }
        else
        {
            std::cout << "Invalid State: " << static_cast<int>(state) << std::endl;
            return 1;
        }

        app.showFrame();
    }

    return 0;
}