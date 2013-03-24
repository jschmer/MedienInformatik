#include <SFMLApp.h>

int main() {
    SFMLApp app(600, 400, sf::Style::Resize | sf::Style::Close, "SFML Toolkit Sample");
    return app.Run();
}