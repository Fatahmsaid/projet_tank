class BarreDeVie {
    public:
        sf::RectangleShape fond;
        sf::RectangleShape barre;
        float vie;
    
        BarreDeVie(float x, float y) {
            vie = 100.0f;
            fond.setSize(sf::Vector2f(104, 14));
            fond.setFillColor(sf::Color::Black);
            fond.setPosition(x, y);
            
            barre.setSize(sf::Vector2f(100, 10));
            barre.setFillColor(sf::Color::Green);
            barre.setPosition(x + 2, y + 2);
        }
    
        void reduireVie(float degats) {
            vie -= degats;
            if (vie < 0) vie = 0;
            barre.setSize(sf::Vector2f(vie, 10));
            if (vie < 40) barre.setFillColor(sf::Color::Yellow);
            if (vie < 20) barre.setFillColor(sf::Color::Red);
        }
    
        void draw(sf::RenderWindow& window) {
            window.draw(fond);
            window.draw(barre);
        }
    };
    