#include "renderer.hpp"
#include <gui/urlbar.hpp>
#include <utils/log.hpp>

Renderer::Renderer(std::map<std::string, std::map<std::string, std::string>> p_css)
    : css(p_css)
{
}
void Renderer::main(std::string url)
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "AWB");

    font.loadFromFile("assets/times-new-roman.ttf");

    for (auto &text : texts)
        text.setFont(font);

    window.setFramerateLimit(60);

    sf::View view;
    BottomBar bottom_bar;
    bottom_bar.make_bar("http://" + url, &window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
	    switch(event.type)
	    {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::Resized:
			sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);

                	view.reset(visibleArea);

                	window.setView(view);
			break;

	    }
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        for (auto &text : texts)
        {
            window.draw(text);
        }
	
        window.draw(bottom_bar.rect);
        window.draw(bottom_bar.text);
	
        window.display();
    }
}

void Renderer::render(std::vector<orca::LayoutData> layout_data)
{
    for (uint i = 0; i < layout_data.size(); i++)
    {
        sf::Text text;
        auto div = layout_data[i];

        text.setCharacterSize(div.text_size);

        if (i == 0)
            text.setPosition({0, 0});
        else
            text.setPosition(0, voff);

        auto color = sf::Color{div.color};

        text.setFillColor(color);

        div.text += "\n\n";
        text.setString(div.text);

        texts.push_back(text);

        voff += div.text_size * 2;
    }
}
