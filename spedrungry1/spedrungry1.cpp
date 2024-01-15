#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <vector>
#include <algorithm>

class Ball {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float speedMultiplier;

    Ball(float radius, sf::Vector2f initialVelocity, float speedMultiplier)
        : velocity(initialVelocity), speedMultiplier(speedMultiplier) {
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(400, 300);
    }

    void move() {
        shape.move(speedMultiplier * velocity);
    }

    void reset(sf::Vector2f initialVelocity, float speedMultiplier) {
        shape.setPosition(200, 250);
        velocity = initialVelocity;
        this->speedMultiplier = speedMultiplier;
    }
};

class Paddle {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Paddle(float width, float height, sf::Vector2f initialVelocity)
        : velocity(initialVelocity) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(400 - width / 2, 550);
    }

    void move() {
        shape.move(velocity);
    }
};

class Brick {
public:
    sf::RectangleShape shape;
    bool isDestroyed;

    Brick(float width, float height, sf::Color color, sf::Vector2f position)
        : isDestroyed(false) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setFillColor(color);
        shape.setPosition(position);
    }

    void reset() {
        isDestroyed = false;
    }
};

class IrregularShape {
public:
    sf::ConvexShape shape;

    IrregularShape() {
        shape.setPointCount(10); // 10 punktów tworzących gwiazdę
        shape.setPoint(0, sf::Vector2f(10, 0));
        shape.setPoint(1, sf::Vector2f(3, 9));
        shape.setPoint(2, sf::Vector2f(0, 3));
        shape.setPoint(3, sf::Vector2f(-3, 9));
        shape.setPoint(4, sf::Vector2f(-10, 0));
        shape.setPoint(5, sf::Vector2f(-3, -9));
        shape.setPoint(6, sf::Vector2f(0, -3));
        shape.setPoint(7, sf::Vector2f(3, -9));
        shape.setPoint(8, sf::Vector2f(10, 0));
        shape.setPoint(9, sf::Vector2f(3, 9));

        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(100, 100);

        shape.setScale(3.0f, 3.0f);
    }
};

class Menu {
public:
    sf::Text startText;
    sf::Text exitText;
    sf::Text levelText;
    sf::Font font;
    sf::Text levelSelectText;
    Menu(float screenWidth, float screenHeight) {
        if (!font.loadFromFile("images/arial.ttf")) {
            std::cerr << "Error loading font\n";
        }

        levelSelectText.setFont(font);
        levelSelectText.setString("Select Starting Level");
        levelSelectText.setCharacterSize(30);
        levelSelectText.setFillColor(sf::Color::White);
        levelSelectText.setPosition(screenWidth / 2 - 150, screenHeight / 2 + 100);

        startText.setFont(font);
        startText.setString("Start");
        startText.setCharacterSize(30);
        startText.setFillColor(sf::Color::Yellow);
        startText.setPosition(screenWidth / 2 - 50, screenHeight / 2 - 50);

        exitText.setFont(font);
        exitText.setString("Exit");
        exitText.setCharacterSize(30);
        exitText.setFillColor(sf::Color::White);
        exitText.setPosition(screenWidth / 2 - 50, screenHeight / 2 + 50);

        levelText.setFont(font);
        levelText.setString("Select Level");
        levelText.setCharacterSize(30);
        levelText.setFillColor(sf::Color::White);
        levelText.setPosition(screenWidth / 2 - 70, screenHeight / 2);
    }
};

sf::Color getRandomColor() {
    int red = rand() % 256;
    int green = rand() % 256;
    int blue = rand() % 256;
    return sf::Color(red, green, blue);
}

void resetGame(Ball* ball, Paddle* paddle, std::vector<Brick>* bricks, int* score, int* level) {
    ball->reset(sf::Vector2f(0.1f, -0.1f), 1.0f);
    paddle->velocity = sf::Vector2f(0.0f, 0.0f);
    *score = 0;

    bricks->clear();
    const int brickRowCount = 5 + *level;
    const int brickColumnCount = 10 + *level;
    const float brickWidth = 70.0f;
    const float brickHeight = 20.0f;

    for (int i = 0; i < brickRowCount; ++i) {
        for (int j = 0; j < brickColumnCount; ++j) {
            sf::Color color = getRandomColor();
            bricks->push_back(Brick(brickWidth, brickHeight, color,
                sf::Vector2f(j * brickWidth, i * brickHeight)));
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(700, 600), "Arkanoid Game");

    Menu menu(window.getSize().x, window.getSize().y);

    Ball ball(10.0f, sf::Vector2f(0.1f, -0.1f), 1.0f);
    Paddle paddle(150.0f, 10.0f, sf::Vector2f(0.0f, 0.0f));

    std::vector<Brick> bricks;
    int score = 0;
    int level = 1; // Starting level

    enum class GameState { MENU, PLAYING, GAME_OVER, EXIT, PAUSED };
    GameState gameState = GameState::MENU;
    int selectedOption = 0;
    sf::Text levelText;

    levelText.setFont(menu.font);
    levelText.setCharacterSize(30);
    levelText.setFillColor(sf::Color::Yellow);
    levelText.setPosition(550, 550);

    IrregularShape irregularShape;

    sf::Text scoreText;
    scoreText.setFont(menu.font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition(10, 520);

    // Load background image
    sf::Texture backgroundTextureLevel1;
    sf::Texture backgroundTextureLevel2;

    if (!backgroundTextureLevel1.loadFromFile("images/background.png") ||
        !backgroundTextureLevel2.loadFromFile("images/background2.jpg")) {
        std::cerr << "Error loading background image\n";
        return 1;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTextureLevel1);

    bool escapePressed = false;

    while (window.isOpen()) {
        sf::Event event;
        if (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    escapePressed = true;
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Escape) {
                    escapePressed = false;
                }
            }
        }

        if (gameState == GameState::MENU) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    menu.startText.setFillColor(sf::Color::White);
                    menu.exitText.setFillColor(sf::Color::White);
                    menu.levelSelectText.setFillColor(sf::Color::White);
                    selectedOption = (selectedOption - 1 + 3) % 3;
                    if (selectedOption == 0) {
                        menu.startText.setFillColor(sf::Color::Yellow);
                    }
                    else if (selectedOption == 1) {
                        menu.levelSelectText.setFillColor(sf::Color::Yellow);
                    }
                    else {
                        menu.exitText.setFillColor(sf::Color::Yellow);
                    }
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    menu.startText.setFillColor(sf::Color::White);
                    menu.exitText.setFillColor(sf::Color::White);
                    menu.levelSelectText.setFillColor(sf::Color::White);
                    selectedOption = (selectedOption + 1) % 3;
                    if (selectedOption == 0) {
                        menu.startText.setFillColor(sf::Color::Yellow);
                    }
                    else if (selectedOption == 1) {
                        menu.levelSelectText.setFillColor(sf::Color::Yellow);
                    }
                    else {
                        menu.exitText.setFillColor(sf::Color::Yellow);
                    }
                }
                else if (event.key.code == sf::Keyboard::Return) {
                    if (selectedOption == 0) {
                        gameState = GameState::PLAYING;
                        resetGame(&ball, &paddle, &bricks, &score, &level);

                    }
                    else if (selectedOption == 1) {
                        gameState = GameState::MENU;
                        selectedOption = 0; // Reset selectedOption
                        menu.levelSelectText.setString("Select Starting Level: " + std::to_string(level));
                    }
                    else {
                        gameState = GameState::EXIT;
                    }
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Up && selectedOption == 1) {
                    if (level > 1) {
                        level--; // Decrease level by 1
                        menu.levelSelectText.setString("Select Starting Level: " + std::to_string(level));
                    }
                }
                else if (event.key.code == sf::Keyboard::Down && selectedOption == 1) {
                    if (level < 2) {  // Update this condition to level < maximum level you want
                        level++; // Increase level by 1
                        menu.levelSelectText.setString("Select Starting Level: " + std::to_string(level));
                    }
                }
            }
        }

        if (gameState == GameState::PLAYING) {
            if (escapePressed) {
                gameState = GameState::PAUSED;
                sf::sleep(sf::milliseconds(200));
            }
            if (level == 2) {
                backgroundSprite.setTexture(backgroundTextureLevel2);
            }
            paddle.velocity.x = 0.0f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
                paddle.shape.getPosition().x > 0) {
                paddle.velocity.x = -0.4f;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
                paddle.shape.getPosition().x + paddle.shape.getSize().x < window.getSize().x) {
                paddle.velocity.x = 0.4f;
            }
        }

        if (gameState == GameState::PAUSED) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) {
                gameState = GameState::PLAYING;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                gameState = GameState::EXIT;
            }
        }

        if (gameState == GameState::GAME_OVER) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Return) {
                    gameState = GameState::MENU;
                    level = 1; // Reset the level to 1
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    gameState = GameState::EXIT;
                }
            }
        }

        window.clear();

        // Draw background
        window.draw(backgroundSprite);
        // Draw irregular shape in the menu
        window.draw(irregularShape.shape);

        switch (gameState) {
        case GameState::MENU:
            window.draw(menu.startText);
            window.draw(menu.levelSelectText);
            window.draw(menu.exitText);
            break;

        case GameState::PLAYING:
            ball.move();
            paddle.move();

            if (ball.shape.getPosition().x <= 0 || ball.shape.getPosition().x + ball.shape.getRadius() * 2 >=
                window.getSize().x) {
                ball.velocity.x = -ball.velocity.x;
            }

            if (ball.shape.getPosition().y <= 0) {
                ball.velocity.y = -ball.velocity.y;
            }

            if (ball.shape.getGlobalBounds().intersects(paddle.shape.getGlobalBounds())) {
                ball.velocity.y = -ball.velocity.y;
                float overlap = ball.shape.getPosition().y + ball.shape.getRadius() - paddle.shape.getPosition().y;
                ball.shape.move(0, -2 * overlap);
            }

            for (auto& brick : bricks) {
                if (!brick.isDestroyed && ball.shape.getGlobalBounds().intersects(brick.shape.getGlobalBounds())) {
                    brick.isDestroyed = true;
                    ball.velocity.y = -ball.velocity.y;
                    score++;
                }
            }

            window.draw(ball.shape);
            window.draw(paddle.shape);

            for (const auto& brick : bricks) {
                if (!brick.isDestroyed) {
                    window.draw(brick.shape);
                }
            }

            scoreText.setString("Press Esc to Pause\nBricks Destroyed: " + std::to_string(score));
            window.draw(scoreText);

            levelText.setString("Level: " + std::to_string(level));
            window.draw(levelText);

            if (ball.shape.getPosition().y + ball.shape.getRadius() * 2 >= window.getSize().y) {
                gameState = GameState::GAME_OVER;
            }
            break;

        case GameState::GAME_OVER:
        {
            sf::Text gameOverText;
            gameOverText.setFont(menu.font);
            gameOverText.setString("Game Over");
            gameOverText.setCharacterSize(50);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(window.getSize().x / 2 - 150, window.getSize().y / 2 - 25);

            sf::Text restartText;
            restartText.setFont(menu.font);
            restartText.setString("Press Enter to Restart");
            restartText.setCharacterSize(30);
            restartText.setFillColor(sf::Color::White);
            restartText.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 + 50);

            sf::Text exitText;
            exitText.setFont(menu.font);
            exitText.setString("Press Esc to Exit");
            exitText.setCharacterSize(30);
            exitText.setFillColor(sf::Color::White);
            exitText.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 + 100);

            window.draw(gameOverText);
            window.draw(restartText);
            window.draw(exitText);
        }
        break;

        case GameState::PAUSED:
        {
            sf::Text pausedText;
            pausedText.setFont(menu.font);
            pausedText.setString("Paused");
            pausedText.setCharacterSize(50);
            pausedText.setFillColor(sf::Color::Yellow);
            pausedText.setPosition(window.getSize().x / 2 - 100, window.getSize().y / 2 - 25);

            sf::Text continueText;
            continueText.setFont(menu.font);
            continueText.setString("Press Enter to Continue");
            continueText.setCharacterSize(30);
            continueText.setFillColor(sf::Color::White);
            continueText.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 + 50);

            sf::Text exitText;
            exitText.setFont(menu.font);
            exitText.setString("Press Esc to Exit");
            exitText.setCharacterSize(30);
            exitText.setFillColor(sf::Color::White);
            exitText.setPosition(window.getSize().x / 2 - 200, window.getSize().y / 2 + 100);

            window.draw(pausedText);
            window.draw(continueText);
            window.draw(exitText);
        }
        break;

        case GameState::EXIT:
            window.close();
            break;
        }

        window.display();
    }

    return 0;
}
