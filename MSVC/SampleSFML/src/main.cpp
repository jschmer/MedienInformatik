/* 
 * Copyright (c) 2013 Jens Schmer
 * This file is distributed under the MIT License.
 * Consult COPYING within this package for further information. 
 */
#include <SFMLApp.h>

int main() {
    SFMLApp app(600, 400, "SFML Toolkit Sample", sf::Style::Resize | sf::Style::Close);
    return app.Run();
}