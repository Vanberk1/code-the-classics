#include <Pudu.hpp>

#include <iostream>
#include <variant>

constexpr uint32_t NUM_ROWS = 20;
constexpr uint32_t NUM_COLUMNS = 28;

constexpr uint32_t BLOCK_SIZE = 32;
constexpr uint32_t HALF_BLOCK_SIZE = BLOCK_SIZE / 2;

constexpr float GRAVITY = 20.0f;

constexpr uint32_t WIDTH = (NUM_COLUMNS * BLOCK_SIZE) + BLOCK_SIZE * 2;
constexpr uint32_t HEIGHT = NUM_ROWS * BLOCK_SIZE;

const std::string TILESET_TEXTURE_NAME = "tileset_texture";
const std::filesystem::path TILESET_TEXTURE_PATH = "assets/tileset.png";
constexpr uint32_t TILESET_TEXTURE_ROWS = 2;
constexpr uint32_t TILESET_TEXTURE_COLUMNS = 2;

using Level = std::array<std::string, NUM_ROWS>;
const std::vector<Level> LEVELS = {
    { 
        "XXXXX     XXXXXXXX     XXXXX",
        "","","","",
        "   XXXXXXX        XXXXXXX   ",
        "","","",
        "   XXXXXXXXXXXXXXXXXXXXXX   ",
        "","","",
        "XXXXXXXXX          XXXXXXXXX",
        "","","","",
        "XXXXX     XXXXXXXX     XXXXX"
    },
    {
        "XXXX    XXXXXXXXXXXX    XXXX",
        "","","","",
        "    XXXXXXXXXXXXXXXXXXXX    ",
        "","","",
        "XXXXXX                XXXXXX",
        "      X              X      ",
        "       X            X       ",
        "        X          X        ",
        "         X        X         ",
        "","","","",
        "XXXX    XXXXXXXXXXXX    XXXX"
    },
    {
        "XXXX    XXXX    XXXX    XXXX",
        "","","","",
        "  XXXXXXXX        XXXXXXXX  ",
        "","","",
        "XXXX      XXXXXXXX      XXXX",
        "","","",
        "    XXXXXX        XXXXXX    ",
        "","","","",
        "XXXX    XXXX    XXXX    XXXX"
    }
};

struct EntityBase
{
    EntityBase(pudu::Application& app) : app(app) {}
    virtual void update(float dt) {}
    virtual void draw() {}

    pudu::Application& app;
    bool isActive = false; // should be true or false by default?
    glm::vec2 position = { 0, 0 };
};

struct Game;
struct PhysicsEntity : EntityBase
{
    PhysicsEntity(pudu::Application& app, uint32_t level, bool gravity = false) 
    : EntityBase(app), levelGridIndex(level), applyGravity(gravity)
    {}

    void update(float dt) override
    {
        if(applyGravity)
        {
            velocity.y = std::min(velocity.y + GRAVITY, maxFallSpeed);
            // std::cout << "velocity (" << velocity.x << ", " << velocity.y << ")" << std::endl;
            if(moveAndCollide(0, velocity.y > 0 ? 1 : -1, std::abs(velocity.y * dt)))
            {
                // std::cout << "landed" << std::endl;
                velocity.y = 0;
                landed = true;
            }

            if(position.y >= HEIGHT)
            {
                position.y = 0;
            }
        }
    }

    bool moveAndCollide(float dx, float dy, float speed)
    {
        glm::vec2 newPos = position + glm::vec2(dx, dy) * speed;
        // std::cout << "delta (" << dx << ", " << dy << ") speed " << speed << std::endl;
        // std::cout << "newPos (" << newPos.x << ", " << newPos.y << ")" << std::endl;
        
        if(newPos.x < BLOCK_SIZE || newPos.x > WIDTH - BLOCK_SIZE)
        {
            return true;
        }
        
        // TODO: Replace with proper AABB collision to avoid subtracting a 
        // small factor to prevent entities from getting stuck in blocks
        if(dy > 0 && isBlock(newPos))
        {
            position.y = std::floor(newPos.y / BLOCK_SIZE) * BLOCK_SIZE - 0.1f;

            return true;
        }

        position = newPos;

        return false;
    }

    bool isBlock(glm::vec2 pos)
    {
        uint32_t gridX = static_cast<uint32_t>(pos.x / BLOCK_SIZE);
        uint32_t gridY = static_cast<uint32_t>(pos.y / BLOCK_SIZE);

        // std::cout << "posX: " << pos.x << " posY: " << pos.y << std::endl;
        // std::cout << "gridX: " << gridX << " gridY: " << gridY << std::endl;

        if(gridY > 0 && gridY < NUM_ROWS && gridX >= 0 && gridX < NUM_COLUMNS)
        {
            auto levelGrid = LEVELS[levelGridIndex];
            auto row = levelGrid[gridY];
            return row.size() > 0 and row[gridX] != ' ';
        }
            
        return false;
    }

    // Needed to check collision with the level blocks
    uint32_t levelGridIndex;

    glm::vec2 velocity = { 0, 0 };
    float maxFallSpeed = 300;
    bool applyGravity = false;
    bool landed = false;
};

struct Player : PhysicsEntity
{
    Player(pudu::Application& app, uint32_t level, bool active)
    : PhysicsEntity(app, level)
    { 
        isActive = active; 
    }

    void update(float dt) override
    {
        PhysicsEntity::update(dt);

        velocity.x = 0;
        if(app.isKeyPressed(pudu::Key::LEFT))
        {
            velocity.x -= 1;
        }
        if(app.isKeyPressed(pudu::Key::RIGHT))
        {
            velocity.x += 1;
        }
        if(app.isKeyJustPressed(pudu::Key::Z) && landed)
        {
            // std::cout << "jump" << std::endl; 
            velocity.y = -570;
            landed = false;
        }

        moveAndCollide(velocity.x, 0, 200 * dt);
    }

    void draw() override
    {
        glm::vec2 offset = { static_cast<float>(HALF_BLOCK_SIZE), -static_cast<float>(HALF_BLOCK_SIZE) };
        app.drawRect(position + offset, { BLOCK_SIZE, BLOCK_SIZE }, 0, { 0, 1, 0, 1 });
    }

    float dx = 1;
};

struct Enemy : PhysicsEntity
{
    Enemy(pudu::Application& app, uint32_t level, bool active)
    : PhysicsEntity(app, level)
    { 
        isActive = active; 
    }

    void update(float dt) override
    {

    }

    void draw() override
    {
        app.drawRect({ 300, 300 }, { 150, 100 }, 0, { 0, 0, 1, 1 });
    }
};

using Entity = std::variant<Player, Enemy>;

class Game
{
public:
    Game(pudu::Application& app, uint32_t players = 0, uint32_t level = 0) 
    : m_app(app), m_numPlayers(players), m_currentLevel(level)
    {
        auto tilesetTexture = 
            m_resources.loadTexture(TILESET_TEXTURE_NAME, TILESET_TEXTURE_PATH);
        tilesetTexture->setRows(TILESET_TEXTURE_ROWS);
        tilesetTexture->setColumns(TILESET_TEXTURE_COLUMNS);

        playCurrentLevel();
    }

    void update()
    {
        float dt = m_app.getDeltaTime();

        if(m_app.isKeyJustPressed(pudu::Key::ESCAPE))
        {
            m_app.close();
            return;
        }

        if(m_app.isKeyJustPressed(pudu::Key::SPACE))
        {
            m_currentLevel = (m_currentLevel + 1) % LEVELS.size();
            playCurrentLevel();
        }

        for(auto& ent : m_entities)
        {
            std::visit([dt](auto&& e) { e.update(dt); }, ent);
        }        
    }

    void draw()
    {
        auto texture = m_resources.getTexture(TILESET_TEXTURE_NAME);
        // Draw level grid
        for(size_t y = 0; y < NUM_ROWS; ++y)
        {
            if(y < NUM_ROWS - 1)
            {
                glm::vec2 leftBlockPos = { HALF_BLOCK_SIZE, (y * BLOCK_SIZE) + HALF_BLOCK_SIZE  };
                glm::vec2 rightBlockPos = { WIDTH - HALF_BLOCK_SIZE, (y * BLOCK_SIZE) + HALF_BLOCK_SIZE  };
                m_app.drawTextureFrame(*texture, leftBlockPos, { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
                m_app.drawTextureFrame(*texture, rightBlockPos, { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            }
            
            auto row = m_levelGrid[y];
            if(row.empty())
            {
                continue;
            }

            uint32_t offsetX = BLOCK_SIZE;
            for(size_t x = 0; x < NUM_COLUMNS; ++x)
            {
                if(row[x] != ' ')
                {
                    glm::vec2 blockPos = { offsetX + HALF_BLOCK_SIZE, (y * BLOCK_SIZE) + HALF_BLOCK_SIZE };
                    m_app.drawTextureFrame(*texture, blockPos, { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
                }
                offsetX += BLOCK_SIZE;
            }
        }

        for(auto& ent : m_entities)
        {
            std::visit([](auto&& e) { e.draw(); }, ent);
        }
    }

private:
    void playCurrentLevel()
    {
        assert(m_numPlayers >= 0 && m_numPlayers <= 2 && m_currentLevel >= 0 && m_currentLevel < LEVELS.size());

        m_levelGrid = LEVELS[m_currentLevel];

        for(uint32_t i = 0; i < m_numPlayers; ++i)
        {
            Player p(m_app, m_currentLevel, true);
            p.applyGravity = true;
            p.position = { 200, 100 };
            m_entities.push_back(p);
        }

        // m_entities.push_back(Enemy(m_app, m_levelGrid, true));
    }

private:
    pudu::Application& m_app;
    pudu::ResourceManager m_resources;

    uint32_t m_numPlayers;
    uint32_t m_currentLevel;

    std::array<std::string, NUM_ROWS> m_levelGrid;

    std::vector<Entity> m_entities;
};

int main()
{
    pudu::Application app(WIDTH, HEIGHT, "Bubble Bobble");

    Game game(app, 2);

    while(app.isRunning())
    {
        app.processInput();
        
        game.update();
        
        app.clearFrame({ 0, 0, 0, 1 });

        game.draw();
        
        app.showFrame();
    }

    return 0;
}
