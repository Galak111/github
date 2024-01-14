#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
class Ball {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;

    Ball(float radius, sf::Vector2f initialVelocity)
        : velocity(initialVelocity) {
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(400, 300);
    }

    void move() {
        shape.move(velocity);
    }

    void reset(sf::Vector2f initialVelocity) {
        shape.setPosition(400, 300);
        velocity = initialVelocity;
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

class Menu {
public:
    sf::Text startText;
    sf::Text exitText;
    sf::Font font;

    Menu(float screenWidth, float screenHeight) {
        if (!font.loadFromFile("images/arial.ttf")) {
            std::cerr << "Error loading font\n";
        }

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
    }
};

sf::Color getRandomColor() {
    int red = rand() % 256;
    int green = rand() % 256;
    int blue = rand() % 256;
    return sf::Color(red, green, blue);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(700, 600), "Arkanoid Game");

    Menu menu(window.getSize().x, window.getSize().y);

    Ball ball(10.0f, sf::Vector2f(0.1f, -0.1f)); // Ball starts moving upwards
    Paddle paddle(150.0f, 10.0f, sf::Vector2f(0.0f, 0.0f));

    const int brickRowCount = 5;
    const int brickColumnCount = 10;
    const float brickWidth = 70.0f;
    const float brickHeight = 20.0f;
    std::vector<Brick> bricks;

    for (int i = 0; i < brickRowCount; ++i) {
        for (int j = 0; j < brickColumnCount; ++j) {
            sf::Color color = getRandomColor();
            bricks.push_back(Brick(brickWidth, brickHeight, color,
                sf::Vector2f(j * brickWidth, i * brickHeight)));
        }
    }

    enum class GameState { MENU, PLAYING, GAME_OVER, EXIT, PAUSED };
    GameState gameState = GameState::MENU;
    int selectedOption = 0;
    int score = 0;

    sf::Text scoreText;
    scoreText.setFont(menu.font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition(10, 520);

    // Load background image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background.png")) {
        std::cerr << "Error loading background image\n";
        return 1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

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
                    selectedOption = (selectedOption - 1 + 2) % 2;
                    if (selectedOption == 0) {
                        menu.startText.setFillColor(sf::Color::Yellow);
                    }
                    else {
                        menu.exitText.setFillColor(sf::Color::Yellow);
                    }
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    menu.startText.setFillColor(sf::Color::White);
                    menu.exitText.setFillColor(sf::Color::White);
                    selectedOption = (selectedOption + 1) % 2;
                    if (selectedOption == 0) {
                        menu.startText.setFillColor(sf::Color::Yellow);
                    }
                    else {
                        menu.exitText.setFillColor(sf::Color::Yellow);
                    }
                }
                else if (event.key.code == sf::Keyboard::Return) {
                    if (selectedOption == 0) {
                        gameState = GameState::PLAYING;
                        score = 0; // Reset the score when starting a new game
                        ball.reset(sf::Vector2f(0.1f, -0.1f)); // Restart the ball
                        for (auto& brick : bricks) {
                            brick.reset();
                        }
                    }
                    else {
                        gameState = GameState::EXIT;
                    }
                }
            }
        }

        if (gameState == GameState::PLAYING) {
            if (escapePressed) {
                gameState = GameState::PAUSED;
                sf::sleep(sf::milliseconds(200));
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
                    gameState = GameState::PLAYING;
                    score = 0; // Reset the score when restarting
                    ball.reset(sf::Vector2f(0.1f, -0.1f)); // Restart the ball
                    for (auto& brick : bricks) {
                        brick.reset();
                    }
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    gameState = GameState::EXIT;
                }
            }
        }

        window.clear();

        // Draw background
        window.draw(backgroundSprite);

        switch (gameState) {
        case GameState::MENU:
            window.draw(menu.startText);
            window.draw(menu.exitText);
            break;

        case GameState::PLAYING:
            ball.move();
            paddle.move();

            if (ball.shape.getPosition().x <= 0 || ball.shape.getPosition().x + ball.shape.getRadius() * 2 >= window.getSize().x) {
                ball.velocity.x = -ball.velocity.x;
            }

            if (ball.shape.getPosition().y <= 0) {
                ball.velocity.y = -ball.velocity.y;
            }

            // Adjust the paddle collision to prevent it from going into the wall
            if (ball.shape.getGlobalBounds().intersects(paddle.shape.getGlobalBounds())) {
                ball.velocity.y = -ball.velocity.y;

                // Adjust the ball position to prevent it from getting stuck in the paddle
                float overlap = ball.shape.getPosition().y + ball.shape.getRadius() - paddle.shape.getPosition().y;
                ball.shape.move(0, -2 * overlap);
            }

            for (auto& brick : bricks) {
                if (!brick.isDestroyed && ball.shape.getGlobalBounds().intersects(brick.shape.getGlobalBounds())) {
                    brick.isDestroyed = true;
                    ball.velocity.y = -ball.velocity.y;
                    score++; // Increase score when a brick is hit
                }
            }

            window.draw(ball.shape);
            window.draw(paddle.shape);

            for (const auto& brick : bricks) {
                if (!brick.isDestroyed) {
                    window.draw(brick.shape);
                }
            }

            // Draw scoreText on top of everything
            scoreText.setString("Press Esc to Pause\nBricks Destroyed: " + std::to_string(score));
            window.draw(scoreText);




            // Check if the ball touched the ground
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
