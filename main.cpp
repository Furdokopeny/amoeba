#include <vector>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "graphics.hpp"
#include "widgets.hpp"

using namespace genv;

enum cellState : uint16_t
{
    STATE_EMPTY,
    STATE_X,
    STATE_O
};

struct gamestate
{
    int level;
    bool gameEnd;
    bool isFirst;
    int sizeOfGame;
    uint32_t HEIGHT;
    uint32_t WIDTH;
    uint64_t Tick;
};

class checkBox : public Widget
{
protected:
    bool clickable;
    cellState state;

public:
    checkBox(int x, int y, int sx, int sy) : Widget(x, y, sx, sy)
    {

    }
    virtual void draw() override
    {

    }

    virtual void handle (genv::event ev) override
    {

    }
    virtual bool is_selected(int mouse_x, int mouse_y)
    {
        return mouse_x>_x && mouse_x<_x+_sx && mouse_y>_y && mouse_y<_y+_sy;
    }
};

class NumberSetter : public Widget
{
protected:
    int temp;
    int interval;

public:
    NumberSetter(int x, int y, int sx, int sy, int _interval) : Widget(x, y, sx, sy)
    {
        temp = 15;
        interval = _interval;
        r = 255;
        g = 255;
        b = 255;
    }
    virtual int tempGetter()
    {
        return temp;
    }
    virtual void draw() override
    {
        gout << move_to(_x, _y) << color(r, g, b) << box(_sx, _sy);
        gout << move_to(_x+2, _y+2) << color(0,0,0) << box(_sx-4, _sy-4);
        gout << move_to(_x + _sx / 2 - 4, _y + _sy /2) << color (0, 255, 0) << text(std::to_string(temp));
        gout << move_to(_x - 30, _y) << color(255,0,0) << box (25, _sy);
        gout << move_to(_x - 20, _y + (_sy / 2)) << color(0,255, 0) <<  text("-");
        gout << move_to(_x + _sx + 5 , _y) << color(255,0,0) << box ( 25, _sy);
        gout << move_to(_x + _sx + 14, _y + (_sy / 2)) << color(0,255, 0) <<  text("+");
    }

    virtual void handle(genv::event ev) override

    {
        if (ev.pos_x >= _x - 30 && ev.pos_x <= _x - 5 && ev.pos_y >= _y && ev.pos_y <= _y + _sy && ev.type == ev_mouse && ev.button==btn_left)
        {
            temp--;
        }
        if (ev.pos_x >= _x + _sx + 5 && ev.pos_x <= _x + _sx + 30 && ev.pos_y >= _y && ev.pos_y <= _y + _sy && ev.type == ev_mouse && ev.button==btn_left)
        {
            temp++;
        }
        if (temp > interval + 15)
        {
            temp = interval + 15;
        }
        if (temp < interval)
        {
            temp = interval;
        }

    }
    virtual bool is_selected(int mouse_x, int mouse_y)
    {
        return mouse_x>_x - 30 && mouse_x<_x+_sx + 30 && mouse_y>_y && mouse_y<_y+_sy;
    }

};

void clear_screen(gamestate Gamestate)
{
    gout << move_to(0,0) << color(0,0,0) << box(Gamestate.WIDTH, Gamestate.HEIGHT);
    gout << refresh;
}

void event_loop(std::vector<Widget*>& widgets,gamestate Gamestate) {
    event evt;
    int focus = -1;
    while(gin >> evt )
        {
        if (evt.type == ev_mouse && evt.button==btn_left) {
            for (size_t i=0;i<widgets.size();i++) {
                if (widgets[i]->is_selected(evt.pos_x, evt.pos_y)) {
                        focus = i;
                }
            }
        }
        if (focus!=-1) {
            widgets[focus]->handle(evt);
            widgets[focus]->rSetter(255);
            widgets[focus]->gSetter(0);
            widgets[focus]->bSetter(0);
        }
        for (Widget * w : widgets) {
            w->draw();
            w->rSetter(255);
            w->gSetter(255);
            w->bSetter(255);
        }
        gout << refresh;
        if (evt.keycode == key_space)
        {
            Gamestate.level++;
            break;
        }
    }
}

void init(gamestate Gamestate)
{
    gout.open(Gamestate.WIDTH, Gamestate.HEIGHT);
}

void running(std::vector<Widget*>& widgets, gamestate Gamestate)
{
    for (Widget * wg : widgets) {
        wg->draw();
    }
    gout << refresh;
    event_loop(widgets, Gamestate);
}

void update(std::vector<Widget*>& widgets, gamestate Gamestate)
{
    event evt;
    while (gin >> evt)
    {
        clear_screen(Gamestate);
        switch(Gamestate.level)
        {
        case 1:
            {

                    NumberSetter * asd = new NumberSetter(Gamestate.WIDTH / 2 - 30, Gamestate.HEIGHT / 2 + 40, 60,60,15);
                    gout << move_to (Gamestate.WIDTH / 2 - 77, Gamestate.HEIGHT / 2 + 35) << color (255,0,0) << text("Size of the map is :");
                    widgets.push_back(asd);
                    gout << move_to (Gamestate.WIDTH / 2 -77, Gamestate.HEIGHT / 2 - 50) << color (255, 0, 0) << text("Press space to start");
                    event_loop(widgets, Gamestate);
                    widgets.pop_back();
                    Gamestate.sizeOfGame = asd->tempGetter();
                    std::cout << Gamestate.sizeOfGame << " " << widgets.size();
                    Gamestate.level++;
                    clear_screen(Gamestate);
            }
        case 2:
            {

                std::cout << Gamestate.level;

            }
        }
    }
}



int main()
{
    gamestate Gamestate;
    Gamestate.gameEnd = false;
    Gamestate.level = 1;
    Gamestate.HEIGHT = 600;
    Gamestate.WIDTH = 1000;
    event evt;
    init(Gamestate);


    std::vector <Widget *> w;
    update(w, Gamestate);


    return 0;
};
