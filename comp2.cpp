#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <memory> // Pour std::unique_ptr

const float GRAVITY = 9.81f;
const float SCALE = 50.0f;
const float TIME_STEP = 0.016f;

class Background {
public:
    sf::Texture texture;
    sf::Sprite sprite;
    
    Background(const std::string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Erreur : Impossible de charger l'image de fond '" << textureFile << "'\n";
        } else {
            sprite.setTexture(texture);
            sprite.setScale(
                static_cast<float>(1900) / texture.getSize().x,
                static_cast<float>(1050) / texture.getSize().y
            );
        }
    }
    
    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

class Ball {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    
    Ball(float radius, sf::Vector2f position, sf::Vector2f initialVelocity)
        : velocity(initialVelocity) {
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
    }
    
    void update(float deltaTime) {
        velocity.y += GRAVITY * deltaTime;
        shape.move(velocity * deltaTime * SCALE);
    }
};

class Tank {
public:
    sf::Texture texture;
    sf::Sprite sprite;
    float speed = 5.0f;
    
    Tank(float x, float y, const std::string& textureFile) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Erreur : Impossible de charger l'image " << textureFile << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setOrigin(texture.getSize().x / 2, texture.getSize().y / 2);
    }
    
    void update() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            sprite.move(-speed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            sprite.move(speed, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            sprite.move(0, -speed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            sprite.move(0, speed);
        }
    }
    
    sf::Vector2f getCenter() {
        return sprite.getPosition();
    }
};

bool checkCollision(const Ball& ball, const Tank& tank) {
    return ball.shape.getGlobalBounds().intersects(tank.sprite.getGlobalBounds());
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 1200), "Tank Battle");
    window.setFramerateLimit(60);
    Background background("fond_decran.png");
    
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "Erreur : Impossible de charger la police\n";
        return -1;
    }
    
    std::unique_ptr<Ball> ball1 = nullptr;
    std::unique_ptr<Ball> ball2 = nullptr;
    
    Tank tank1(200, 400, "tank.png");
    Tank tank2(600, 400, "tank_2.png");
    
    bool isTank1Control = true;
    bool boom = false;
    float boomTime = 0.0f;  // Nouveau compteur de temps
    sf::Text boomText;
    boomText.setFont(font);
    boomText.setCharacterSize(50);
    boomText.setFillColor(sf::Color::Red);
    boomText.setString("BOOM");
    boomText.setPosition(800, 600);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                isTank1Control = !isTank1Control;
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                sf::Vector2f tankCenter = isTank1Control ? tank1.getCenter() : tank2.getCenter();
                sf::Vector2f initialVelocity(10.0f, -10.0f);
                
                if (isTank1Control) {
                    ball1 = std::make_unique<Ball>(10.0f, tankCenter, initialVelocity);
                } else {
                    ball2 = std::make_unique<Ball>(10.0f, tankCenter, initialVelocity);
                }
            }
        }
        
        if (isTank1Control) {
            tank1.update();
        } else {
            tank2.update();
        }
        
        if (ball1) {
            ball1->update(TIME_STEP);
            if (checkCollision(*ball1, tank2)) {
                boom = true;
                boomTime = 0.0f;  // Reset le temps à chaque collision
            }
        }
        
        if (ball2) {
            ball2->update(TIME_STEP);
            if (checkCollision(*ball2, tank1)) {
                boom = true;
                boomTime = 0.0f;  // Reset le temps à chaque collision
            }
        }
        
        if (boom) {
            boomTime += TIME_STEP;  // Incrémente le temps écoulé
            if (boomTime >= 3.0f) {  // Si 3 secondes se sont écoulées
                boom = false;  // Cache le texte "BOOM"
            }
        }
        
        window.clear(sf::Color::White);
        background.draw(window);
        window.draw(tank1.sprite);
        window.draw(tank2.sprite);
        if (ball1) {
            window.draw(ball1->shape);
        }
        if (ball2) {
            window.draw(ball2->shape);
        }
        if (boom) {
            window.draw(boomText);
        }
        window.display();
    }
    
    return 0;
}
