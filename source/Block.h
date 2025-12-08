#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>

// This class represents a single falling block in the game.
// I made it inherit from sf::Drawable so I can draw it directly with window.draw(block).
class Block : public sf::Drawable
{
private:
    sf::RectangleShape shape; // the actual square shape
    float speed;              // how fast it falls down

public:
    Block(sf::Vector2f pos, float fallingSpeed, int stage)
    {
        // Starting block size (already large because the game is chaotic)
        sf::Vector2f size(120.f, 120.f);

        // Increase size as the game gets harder
        if (stage >= 3)
            size = sf::Vector2f(140.f, 140.f);

        // Even bigger blocks on higher stages
        if (stage >= 5)
            size = sf::Vector2f(160.f, 160.f);

        shape.setSize(size);
        shape.setOrigin(size.x / 2.f, size.y / 2.f); // center pivot so it falls naturally

        // Slight outline to make blocks easier to see
        shape.setOutlineThickness(3.f);
        shape.setOutlineColor(sf::Color::Black);

        // Random bright colours so blocks are visually different
        sf::Color colours[] = {
            sf::Color::Red,
            sf::Color::Magenta,
            sf::Color::Yellow,
            sf::Color(0, 255, 128),
            sf::Color(0, 180, 255),
            sf::Color(255, 140, 0)
        };

        int index = std::rand() % 6;
        shape.setFillColor(colours[index]);

        // Set initial spawn position
        shape.setPosition(pos);

        // Store falling speed
        speed = fallingSpeed;
    }

    // Move block down each frame
    void update(float dt)
    {
        // Only falls vertically
        shape.move(0.f, speed * dt);
    }

    // Used for collision detection against the player
    sf::FloatRect getBounds() const
    {
        return shape.getGlobalBounds();
    }

    // Check if the block has completely left the screen
    bool isOffScreen(float screenHeight) const
    {
        // +200 so they fully disappear before being removed
        return shape.getPosition().y > screenHeight + 200.f;
    }

private:
    // This is required because Block inherits sf::Drawable
    // SFML calls this when you do window.draw(block)
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        target.draw(shape, states);
    }
};
