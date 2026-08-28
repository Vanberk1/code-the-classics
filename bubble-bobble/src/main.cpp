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

// TODO: Create constant variables for other entities. 
// Now enemies and bubbles use player's variables with some harcoded scale factor.
constexpr float GRAVITY = 20.0f;
constexpr float PLAYER_SPEED = 250.0f;
constexpr float PLAYER_JUMP_SPEED = 650.0f;
constexpr float MAX_FALL_SPEED = 150.0f;

const std::string TILESET_TEXTURE_NAME = "tileset_texture";
const std::filesystem::path TILESET_TEXTURE_PATH = "assets/tileset.png";
constexpr uint32_t TILESET_TEXTURE_ROWS = 2;
constexpr uint32_t TILESET_TEXTURE_COLUMNS = 2;

const std::filesystem::path BUB_TEXTURE_PATH = "assets/bub.png";
const std::filesystem::path ENEMY_1_TEXTURE_PATH = "assets/enemy_1.png";
const std::filesystem::path BUBBLE_GREEN_TEXTURE_PATH = "assets/bubble_green.png";

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
        "  XXXXXXX  XXXXXX  XXXXXXX  ", 
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

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

struct Game;
struct EntityContext
{
    std::shared_ptr<pudu::Application> app;
    std::shared_ptr<Game> game;
    uint32_t levelIndex;
};

// TODO: Maybe separate entity data in different components
struct EntityData
{
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 velocity;
    bool isActive = false;
    bool applyGravity = false;
    bool facingRight = true;
    bool landed = false;
    bool topCollision = false;
};

struct Sprite
{
    std::shared_ptr<pudu::Texture> texture;
    glm::vec2 spriteSize;
};

// NOTE: Free functions to work with EntityData
namespace
{

bool IsBlock(const Level& level, glm::vec2 pos)
{
    uint32_t gridX = static_cast<uint32_t>((pos.x - LEVEL_X_OFFSET) / BLOCK_SIZE);
    uint32_t gridY = static_cast<uint32_t>(pos.y / BLOCK_SIZE);

    // std::cout << "posX: " << pos.x << " posY: " << pos.y << std::endl;
    // std::cout << "gridX: " << gridX << " gridY: " << gridY << std::endl;

    if(gridY > 0 && gridY < NUM_ROWS && gridX >= 0 && gridX < NUM_COLUMNS)
    {
        auto row = level[gridY];
        return row.size() > 0 and row[gridX] != ' ';
    }
        
    return false;
}

bool MoveAndCollide(const Level& level, EntityData& data, float dx, float dy, float speed)
{
    glm::vec2 newPos = data.position + glm::vec2(dx, dy) * speed;

    glm::vec2 bottom = { newPos.x, newPos.y + data.size.y / 2 };
    glm::vec2 top = { newPos.x, newPos.y - data.size.y / 2 };
    glm::vec2 left = { newPos.x - data.size.x / 2, newPos.y };
    glm::vec2 right = { newPos.x + data.size.x / 2, newPos.y };
    
    if(left.x < LEVEL_X_OFFSET || right.x > WIDTH - LEVEL_X_OFFSET)
    {
        return true;
    }

    bool collided = false;
    // TODO: Replace with proper AABB collision to avoid subtracting a 
    // small factor to prevent entities from getting stuck in blocks

    // left and right positions are downscaled a little bit to prevent entities from getting stuck in tight spaces
    if((dy > 0) && (IsBlock(level, bottom) || IsBlock(level, { left.x * 1.01f, bottom.y }) || IsBlock(level, { right.x * 0.99f, bottom.y })))
    {
        // Prevents player to get stuck in a column of tiles.
        if(IsBlock(level, data.position))
        {
            collided = false;
        }
        else 
        {
            data.position.y = static_cast<uint32_t>(newPos.y / BLOCK_SIZE) * BLOCK_SIZE - 0.01f;
            data.velocity.y = 0;
            collided = true;
        }
    }
    else if(dy < 0 && IsBlock(level, top))
    {
        data.topCollision = true;
    }
    
    if((dx < 0) && !data.topCollision && (IsBlock(level, left) || IsBlock(level, { left.x, bottom.y }) || IsBlock(level, { left.x, top.y })))
    {
        data.position.x = (static_cast<uint32_t>((newPos.x) / BLOCK_SIZE) + 1) * BLOCK_SIZE;
        data.velocity.x = 0;
        collided = true;
        
    }
    else if((dx > 0) && !data.topCollision && (IsBlock(level, right) || IsBlock(level, { right.x, bottom.y }) || IsBlock(level, { right.x, top.y })))
    {
        data.position.x = (static_cast<uint32_t>((right.x - LEVEL_X_OFFSET) / BLOCK_SIZE) + 1) * BLOCK_SIZE;
        data.velocity.x = 0;
        collided = true;
    }

    if(!collided)
    {
        data.position = newPos;
    }

    return collided;
}

void GravityUpdate(const Level& level, EntityData& data, float dt)
{
    if(data.applyGravity)
    {
        data.velocity.y = std::min(data.velocity.y + GRAVITY, MAX_FALL_SPEED);
        // std::cout << "velocity (" << velocity.x << ", " << velocity.y << ")" << std::endl;
        if(MoveAndCollide(level, data, 0, data.velocity.y > 0 ? 1 : -1, std::abs(data.velocity.y * dt)))
        {
            data.landed = true;
            data.topCollision = false;
        }
    }

    if(data.position.y > HEIGHT)
    {
        data.position.y = 0;
    }
}

}

struct Player
{
    EntityContext ctx;
    EntityData data;

    Sprite sprite;

    Player(
        std::shared_ptr<pudu::Application> app, 
        std::shared_ptr<Game> game, 
        uint32_t level, 
        bool active)
    : ctx({ app, game, level })
    {
        data.isActive = active;
        data.applyGravity = true;
    }

    // NOTE: Player update method defined outside the struct body to use game object methods.
    void update(float dt);

    void draw()
    {
        ctx.app->drawTextureFrame(*sprite.texture, data.position, sprite.spriteSize, 0, 0, glm::vec4(1), !data.facingRight);
    }

};

// TODO: Implement all other enemies someday
// Zen-chan, Maita, Monsta, Pulpul, Banebou, Hidegons, Drunk, Invader, Skel-Monsta, Rascal
enum class EnemyType
{
    ZenChan
};

enum class EnemyState
{
    Normal,
    Aggressive
};

struct Enemy
{
    EntityContext ctx;
    EntityData data;

    Sprite sprite;

    EnemyType type = EnemyType::ZenChan;
    EnemyState state = EnemyState::Normal;
    float chageDirectionTimer = 0.0f;
    std::optional<glm::vec2> targetPos;

    Enemy(
        std::shared_ptr<pudu::Application> app,
        std::shared_ptr<Game> game,
        uint32_t level, 
        bool active, 
        EnemyType type,
        EnemyState state)
    : ctx({ app, game, level }), type(type), state(state)
    {
        data.isActive = active;
        data.applyGravity = true;
    }

    // NOTE: Enemy update method defined outside the struct body to use game object methods.
    void update(float dt);

    void draw()
    {
        ctx.app->drawTextureFrame(*sprite.texture, data.position, sprite.spriteSize, 0, 0, glm::vec4(1), data.facingRight);
        // ctx.app->drawRect(data.position, data.size, 0, { 0, 0, 1, 1 });
    }

};

struct Bubble
{
    EntityContext ctx;
    EntityData data;

    Sprite sprite;

    float floatingTime = 1.0f;
    float lifeTime = 5.0f;
    bool floating = false;

    std::optional<EnemyType> trappedEnemy;
    
    Bubble(
        std::shared_ptr<pudu::Application> app,
        std::shared_ptr<Game> game,
        uint32_t level, 
        bool active)
    {
        ctx = { app, game, level };
        data.isActive = active;
        data.applyGravity = false;
    }

    void update(float dt)
    {
        lifeTime -= dt;
        floatingTime -= dt;

        if(floatingTime <= 0.0f || trappedEnemy)
        {
            floating = true;
            data.velocity = { 0, -1 };
        }

        if(lifeTime <= 0.0f)
        {
            data.isActive = false;
            return;
        }
        
        if(!floating && MoveAndCollide(LEVELS[ctx.levelIndex], data, data.velocity.x, 0, PLAYER_SPEED * dt))
        {
            floating = true;
            data.velocity = { 0, -1 };
        }
        else
        {
            data.position.y += data.velocity.y * (PLAYER_SPEED / 2.0f) * dt;
        }
    }

    void draw()
    {
        // TODO: Frame should be set according to the trapped enemy texture.
        ctx.app->drawTextureFrame(*sprite.texture, data.position, sprite.spriteSize, 3, 0, glm::vec4(1));
    }
};

using Entity = std::variant<Player, Enemy, Bubble>;

class Game
{
public:
    Game(std::shared_ptr<pudu::Application> app, uint32_t players = 0, uint32_t level = 0) 
    : m_app(app), m_numPlayers(players), m_currentLevel(level)
    {
        m_instance = std::shared_ptr<Game>(this);

        auto tilesetTexture = 
            m_resources.loadTexture(TILESET_TEXTURE_NAME, TILESET_TEXTURE_PATH);
        tilesetTexture->setRows(TILESET_TEXTURE_ROWS);
        tilesetTexture->setColumns(TILESET_TEXTURE_COLUMNS);

        playCurrentLevel();
    }

    void update()
    {
        float dt = m_app->getDeltaTime();

        if(m_app->isKeyJustPressed(pudu::Key::ESCAPE))
        {
            m_app->close();
            return;
        }

        if(m_app->isKeyJustPressed(pudu::Key::SPACE))
        {
            m_currentLevel = (m_currentLevel + 1) % LEVELS.size();
            playCurrentLevel();
        }

        if(!m_entitiesToAdd.empty())
        {
            m_entities.insert(m_entities.end(), m_entitiesToAdd.begin(), m_entitiesToAdd.end());
            m_entitiesToAdd.clear();
        }

        std::optional<glm::vec2> playerPos = std::nullopt;
        if(m_playerIdx && m_playerIdx < m_entities.size())
        {
            playerPos = std::get<Player>(m_entities[*m_playerIdx]).data.position;
        }

        for(auto& ent : m_entities)
        {
            std::visit([dt, &playerPos](auto&& e) { 
                // Check at compilation if the entity is an enemy to set target position.
                if constexpr (std::is_same_v<std::decay_t<decltype(e)>, Enemy>)
                {
                    e.targetPos = playerPos;
                }
                e.update(dt); 
            }, ent);
        }

        std::erase_if(m_entities, [](auto& ent) {
            return std::visit([](auto&& e) { return !e.data.isActive; }, ent);
        });
    }

    void draw()
    {
        auto texture = m_resources.getTexture(TILESET_TEXTURE_NAME);
        // Draw level grid
        for(size_t y = 0; y < NUM_ROWS; ++y)
        {
            // NOTE: All levels have two vertical lines at both edges of the screen
            glm::vec2 leftBlockPos = { HALF_BLOCK_SIZE, (y * BLOCK_SIZE) + HALF_BLOCK_SIZE  };
            glm::vec2 rightBlockPos = { WIDTH - HALF_BLOCK_SIZE, (y * BLOCK_SIZE) + HALF_BLOCK_SIZE  };
            m_app->drawTextureFrame(*texture, leftBlockPos, { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            m_app->drawTextureFrame(*texture, leftBlockPos + glm::vec2(BLOCK_SIZE, 0), { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            m_app->drawTextureFrame(*texture, rightBlockPos, { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            m_app->drawTextureFrame(*texture, rightBlockPos - glm::vec2(BLOCK_SIZE, 0), { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
            
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
                    m_app->drawTextureFrame(*texture, blockPos, { BLOCK_SIZE, BLOCK_SIZE }, m_currentLevel);
                }
                offsetX += BLOCK_SIZE;
            }
        }

        for(auto& ent : m_entities)
        {
            std::visit([](auto&& e) { e.draw(); }, ent);
        }
    }

    void spawnBubble(glm::vec2 position, float directionX)
    {
        Bubble b(m_app, m_instance, m_currentLevel, true);
        b.data.position = position;
        b.data.velocity.x = directionX;
        b.data.size = glm::vec2(16 * SCALE_FACTOR * 2);

        b.sprite.texture = m_resources.loadTexture(BUBBLE_GREEN_TEXTURE_PATH.string(), BUBBLE_GREEN_TEXTURE_PATH);
        b.sprite.texture->setRows(1);
        b.sprite.texture->setColumns(7);
        b.sprite.spriteSize = glm::vec2(20 * SCALE_FACTOR * 2);

        m_entitiesToAdd.push_back(b);
    }

    void spawnEnemy(glm::vec2 position, glm::vec2 direction, EnemyType type, EnemyState state)
    {
        // TODO: Setup enemy properties according the type.
        Enemy e(m_app, m_instance, m_currentLevel, true, type, state);
        e.data.position = position;
        e.data.velocity = (direction.length() == 0) ? direction : glm::normalize(direction);
        e.data.size = glm::vec2(16 * SCALE_FACTOR * 2);
        e.data.facingRight = direction.x > 0;
        e.chageDirectionTimer = 5.0f;
        // TODO: Texture should be set according to the enemy type and state
        e.sprite.texture = m_resources.loadTexture(ENEMY_1_TEXTURE_PATH.string(), ENEMY_1_TEXTURE_PATH);
        e.sprite.texture->setRows(1);
        e.sprite.texture->setColumns(4);
        e.sprite.spriteSize = glm::vec2(20 * SCALE_FACTOR * 2);

        m_entitiesToAdd.push_back(e);
    }

    std::vector<Entity>& entities() { return m_entities; }

private:
    void playCurrentLevel()
    {
        assert(m_numPlayers >= 0 && m_numPlayers <= 2 && m_currentLevel >= 0 && m_currentLevel < LEVELS.size());

        m_levelGrid = LEVELS[m_currentLevel];

        m_playerIdx = std::nullopt;
        m_entities.clear();

        for(uint32_t i = 0; i < m_numPlayers; ++i)
        {
            Player p(m_app, m_instance, m_currentLevel, true);
            p.data.position = { 120, 768 };
            p.data.size = glm::vec2(16 * SCALE_FACTOR * 2);
            
            // TODO: Player texture should be set according to the number of players
            p.sprite.texture = m_resources.loadTexture(BUB_TEXTURE_PATH.string(), BUB_TEXTURE_PATH);
            p.sprite.texture->setRows(1);
            p.sprite.texture->setColumns(2);
            p.sprite.spriteSize = glm::vec2(20 * SCALE_FACTOR * 2);

            m_playerIdx = m_entities.size();
            m_entities.push_back(p);
        }

        std::vector<glm::vec2> enemyPositionsLevel1 = {
            { WIDTH / 2, 0 },
            { WIDTH / 2, -64 },
            { WIDTH / 2, -128 }
        };
        for(const auto& pos : enemyPositionsLevel1)
        {
            spawnEnemy(pos, { -1, 0 }, EnemyType::ZenChan, EnemyState::Normal);
        }
    }

private:
    // NOTE: Self intance reference to share with entities
    std::shared_ptr<Game> m_instance;
    std::shared_ptr<pudu::Application> m_app;
    pudu::ResourceManager m_resources;

    uint32_t m_numPlayers;
    uint32_t m_currentLevel;

    std::array<std::string, NUM_ROWS> m_levelGrid;

    std::vector<Entity> m_entitiesToAdd;
    std::vector<Entity> m_entities;

    // TODO: this std::optional feels overkill, but it avoids using -1 as an invalid value.
    // For the moment expects only one player.
    std::optional<uint32_t> m_playerIdx;
};

// NOTE: Player update method defined outside the struct body to use game object methods.
void Player::update(float dt)
{
    auto& velocity = data.velocity;

    velocity.x = 0;
    if(ctx.app->isKeyPressed(pudu::Key::LEFT))
    {
        velocity.x -= 1;
    }
    if(ctx.app->isKeyPressed(pudu::Key::RIGHT))
    {
        velocity.x += 1;
    }
    if(ctx.app->isKeyJustPressed(pudu::Key::X) && data.landed && velocity.y == 0)
    {
        // std::cout << "jump" << std::endl; 
        velocity.y = -PLAYER_JUMP_SPEED;
        data.landed = false;
    }
    if(ctx.app->isKeyJustPressed(pudu::Key::Z))
    {
        // std::cout << "shoot bubble" << std::endl;
        ctx.game->spawnBubble(data.position, data.facingRight ? 1 : -1);
    }

    float speed = PLAYER_SPEED;
    if(velocity.y != 0)
    {
        speed /= 2;
    }

    if(velocity.x != 0)
    {
        data.facingRight = velocity.x > 0;
    }

    GravityUpdate(LEVELS[ctx.levelIndex], data, dt);
    MoveAndCollide(LEVELS[ctx.levelIndex], data, velocity.x, 0, speed * dt);
}

// NOTE: Enemy update method defined outside the struct body to use game object methods.
// TODO: Separate classes in different files.
void Enemy::update(float dt)
{
    GravityUpdate(LEVELS[ctx.levelIndex], data, dt);

    chageDirectionTimer -= dt;
        
    if(data.velocity.y == 0.0f && MoveAndCollide(LEVELS[ctx.levelIndex], data, data.velocity.x, 0, PLAYER_SPEED * 0.7f * dt))
    {
        data.velocity.x = -data.velocity.x;
        chageDirectionTimer = 0.0f;
    }
    
    if(chageDirectionTimer <= 0.0f)
    {
        std::vector<int> directions = { -1, 1 };
        if(targetPos)
        {
            directions.push_back(sign(targetPos->x - data.position.x) < 0 ? -1 : 1);
        }

        int dirIdx = pudu::utils::GetRandomInt(0, directions.size() - 1);
        data.velocity.x = directions[dirIdx];
        chageDirectionTimer = pudu::utils::GetRandomFloat(1, 4);
    }

    if(data.velocity.x != 0)
    {
        data.facingRight = data.velocity.x > 0;
    }

    bool trapped = false;
    for(auto& ent : ctx.game->entities())
    {
        std::visit([this, &trapped](auto&& e) { 
            // Check at compilation if the entity is an enemy to set target position.
            if constexpr (std::is_same_v<std::decay_t<decltype(e)>, Bubble>)
            {
                if(!e.trappedEnemy && (glm::distance(data.position, e.data.position) < data.size.x / 2))
                {
                    data.isActive = false;
                    e.trappedEnemy = type;
                    // TODO: Make a better way to set texture and frame of trapped enemies
                    e.sprite.texture = sprite.texture;
                    trapped = true;
                }
            }
        }, ent);

        if(trapped)
        {
            break;
        }
    }
}

int main()
{
    auto app = std::make_shared<pudu::Application>(WIDTH, HEIGHT, "Bubble Bobble");

    Game game(app, 1);

    while(app->isRunning())
    {
        app->processInput();
        
        game.update();
        
        app->clearFrame({ 0, 0, 0, 1 });

        game.draw();
        
        app->showFrame();
    }

    return 0;
}
