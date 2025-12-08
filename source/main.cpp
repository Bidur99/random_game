#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "Block.h"

// Using fixed window size for fullscreen mode (1080p)
const unsigned WINDOW_WIDTH = 1920;
const unsigned WINDOW_HEIGHT = 1080;

// Simple list of game states so the game knows what screen it’s in
enum GameState { MENU, PLAYING, PAUSED, GAMEOVER };

// Particle system used for the death explosion
// (each one is a small circle that flies outward and fades)
struct Particle
{
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float life;

    Particle(sf::Vector2f pos)
    {
        // Small circle as the particle
        shape.setRadius(5.f);
        shape.setOrigin(5.f, 5.f);
        shape.setFillColor(sf::Color(255, 100, 100, 230));

        // Give it a random angle + speed so it flies outwards
        float angle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.f;
        float speed = 180.f + static_cast<float>(std::rand() % 150);
        velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);

        life = 0.7f; // fades away after 0.7 seconds

        shape.setPosition(pos);
    }

    // Returns true when the particle is 'dead' and should be removed
    bool update(float dt)
    {
        life -= dt;
        shape.move(velocity * dt);

        // Fade out the alpha (opacity)
        sf::Color c = shape.getFillColor();
        c.a = static_cast<sf::Uint8>(std::max(0.f, life) * 255);
        shape.setFillColor(c);

        return life <= 0.f;
    }
};

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Creating fullscreen window
    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "Dodge or Die",
        sf::Style::Fullscreen
    );
    window.setFramerateLimit(90);

    // These two flags help avoid the ESC/Q keys being spammed
    bool escReleased = true;
    bool qReleased = true;

   
    // Background gradient setup
   
    // Using a quad so top is dark blue/purple and bottom is deeper purple
    sf::VertexArray background(sf::Quads, 4);
    background[0].position = sf::Vector2f(0.f, 0.f);
    background[1].position = sf::Vector2f(WINDOW_WIDTH, 0.f);
    background[2].position = sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    background[3].position = sf::Vector2f(0.f, WINDOW_HEIGHT);

    background[0].color = sf::Color(20, 20, 40);
    background[1].color = sf::Color(20, 20, 40);
    background[2].color = sf::Color(50, 0, 70);
    background[3].color = sf::Color(50, 0, 70);

    
    // Player rectangle + glow effect
    
    sf::RectangleShape player(sf::Vector2f(160.f, 32.f));
    player.setFillColor(sf::Color::Cyan);
    player.setOrigin(player.getSize().x / 2.f, player.getSize().y / 2.f);
    player.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 70.f);

    float playerSpeed = 600.f;

    // Glow is a big faint cyan circle rendered behind the player
    sf::CircleShape playerGlow(60.f);
    playerGlow.setOrigin(60.f, 60.f);
    playerGlow.setFillColor(sf::Color(0, 255, 255, 30));

    
    // Falling blocks list
    
    std::vector<Block> blocks;
    float spawnTimer = 0.f;
    float spawnInterval = 0.7f; // how often new blocks appear

   
    // Particle list
    
    // explosion particles generated on death
    std::vector<Particle> particles;

    
    // Score and difficulty handling
   
    float timeSurvived = 0.f; // how long player stayed alive
    int score = 0;
    int highScore = 0;
    int stage = 1;

    // Load previous high score
    {
        std::ifstream in("highscore.txt");
        if (in.is_open())
            in >> highScore;
    }

   
    // Font + HUD text setup
    
    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/Arial.ttf"); // using a built-in Windows font

    sf::Text scoreText, highScoreText, stageText;

    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20.f, 20.f);

    highScoreText.setFont(font);
    highScoreText.setCharacterSize(30);
    highScoreText.setFillColor(sf::Color::Yellow);
    highScoreText.setPosition(WINDOW_WIDTH - 300.f, 20.f);

    stageText.setFont(font);
    stageText.setCharacterSize(30);
    stageText.setFillColor(sf::Color::Green);
    stageText.setPosition(WINDOW_WIDTH / 2.f - 60.f, 20.f);

    
    // Main menu text
    
    sf::Text menuTitle;
    menuTitle.setFont(font);
    menuTitle.setCharacterSize(70);
    menuTitle.setFillColor(sf::Color::Cyan);
    menuTitle.setString("DODGE OR DIE");
    menuTitle.setPosition(WINDOW_WIDTH / 2.f - 300.f, 250.f);

    sf::Text menuStart;
    menuStart.setFont(font);
    menuStart.setCharacterSize(40);
    menuStart.setFillColor(sf::Color::White);
    menuStart.setString("Click or Press ENTER to Start");
    menuStart.setPosition(WINDOW_WIDTH / 2.f - 350.f, 400.f);

    sf::Text menuHS;
    menuHS.setFont(font);
    menuHS.setCharacterSize(34);
    menuHS.setFillColor(sf::Color::Yellow);
    menuHS.setPosition(WINDOW_WIDTH / 2.f - 150.f, 480.f);

    
    // Pause menu
    
    sf::Text pauseText;
    pauseText.setFont(font);
    pauseText.setCharacterSize(50);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setString("PAUSED\nPress ESC to Resume\nPress Q to Quit");
    pauseText.setPosition(WINDOW_WIDTH / 2.f - 250.f, WINDOW_HEIGHT / 2.f - 100.f);

   
    // Game over screen
   
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER\nPress R to Restart\nPress M for Menu\nPress Q to Quit");
    gameOverText.setPosition(WINDOW_WIDTH / 2.f - 300.f, WINDOW_HEIGHT / 2.f - 120.f);

    
    // Sound effects
    
    sf::SoundBuffer clickBuffer, stageBuffer, deathBuffer;
    sf::Sound clickSound, stageSound, deathSound;

    clickBuffer.loadFromFile("resources/sounds/click.ogg");
    clickSound.setBuffer(clickBuffer);

    stageBuffer.loadFromFile("resources/sounds/stageup.ogg");
    stageSound.setBuffer(stageBuffer);

    deathBuffer.loadFromFile("resources/sounds/death.ogg");
    deathSound.setBuffer(deathBuffer);

    
    // Background music loop
    
    sf::Music bgMusic;
    if (bgMusic.openFromFile("resources/sounds/bgmusic.ogg"))
    {
        bgMusic.setLoop(true);
        bgMusic.setVolume(50);
        bgMusic.play();
    }

    
    // Starting game state + timer
    
    GameState gameState = MENU;
    sf::Clock clock;

    // The main game loop — runs until the window closes
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds(); // time between frames

        
        // HANDLE INPUT EVENTS
       
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Clicking the X button closes the game
            if (event.type == sf::Event::Closed)
                window.close();

            // Q = quit anywhere (but with cooldown)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            {
                if (qReleased)
                    window.close();
                qReleased = false;
            }
            else qReleased = true;

            // ESC in menu also quits
            if (gameState == MENU && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                if (escReleased)
                    window.close();
                escReleased = false;
            }
            else escReleased = true;

            // Mouse click on start button (menu)
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                if (gameState == MENU &&
                    menuStart.getGlobalBounds().contains(mousePos))
                {
                    clickSound.play();

                    // reset everything
                    blocks.clear();
                    particles.clear();
                    timeSurvived = 0.f;
                    score = 0;
                    stage = 1;
                    player.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 70.f);

                    gameState = PLAYING;
                }
            }
        }

        
        // MENU STATE LOGIC
       
        if (gameState == MENU)
        {
            menuHS.setString("High Score: " + std::to_string(highScore));

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                clickSound.play();

                blocks.clear();
                particles.clear();
                timeSurvived = 0.f;
                score = 0;
                stage = 1;
                player.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 70.f);

                gameState = PLAYING;
            }
        }

        
        // PAUSED LOGIC
       
        else if (gameState == PAUSED)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                if (escReleased)
                    gameState = PLAYING;
                escReleased = false;
            }
            else escReleased = true;
        }

        
        // GAME OVER LOGIC
        
        else if (gameState == GAMEOVER)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            {
                blocks.clear();
                particles.clear();
                timeSurvived = 0.f;
                score = 0;
                stage = 1;
                player.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 70.f);

                gameState = PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
                gameState = MENU;
        }

       
        // MAIN GAME LOGIC (PLAYING)
        
        if (gameState == PLAYING)
        {
            // ESC pauses the game
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                if (escReleased)
                    gameState = PAUSED;

                escReleased = false;
            }
            else escReleased = true;

            // Score increases by time survived
            timeSurvived += dt;
            score = static_cast<int>(timeSurvived);

            // Difficulty scaling: the further you get, the faster blocks spawn
            int newStage = stage;

            if (score < 15) newStage = 1, spawnInterval = 0.70f;
            else if (score < 30) newStage = 2, spawnInterval = 0.60f;
            else if (score < 50) newStage = 3, spawnInterval = 0.50f;
            else if (score < 70) newStage = 4, spawnInterval = 0.40f;
            else if (score < 90) newStage = 5, spawnInterval = 0.33f;
            else newStage = 6, spawnInterval = 0.28f;

            // play stage sound only when stage updates
            if (newStage > stage)
            {
                stage = newStage;
                stageSound.play();
            }

            // Player movement (keyboard)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                player.move(-playerSpeed * dt, 0.f);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                player.move(playerSpeed * dt, 0.f);

            // Player follows mouse if left button is held
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                float mouseX = sf::Mouse::getPosition(window).x;
                player.setPosition(mouseX, player.getPosition().y);
            }

            // Keep player inside screen bounds
            sf::FloatRect pb = player.getGlobalBounds();
            if (pb.left < 0)
                player.setPosition(pb.width / 2.f, pb.top + pb.height / 2.f);

            if (pb.left + pb.width > WINDOW_WIDTH)
                player.setPosition(WINDOW_WIDTH - pb.width / 2.f, pb.top + pb.height / 2.f);

            // Block spawning
            spawnTimer += dt;
            if (spawnTimer >= spawnInterval)
            {
                blocks.emplace_back(
                    sf::Vector2f(std::rand() % WINDOW_WIDTH, -150.f),
                    200.f + stage * 40.f,
                    stage
                );
                spawnTimer = 0.f;
            }

            // Update all blocks
            for (auto& b : blocks)
                b.update(dt);

            // Collision detection with player
            for (auto& b : blocks)
            {
                if (b.getBounds().intersects(player.getGlobalBounds()))
                {
                    // update high score file
                    if (score > highScore)
                    {
                        highScore = score;
                        std::ofstream out("highscore.txt");
                        out << highScore;
                    }

                    deathSound.play();

                    // collision -> spawn many particles
                    for (int i = 0; i < 40; i++)
                        particles.emplace_back(player.getPosition());

                    gameState = GAMEOVER;
                    break;
                }
            }

            // Remove blocks that have fallen off-screen
            blocks.erase(
                std::remove_if(blocks.begin(), blocks.end(),
                    [&](const Block& b) { return b.isOffScreen(WINDOW_HEIGHT); }),
                blocks.end()
            );

            // Update all particles
            particles.erase(
                std::remove_if(particles.begin(), particles.end(),
                    [&](Particle& p) { return p.update(dt); }),
                particles.end()
            );

            // Update text
            scoreText.setString("Score: " + std::to_string(score));
            highScoreText.setString("High Score: " + std::to_string(highScore));
            stageText.setString("Stage: " + std::to_string(stage));
        }

        
        // DRAW EVERYTHING ON SCREEN
       
        window.clear();
        window.draw(background);

        if (gameState == MENU)
        {
            window.draw(menuTitle);
            window.draw(menuStart);
            window.draw(menuHS);
        }
        else
        {
            // Draw every block
            for (auto& b : blocks)
                window.draw(b);

            // Draw glow + player rectangle
            playerGlow.setPosition(player.getPosition());
            window.draw(playerGlow);
            window.draw(player);

            // Draw particle explosion
            for (auto& p : particles)
                window.draw(p.shape);

            // HUD text
            window.draw(scoreText);
            window.draw(highScoreText);
            window.draw(stageText);

            // Pause screen
            if (gameState == PAUSED)
                window.draw(pauseText);

            // Game over screen
            if (gameState == GAMEOVER)
                window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}
