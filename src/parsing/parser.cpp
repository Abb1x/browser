#include "parser.hpp"
#include <algorithm>
#include <string>
#include <utils/log.hpp>
#include <wctype.h>

// Spoiler alert: most of the code is inspired by https://github.com/jackfischer/andreessen
// please stop stealing code.... this is unethical and is against POSIX..
enum state
{
    TAGNAME,
    BETWEENTAGS,
    ENDTAG
};

enum css_state
{
    CLASSNAME,
    PROPERTY,
    CODE,
    OFF
};

void Parser::add_css_to_children(DOMNode *n)
{

    for (uint i = 0; i < n->children.size(); i++)
    {
        if (n->children[i]->name != "head")
        {
            for (auto p : css[n->name])
            {
                css[n->children[i]->name][p.first] = p.second;
            }

            add_css_to_children(n->children[i]);
        }
    }
}
DOMNode *Parser::parse_html(std::string code)
{
    char current_char = code[0], last_char;
    bool parsing = true, tag_name = false, equal_sign = false, between_tag = false;

    // The number of characters and the current parsing state.
    int number_of_chars = 1, state;

    DOMNode *current_node, *root = 0;

    // This is just a temporary string.
    std::string holder, key_holder, plain_holder;

    while (parsing)
    {
        // Determining the current state of parsing, is in tag or not?

        last_char = current_char;
        current_char = code[number_of_chars];

        if (last_char == '<')
            tag_name = false;

        if ((last_char == '<' && current_char != '/' && current_char != '-' && current_char != '!'))
        {
            state = TAGNAME;
        }

        if (last_char == '>' && current_char != '<')
        {
            state = BETWEENTAGS;
        }

        if (current_char == '/' && last_char == '<')
        {
            state = ENDTAG;
        }

        switch (state)
        {
        case TAGNAME:
        {

            // Here we only want to get the letters out of the tag name and not spaces.
            if ((std::isdigit(current_char) || std::isalpha(current_char)) && current_char != ' ' && current_char != '>')
            {
                holder += current_char;
            }

            if (current_char != ' ' && current_char != '>' && current_char != '"')
            {
                plain_holder += current_char;
            }

            if (!root && last_char == '<') // If the root of the tree is undefined, let the tag be it.
            {
                DOMNode *node = new DOMNode();

                root = node;

                current_node = node;
            }

            else if (last_char == '<') // else, set the tag as child of the current node.
            {
                DOMNode *node = new DOMNode(current_node);

                current_node->add_child(node);

                current_node = node;
            }

            if ((current_char == ' ' || current_char == '>') && !tag_name) // if at the end of the tag
            {

                holder = remove_whitespace(holder); // Remove every space
                current_node->name = holder;
                if (root == current_node)
                    log(INFO, "Created new root node: %s", current_node->name.c_str());
                else
                {
                    log(INFO, "Created new node: %s with %s as parent", current_node->name.c_str(), current_node->parent->name.c_str());

                    if (!current_node->has_class)
                        current_node->parent->add_attributes_to_children();
                    else
                    {
                        current_node->add_attributes_to_children();
                    }
                }

                holder = "";
                plain_holder = "";
                tag_name = true;
            }

            if (plain_holder.size() != 0 && (current_char == ' ' || current_char == '>') && equal_sign)
            {

                current_node->add_attribute(key_holder, plain_holder);
                log(INFO, "Node \"%s\" attribute \"%s\": \"%s\"", current_node->name.c_str(), key_holder.c_str(), plain_holder.c_str());
                if (key_holder == "class")
                {
                    current_node->has_class = true;
                }

                holder = "";
                equal_sign = false;
            }

            if (current_char == '=')
            {
                equal_sign = true;
                key_holder = holder;
                holder = "";
                plain_holder = "";
            }
            break;
        }

        case BETWEENTAGS:
        {

            if (current_char == '<')
            {

                state = ENDTAG;
                between_tag = true;
                break;
            }
            holder += current_char;
            break;
        }

        case ENDTAG:
        {

            if (between_tag)
            {

                int position_b, position_e;

                for (size_t i = 0; i < holder.size(); i++)
                {
                    if (!std::iswspace(holder[i]))
                    {
                        position_b = i;
                        break;
                    }
                    else
                        position_b = i;
                }

                for (size_t i = holder.size() - 1; i >= 0; i--)
                {
                    if (!std::iswspace(holder[i]))
                    {
                        position_e = i + 1;
                        break;
                    }
                    else
                    {
                        position_e = i;
                    }
                }

                bool junk = (position_e <= position_b) ? true : false;

                if (!junk)
                {
                    holder = holder.substr(position_b, position_e);
                    current_node->text = holder;
                    between_tag = false;
                }
                else
                {
                    between_tag = false;
                }
                holder = "";
            }

            if (current_char == '>')
            {
                if (current_node == root)
                    break;

                current_node = current_node->parent;
            }
            break;
        }
        default:
            break;
        }
        number_of_chars++;

        if (number_of_chars == (int)code.length())
            parsing = false;
    }

    css = parse_css(find_css(root));
    root_node = root;
    add_css_to_children(root);
    return root;
}

DOMNode *Parser::find_node(DOMNode *base, std::string name)
{
    if (base->name == name)
    {
        return base;
    }

    else if (base->children.size() >= 1)
    {

        auto it = find_if(base->children.begin(), base->children.end(), [&](const DOMNode &obj) { return obj.name == name; });

        int pos = 0;

        if (it != base->children.end())
        {
            pos = std::distance(base->children.begin(), it);
        }

        return find_node(base->children[pos], name);
    }

    else
    {
        return NULL;
    }
}

std::string Parser::find_css(DOMNode *n)
{
    if (n->name == "style")
    {
        return n->text;
    }

    else if (n->children.size() >= 1)
    {

        auto it = find_if(n->children.begin(), n->children.end(), [&](const DOMNode &obj) { return obj.name == "style"; });

        int pos = 0;
        if (it != n->children.end())
        {
            pos = std::distance(n->children.begin(), it);
        }
        return find_css(n->children[pos]);
    }

    else
    {
        return "";
    }
}

std::map<std::string, std::map<std::string, std::string>> Parser::parse_css(std::string code)
{
    code.erase(std::remove(code.begin(), code.end(), '\n'), code.end());
    code.erase(std::remove(code.begin(), code.end(), ' '), code.end());
    code.erase(std::remove(code.begin(), code.end(), '\t'), code.end());

    std::map<std::string, std::map<std::string, std::string>> map;

    int state = OFF;

    std::string s_class, field, value, tag;

    bool b_class = false, b_tag = false;

    for (char c : code)
    {
        if (state == OFF && c == '.')
        {
            state = CLASSNAME;
            b_class = true;
            b_tag = false;
        }

        else if (state == CLASSNAME && c != '{')
        {
            s_class.push_back(c);
        }

        else if (state == OFF && c != '{')
        {
            tag.push_back(c);

            b_tag = true;
            b_class = false;
        }

        else if (state == OFF && c == '{')
        {
            state = CODE;
        }

        else if (state == CLASSNAME && c == '{')
        {
            state = CODE;
        }

        else if (state == CODE && c != ':' && c != '}')
        {
            field.push_back(c);
        }

        else if (state == CODE && c == ':')
        {
            state = PROPERTY;
        }

        else if (state == CODE && c == '}')
        {
            s_class = "";
            tag = "";

            state = OFF;
        }

        else if (state == PROPERTY && c != ';')
        {
            value.push_back(c);
        }

        else if (state == PROPERTY && c == ';')
        {
            if (b_class)
            {
                map[s_class][field] = value;
            }

            if (b_tag)
            {
                map[tag][field] = value;
            }

            field = "";
            value = "";
            state = CODE;
        }
    }

    return map;
}

std::string Parser::remove_whitespace(std::string string)
{
    int position_b;
    int position_e;

    for (int i = 0; i < (int)string.size(); i++)
    {
        if (!std::iswspace(string[i]))
        {
            position_b = i;
            break;
        }
        else
        {
            position_b = i;
        }
    }

    for (int i = string.size() - 1; i >= 0; i--)
    {
        if (!std::iswspace(string[i]))
        {
            position_e = i + 1;
            break;
        }
        else
        {
            position_e = i;
        }
    }
    return string.substr(position_b, position_e);
}
