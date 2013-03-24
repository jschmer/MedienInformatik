#include <SFMLApp.h>

int main() {
    SFMLApp app(600, 400, "SFML Toolkit Sample", sf::Style::Resize | sf::Style::Close);
    return app.Run();
}