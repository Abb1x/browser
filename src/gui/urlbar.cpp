#include <gui/urlbar.hpp>
#include <utils/log.hpp>

void BottomBar::make_bar(std::string url, sf::RenderWindow *win)
{
    auto win_size = win->getSize();
    font.loadFromFile("assets/FiraCode-Regular.ttf");

    rect.setSize({static_cast<float>(win_size.x), 30});
    rect.setPosition({0, static_cast<float>(win_size.y) - 30});

    rect.setFillColor(sf::Color{0x2E3440FF});

    text.setFont(font);
    text.setCharacterSize(15);
    text.setPosition({static_cast<float>(win_size.x) - (15 * url.size() - 15), static_cast<float>(win_size.y) - 25});
    text.setFillColor(sf::Color{0xA3BE8CFF});
    text.setString("http://themostamazingwebsiteontheinternet.com");
}
