#ifndef WIDGETS_HPP_INCLUDED
#define WIDGETS_HPP_INCLUDED

#include "graphics.hpp"
#include "D:\prog\GraphicsKit64\hf\application.h"

class Widget {
protected:
    int _x, _y, _sx, _sy;
    int r, g, b;
    bool selected;
public:
    Widget (int x, int y, int sx, int sy)
    {
        _x = x;
        _y = y;
        _sx = sx;
        _sy = sy;
    };
    virtual bool is_selected(int mouse_x, int mouse_y) = 0;
    void rSetter(int _r)
    {
        r = _r;
    }
    void gSetter(int _g)
    {
        g = _g;
    }
    void bSetter(int _b)
    {
        b = _b;
    }
    virtual void draw() = 0;
    virtual void handle(genv::event ev) = 0;
};

                                                                                                                                                                                                                                                                  //szeretnek pontlevonast kerni a beadandomra mert feltoltottem egy oktatasi segedletet sajat munka helyett :)

#endif // WIDGETS_HPP_INCLUDED
