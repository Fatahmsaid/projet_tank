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
    
        // Constructeur qui charge l'image de fond
        Background(const std::string& textureFile) {
            if (!texture.loadFromFile(textureFile)) {
                std::cerr << "Erreur : Impossible de charger l'image de fond '" << textureFile << "'\n";
            } else {
                std::cout << "Image de fond chargée avec succès." << std::endl;
                sprite.setTexture(texture);
    
                // Redimensionner l'image pour qu'elle couvre tout l'écran
                sprite.setScale(
                    static_cast<float>(1900) / texture.getSize().x, // Adapter à la largeur de la fenêtre
                    static_cast<float>(1050) / texture.getSize().y  // Adapter à la hauteur de la fenêtre
                );
            }
        }
    
        // Méthode pour dessiner le fond dans la fenêtre
        void draw(sf::RenderWindow& window) {
            window.draw(sprite);
        }
    };

// Classe représentant une balle
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

// Classe représentant un curseur
#include <SFML/Graphics.hpp>
#include <iostream>

class Slider {
public:
    sf::RectangleShape bar;      // Barre du slider
    sf::RectangleShape knob;     // Curseur du slider
    sf::Text titleText;          // Titre du slider
    sf::Text valueText;          // Texte pour la valeur du slider
    float minValue, maxValue;    // Valeurs minimales et maximales du slider
    float value;                 // Valeur actuelle du slider
    bool isAdjusting;            // Si le slider est en train d'être ajusté

    // Constructeur
    Slider(float x, float y, float width, float minValue, float maxValue, const std::string& title, sf::Font& font)
        : minValue(minValue), maxValue(maxValue), value((minValue + maxValue) / 2), isAdjusting(false) {
        bar.setSize(sf::Vector2f(width, 5));
        bar.setPosition(x, y);
        bar.setFillColor(sf::Color::Black);

        knob.setSize(sf::Vector2f(10, 20));
        knob.setPosition(x + (width / 2), y - 5);
        knob.setFillColor(sf::Color::Red);

        // Initialiser le titre
        titleText.setFont(font);
        titleText.setString(title);
        titleText.setCharacterSize(14);
        titleText.setFillColor(sf::Color::Black);
        titleText.setPosition(x, y - 25); // Placer le titre au-dessus du slider

        // Initialiser le texte de la valeur
        valueText.setFont(font);
        valueText.setCharacterSize(14);
        valueText.setFillColor(sf::Color::Black);
        valueText.setPosition(x + width + 10, y - 10); // Placer la valeur à droite du slider
        updateValueText();
    }

    // Mettre à jour le slider et les valeurs associées
    void update(sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                float mouseX = static_cast<float>(event.mouseButton.x);
                float mouseY = static_cast<float>(event.mouseButton.y);
                if (mouseX >= knob.getPosition().x && mouseX <= knob.getPosition().x + knob.getSize().x &&
                    mouseY >= knob.getPosition().y && mouseY <= knob.getPosition().y + knob.getSize().y) {
                    isAdjusting = true;
                }
            }
        } else if (event.type == sf::Event::MouseButtonReleased) {
            isAdjusting = false;
        } else if (event.type == sf::Event::MouseMoved && isAdjusting) {
            float mouseX = static_cast<float>(event.mouseMove.x);
            float newX = std::max(bar.getPosition().x, std::min(mouseX, bar.getPosition().x + bar.getSize().x - knob.getSize().x));
            knob.setPosition(newX, knob.getPosition().y);
            value = minValue + ((newX - bar.getPosition().x) / (bar.getSize().x - knob.getSize().x)) * (maxValue - minValue);
            updateValueText();  // Mise à jour de la valeur
        }
    }

    // Mettre à jour le texte de la valeur
    void updateValueText() {
        valueText.setString(std::to_string(static_cast<int>(value)));
    }

    // Dessiner le slider et ses composants
    void draw(sf::RenderWindow& window) {
        window.draw(bar);
        window.draw(knob);
        window.draw(titleText);
        window.draw(valueText);
    }
};


// Classe représentant un tank mobile
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

    // Chargement de la police
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "Erreur : Impossible de charger la police\n";
        return -1;
    }

    // Créer un Slider avec un titre
    Slider speedSlider(50, 100, 200, 0, 30, "VITESSE", font);
    Slider angleSlider(50, 150, 200, 180, 0, "ANGLE", font);

    std::unique_ptr<Ball> ball1 = nullptr; // Balle du premier tank
    std::unique_ptr<Ball> ball2 = nullptr; // Balle du deuxième tank

    Tank tank1(200, 400, "tank.png"); // Premier tank
    Tank tank2(600, 400, "tank_2.png"); // Deuxième tank

    bool isTank1Control = true; // Le contrôle du tank 1 est actif par défaut
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

            speedSlider.update(event);
            angleSlider.update(event);

            // Changer de contrôle entre les tanks
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
                isTank1Control = !isTank1Control; // Basculer le contrôle
            }

            // Tirer une balle depuis le centre du tank contrôlé
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                sf::Vector2f tankCenter = isTank1Control ? tank1.getCenter() : tank2.getCenter();
                float angleRad = angleSlider.value * (3.14159265f / 180.0f);
                sf::Vector2f initialVelocity(speedSlider.value * std::cos(angleRad), -speedSlider.value * std::sin(angleRad));

                if (isTank1Control) {
                    ball1 = std::make_unique<Ball>(10.0f, tankCenter, initialVelocity);
                } else {
                    ball2 = std::make_unique<Ball>(10.0f, tankCenter, initialVelocity);
                }
            }
        }

        // Mettre à jour les tanks
        if (isTank1Control) {
            tank1.update();
        } else {
            tank2.update();
        }

        // Mettre à jour les balles
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
        // Dessiner les sliders
        speedSlider.draw(window);
        angleSlider.draw(window);

        // Dessiner les tanks et les balles
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
