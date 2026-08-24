#include <Pudu.hpp>

#include <iostream>
#include <variant>

constexpr uint32_t SCALE_FACTOR = 2;

constexpr uint32_t BLOCK_SIZE = 16 * SCALE_FACTOR;
constexpr uint32_t HALF_BLOCK_SIZE = BLOCK_SIZE / 2;

constexpr uint32_t LEVEL_X_OFFSET = 2 * BLOCK_SIZE;

constexpr uint32_t NUM_ROWS = 26;
constexpr uint32_t NUM_COLUMNS = 28;

constexpr uint32_t WIDTH = NUM_COLUMNS * BLOCK_SIZE + 4 * BLOCK_SIZE;
constexpr uint32_t HEIGHT = NUM_ROWS * BLOCK_SIZE;

constexpr float GRAVITY = 20.0f;
constexpr float PLAYER_SPEED = 250.0f;
constexpr float PLAYER_JUMP_SPEED = 700.0f;

const std::string TILESET_TEXTURE_NAME = "tileset_texture";
const std::filesystem::path TILESET_TEXTURE_PATH = "assets/tileset.png";
constexpr uint32_t TILESET_TEXTURE_ROWS = 2;
constexpr uint32_t TILESET_TEXTURE_COLUMNS = 2;

const std::filesystem::path BUB_TEXTURE_PATH = "assets/bub.png";

using Level = std::array<std::string, NUM_ROWS>;
const std::vector<Level> LEVELS = {
    {
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXX",
        "", "", "", "", "", "", "", "",
        "", 
        "XX   XXXXXXXXXXXXXXXXXX   XX",
        "", "", "", "",
        "XX   XXXXXXXXXXXXXXXXXX   XX", 
        "", "", "", "",
        "XX   XXXXXXXXXXXXXXXXXX   XX", 
        "", "", "", "",
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    },
    {
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXX",
        "", "", "", "",
        "           XXXXXX           ", 
        "", "", "", "",
        "        XXXXX  XXXXX        ",
        "", "", "", "",
        "      XXXXXXXXXXXXXXXX      ", 
        "", "", "", "",
        "  XXXXXXX  XXXXXX  XXXXXXXX ", 
        "", "", "", "",
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    },
    {
        "XXXXXXX    XXXXXX    XXXXXXX",
        "", "", "", "",
        "   XXXXXXXX      XXXXXXXX   ", 
        "   X                    X   ", 
        "   X                    X   ", 
        "   X                    X   ", 
        "   X                    X   ",
        "   XXXXXXXXX    XXXXXXXXX   ",
        "   X                    X   ", 
        "   X                    X   ", 
        "   X                    X   ", 
        "   X                    X   ",
        "   XXXXXXXXXX  XXXXXXXXXX   ", 
        "", "", "", "",
        "XXXXX  XXX        XXX  XXXXX",
        "", "", "", "",
        "XXXXXXX    XXXXXX    XXXXXXX"
    },
    {
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXX",
        "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "",
        "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"
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

            if(position.y > HEIGHT)
            {
                position.y = 0;
            }
            else if(position.y < 0)
            {
                position.y = HEIGHT;
            }
        }
    }

    bool moveAndCollide(float dx, float dy, float speed)
    {
        glm::vec2 newPos = position + glm::vec2(dx, dy) * speed;

        glm::vec2 bottom = { newPos.x, newPos.y + size.y / 2 };
        glm::vec2 left = { newPos.x - size.x / 2, newPos.y };
        glm::vec2 right = { newPos.x + size.x / 2, newPos.y };
        
        if(left.x < LEVEL_X_OFFSET || right.x > WIDTH - LEVEL_X_OFFSET)
        {
            return true;
        }
        
        // TODO: Replace with proper AABB collision to avoid subtracting a 
        // small factor to prevent entities from getting stuck in blocks
        bool collided = false;
        if(dy > 0 && isBlock({newPos.x, bottom.y}))
        {
            collided = true;
        }
        else if(dx < 0 && isBlock(left))
        {
            collided = true;
        }
        else if(dx > 0 && isBlock(right))
        {
            collided = true;
        }

        if(!collided)
        {
            position = newPos;
        }

        return collided;
    }

    bool isBlock(glm::vec2 pos)
    {
        uint32_t gridX = static_cast<uint32_t>((pos.x - LEVEL_X_OFFSET) / BLOCK_SIZE);
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

    glm::vec2 size = { 0, 0 };
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

        // TODO: Player texture should be set according to the number of players
        texture = std::make_shared<pudu::Texture>(BUB_TEXTURE_PATH);
        texture->setRows(1);
        texture->setColumns(2);
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
            velocity.y = -PLAYER_JUMP_SPEED;
            landed = false;
        }

        if(velocity.x != 0)
        {
            facingRight = velocity.x > 0;
        }

        moveAndCollide(velocity.x, 0, PLAYER_SPEED * dt);
    }

    void draw() override
    {
        app.drawTextureFrame(*texture, position, spriteSize, 0, 0, glm::vec4(1), !facingRight);
        // app.drawRect(position, size, 0, { 1, 0, 0, 1}, false);
        // app.drawRect(position, spriteSize, 0, { 0, 1, 0, 1}, false);
    }

    std::shared_ptr<pudu::Texture> texture;
    glm::vec2 spriteSize;
    bool facingRight = true;
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
            glm::vec2 leftBlockPos = { HALF_BLOCK_SIZE, (y * BLOCK_SIZE) + HALF_BLOCK_SIZE  };
            glm::vec2 rightBlockPos = { WIDTH - HALF_BLOCK_SIZE, (y * BLOCK_SIZE) + HALF_BLOCK_SIZE  };
            m_app.drawTextureFrame(*texture, leftBlockPos, { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            m_app.drawTextureFrame(*texture, leftBlockPos + glm::vec2(BLOCK_SIZE, 0), { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            m_app.drawTextureFrame(*texture, rightBlockPos, { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            m_app.drawTextureFrame(*texture, rightBlockPos - glm::vec2(BLOCK_SIZE, 0), { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            
            auto row = m_levelGrid[y];
            if(row.empty())
            {
                continue;
            }

            uint32_t offsetX = LEVEL_X_OFFSET;
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

        m_entities.clear();

        for(uint32_t i = 0; i < m_numPlayers; ++i)
        {
            Player p(m_app, m_currentLevel, true);
            p.applyGravity = true;
            p.position = { 120, 768 };
            p.size = glm::vec2(16 * SCALE_FACTOR * 2);
            p.spriteSize = glm::vec2(20 * SCALE_FACTOR * 2);
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
