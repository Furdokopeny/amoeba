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

class NumberSetter : public Widget
{
protected:
    int temp;
    int interval;

public:
    NumberSetter(int x, int y, int sx, int sy, int _interval) : Widget(x, y, sx, sy)
    {
        temp = 0;
        interval = _interval;
        r = 255;
        g = 255;
        b = 255;
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
        if (ev.type == ev_key && (ev.keycode == key_backspace)) {
            temp-=10;
        }
        if (ev.type == ev_key && (ev.keycode == key_enter)) {
            temp+=10;
        }
        if (ev.type == ev_key && (ev.keycode == key_space)) {
            temp+=1;
        }
        if (ev.type == ev_key && (ev.keycode == key_lctrl)) {
            temp-=1;
        }
        if (ev.pos_x >= _x - 30 && ev.pos_x <= _x - 5 && ev.pos_y >= _y && ev.pos_y <= _y + _sy && ev.type == ev_mouse && ev.button==btn_left)
        {
            temp--;
        }
        if (ev.pos_x >= _x + _sx + 5 && ev.pos_x <= _x + _sx + 30 && ev.pos_y >= _y && ev.pos_y <= _y + _sy && ev.type == ev_mouse && ev.button==btn_left)
        {
            temp++;
        }
        if (temp > interval)
        {
            temp = interval;
        }
        if (temp < -1 * interval)
        {
            temp = -1 * interval;
        }

    }
    virtual bool is_selected(int mouse_x, int mouse_y)
    {
        return mouse_x>_x - 30 && mouse_x<_x+_sx + 30 && mouse_y>_y && mouse_y<_y+_sy;
    }

};


class DropDownMenu : public Widget
{
protected:
    int interval;
    int currentChoicePlace = 0;
    std::string currentChoice = "...";
    std::vector <std::string> choices;
    int maximum;
    int actioncounter = 2;

public:
    DropDownMenu(int x, int y, int sx, int sy, int _interval, std::vector <std::string> _choicesInput) : Widget(x, y, sx, sy)
    {
        interval = _interval;
        for ( int i = 0 ; i < _choicesInput.size(); i++)
        {
            choices.push_back(_choicesInput[i]);
            r = 255;
            g = 255;
            b = 255;
            maximum = _choicesInput.size();
        }

    }
    virtual void draw() override
    {
        gout << move_to(_x, _y) << color(r, g, b) << box(_sx, _sy);
        gout << move_to(_x+2, _y+2) << color(0,0,0) << box(_sx-4, _sy-4);
        gout << move_to(_x + _sx / 2 - 30, _y + _sy /2) << color (0, 255, 0) << text(currentChoice);
        if (!actioncounter % 2 == 0)
        {
            for (int i = 0; i < interval; i++)
            {
                gout << move_to(_x, _y + (_sy * (i + 1))) << color(r,g,b) << box(_sx, _sy);
                gout << move_to(_x+2, _y + ((i + 1) * _sy) +2) << color(0,0,0) << box(_sx-4, _sy-4);
                gout << move_to (_x + (_sx / 2)- 30, _y + ((_sy * (i + 1)+ 30))) << color(0, 255, 0) << text(choices[i + currentChoicePlace]);
            }
        }
        if (actioncounter % 2 == 0)
        {
            for (int i = 0; i < interval; i++)
            {
                gout << move_to(_x, _y + (_sy * (i + 1))) << color(0,0,0) << box(_sx, _sy);
                gout << move_to(_x+2, _y + ((i + 1) * _sy) +2) << color(0,0,0) << box(_sx-4, _sy-4);
            }
        }
    }

    virtual void handle (genv::event ev) override
    {
        if (ev.type == ev_mouse && ev.button == btn_wheelup)
        {
            currentChoicePlace--;
            if (currentChoicePlace < 1)
            {
                currentChoicePlace = 0;
            }
        }
        if (ev.type == ev_mouse && ev.button == btn_wheeldown)
        {
            currentChoicePlace++;
            if (currentChoicePlace > interval)
            {
                currentChoicePlace = interval;
            }
        }
        if (ev.type == ev_mouse && ev.button==btn_left)
        {
            actioncounter++;
            for (int i = 0; i < interval; i++)
            {
                gout << move_to(_x, _y + (_sy * (i + 1))) << color(r, g, b) << box(_sx, _sy);
                gout << move_to(_x+2, _y + ((i + 1) * _sy) +2) << color(0,0,0) << box(_sx-4, _sy-4);
                if (ev.pos_x >= _x && ev.pos_x <= _x + _sx  && ev.pos_y >= _y + (_sy * (i + 1)) && ev.pos_y <= _y + (_sy * (i + 2)))
                {
                    currentChoice = choices[i + currentChoicePlace];
                }
            }
        }
    }
    virtual bool is_selected(int mouse_x, int mouse_y)
    {
        return mouse_x>_x && mouse_x<_x+_sx && mouse_y>_y && mouse_y<_y+_sy;
    }
};


class textOutput : public Widget
{
protected:
    int interval;
    int currentChoicePlace = 0;
    std::string currentChoice = "asd";
    std::vector <std::string> choices;
    int maximum;

public:
    textOutput(int x, int y, int sx, int sy, int _interval, std::vector <std::string> _choicesInput) : Widget(x, y, sx, sy)
    {
        interval = _interval;
        for ( int i = 0 ; i < _choicesInput.size(); i++)
        {
            choices.push_back(_choicesInput[i]);
            r = 255;
            g = 255;
            b = 255;
            maximum = _choicesInput.size();
        }
        gout << move_to(_x, _y) << color(r,g,b) << box(_sx, _sy);
        gout << move_to(_x + 2, _y + 2) << color(0,0,0) << box(_sx-4, _sy-4);
        gout << move_to(_x - 30, _y) << color(255,0,0) << box (25, _sy);
        gout << move_to(_x - 20, _y + (_sy / 2)) << color(0,255, 0) <<  text("-");
        gout << move_to(_x + _sx + 5 , _y) << color(255,0,0) << box ( 25, _sy);
        gout << move_to(_x + _sx + 14, _y + (_sy / 2)) << color(0,255, 0) <<  text("+");
            for (int i = 0; i < interval; i++)
            {
                gout << move_to (_x, _y + ((i + 1) *30 )) << color(0, 255, 0) << text(choices[i + currentChoicePlace]);
            }
    }
    virtual void draw() override
    {
        gout << move_to(_x, _y) << color(r,g,b) << box(_sx, _sy);
        gout << move_to(_x + 2, _y + 2) << color(0,0,0) << box(_sx-4, _sy-4);
        gout << move_to(_x - 30, _y) << color(255,0,0) << box (25, _sy);
        gout << move_to(_x - 20, _y + (_sy / 2)) << color(0,255, 0) <<  text("-");
        gout << move_to(_x + _sx + 5 , _y) << color(255,0,0) << box ( 25, _sy);
        gout << move_to(_x + _sx + 14, _y + (_sy / 2)) << color(0,255, 0) <<  text("+");
            for (int i = 0; i < interval; i++)
            {
                gout << move_to (_x, _y + ((i + 1) *30 )) << color(0, 255, 0) << text(choices[i + currentChoicePlace]);
            }
    }

    virtual void handle (genv::event ev) override
    {
        if (ev.type == ev_key && ev.keycode == key_enter)
        {
            currentChoicePlace++;
        }
        if (ev.type == ev_key && ev.keycode == key_backspace)
        {
            currentChoicePlace--;
        }
        if (ev.pos_x >= _x - 30 && ev.pos_x <= _x - 5 && ev.pos_y >= _y && ev.pos_y <= _y + _sy && ev.type == ev_mouse && ev.button==btn_left)
        {
            currentChoicePlace--;
        }
        if (ev.pos_x >= _x + _sx + 5 && ev.pos_x <= _x + _sx + 30 && ev.pos_y >= _y && ev.pos_y <= _y + _sy && ev.type == ev_mouse && ev.button==btn_left)
        {
            currentChoicePlace++;
        }
        if (currentChoicePlace < 0)
            {
                currentChoicePlace = 0;
            }
        if (currentChoicePlace > interval)
            {
                currentChoicePlace = interval;
            }
    }
    virtual bool is_selected(int mouse_x, int mouse_y)
    {
        return mouse_x>_x && mouse_x<_x+_sx && mouse_y>_y && mouse_y<_y+_sy;
    }
};



void clear_screen()
{
    gout << move_to(0,0) << color(0,0,0) << box_to(800, 600);
}

void event_loop(std::vector<Widget*>& widgets) {
    event evt;
    int focus = -1;
    while(gin >> evt ) {
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
        if (evt.keycode == key_escape)
        {
            break;
        }
    }
}

void init(int width, int height)
{
    gout.open(width, height);

}

void running(std::vector<Widget*>& widgets)
{
    for (Widget * wg : widgets) {
        wg->draw();
    }
    gout << refresh;
    event_loop(widgets);
}

int main()
{
    init(1000, 600);
    event ev;

    std::vector <std::string> test1;
    test1.push_back("test 1.1");
    test1.push_back("test 1.2");
    test1.push_back("test 1.3");
    test1.push_back("test 1.4");
    test1.push_back("test 1.5");
    test1.push_back("test 1.6");

    std::vector <std::string> test2;
    test2.push_back("test 2.1");
    test2.push_back("test 2.2");
    test2.push_back("test 2.3");
    test2.push_back("test 2.4");
    test2.push_back("test 2.5");
    test2.push_back("test 2.6");
    test2.push_back("test 2.7");
    test2.push_back("test 2.8");

    std::vector<Widget*> w;
    NumberSetter * first = new NumberSetter(50,50,50,50,10);
    NumberSetter * second = new NumberSetter(50,200,50,50,20);
    DropDownMenu * third = new DropDownMenu(250, 50, 150, 50, 3, test1);
    DropDownMenu * fourth = new DropDownMenu(450, 50, 150, 50, 4, test2);
    textOutput * fifth = new textOutput(50, 300, 150, 150, 4, test2);
    w.push_back(first);
    w.push_back(second);
    w.push_back(third);
    w.push_back(fourth);
    w.push_back(fifth);

    while (!(ev.type == ev_key && ev.keycode == key_backspace))
    {
        running (w);
    }


    /*for (Widget * wg : w) {
        wg->draw();
    }
    gout << refresh;
    event_loop(w);*/

    return 0;
}
