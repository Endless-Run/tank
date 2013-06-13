#include "Window.hpp"

#include <iostream>
#include "Game.hpp"


bool Window::windowExists_ = false;

Window::Window(int width, int height, int flags)
    :caption_(""),
    size_({width, height}),
    valid_(false)
{
    if(!windowExists_)
    {
        valid_ = true;
        bool success = true;

        Game::log() << "Opening Window" << std::endl;

        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            Game::log() << "OH GOD, SDL REALLY COCKED UP PROPER: " << SDL_GetError() << std::endl;
            success = false;
        }

        if(IMG_Init(IMG_INIT_PNG) == -1)
        {
            Game::log() << "Something went wrong: " << IMG_GetError() << std::endl;
            success = false;
        }

        if(SDL_SetVideoMode(width, height, 32, flags) == NULL)
        {
            Game::log() << "OH NOEZ: " << SDL_GetError() << std::endl;
            success = false;
        }

        windowExists_ = success;
    }
}

Window::~Window()
{
    if(windowExists_ && valid_)
    {
        Game::log() << "Closing Window" << std::endl;

        IMG_Quit();
        SDL_Quit();
    }
}

void Window::setWidth(int width, int height)
{
    //Unfortunately SDL will create a new window, which is generally
    //undesired, so this function shouldn't do anything
}

void Window::setIcon(std::string&& path)
{
    if(windowExists_ && valid_)
    {
        SDL_WM_SetIcon(IMG_Load(path.c_str()), NULL);
    }
}

void Window::setCaption(std::string&& caption)
{
    if(windowExists_ && valid_)
    {
        SDL_WM_SetCaption(caption.c_str(), NULL);
    }
}

Vectori const& Window::getSize()
{
    return size_;
}

std::string const& Window::getCaption()
{
    return caption_;
}
