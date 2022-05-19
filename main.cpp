#include <vector>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
#include "graphics.hpp"
#include "widgets.hpp"

using namespace genv;


struct gamestate
{
    int level;
    bool gameEnd;
    bool isFirst;
    int sizeOfGame;
    uint32_t HEIGHT;
    uint32_t WIDTH;
    uint64_t Tick = 1;
    bool Clicked;
    uint8_t* scratchMemory;
    std::vector <Widget *> w;
};

template<typename T> struct V3 {
    union {
        struct {
            T x;
            T y;
            T z;
        };
        T arr[3];
    };
};
template<typename T> struct V2 {
    union {
        struct {
            T x;
            T y;
        };
        T arr[2];
    };
    T& operator [](uint32_t i) {
        return arr[i];
    }
    V2 operator + (T scalar) {
        return { x + scalar , y + scalar };
    }
    V2 operator - (T scalar) {
        return { x - scalar , y - scalar };
    }
    V2 operator * (T scalar) {
        return { x * scalar , y * scalar };
    }
    V2 operator / (T scalar) {
        return { x / scalar , y / scalar };
    }
    V2 operator + (V2<T> other) {
        return { x + other.x , y + other.y };
    }
    V2 operator - (V2<T> other) {
        return { x - other.x , y - other.y };
    }
    V2 operator * (V2<T> other) {
        return { x * other.x , y * other.y };
    }
    V2 operator / (V2<T> other) {
        return { x / other.x , y / other.y };
    }
    bool operator == (V2<T> other) {
        return { x == other.x && y == other.y };
    }
    bool operator != (V2<T> other) {
        return { x != other.x || y != other.y };
    }
    T length() {
        return sqrt(x * x + y * y);
    }
    V2 normalize() {
        T l = length();
        return { x / l, y / l };
    }
    template <typename K>
    explicit operator K()
    {
        K product;
        product.x = (decltype(K::x))x;
        product.y = (decltype(K::y))y;
        return product;
    }
};

void SetCellClickability(V2 <int> coord, gamestate Gamestate, bool clickability)
{
    int index;
    index = coord.x + coord.y * Gamestate.sizeOfGame;
    if (coord.x < Gamestate.sizeOfGame && coord.x >= 0 && coord.y < Gamestate.sizeOfGame && coord.y >= 0)
    {
        if (Gamestate.w[index]->stateGetter() == STATE_NON)
        {
            Gamestate.w[index]->clickableSetter(clickability);
        }
    }
}

uint32_t PossibleMoves(gamestate Gamestate, uint32_t * result)
{
    int temp = 0;
    for (uint32_t i = 0; i < Gamestate.w.size(); i++)
    {
        if (Gamestate.w[i]->clickableGetter())
        {
            result[temp++] = i;
        }
    }
    return temp;
}

bool CheckWinState(gamestate Gamestate, uint32_t playerOrAi)
{
    int xpos = 0, ypos = 0;
    for (int i = 0; i < Gamestate.w.size(); i++)
    {
        if (Gamestate.w[i]->stateGetter() == playerOrAi)
        {
            uint32_t counter = 0;
            int tempy = ypos + 1;
            int tempx = xpos;
            while (tempy < Gamestate.sizeOfGame && Gamestate.w[tempx + tempy * Gamestate.sizeOfGame]->stateGetter() == playerOrAi)
            {
                tempy++;
                counter++;
                if (counter > 3)
                {
                    return true;
                }
            }
            tempy = ypos + 1;
            tempx = xpos + 1;
            counter = 0;
            uint32_t tmp = Gamestate.w[tempx + tempy * Gamestate.sizeOfGame]->stateGetter();
            while (tempy < Gamestate.sizeOfGame && tempx < Gamestate.sizeOfGame && Gamestate.w[tempx + tempy * Gamestate.sizeOfGame]->stateGetter() == playerOrAi)
            {
                tempy++;
                tempx++;
                counter++;
                if (counter > 3)
                {
                    return true;
                }
            }
            tempy = ypos;
            tempx = xpos + 1;
            counter = 0;
            while (tempx < Gamestate.sizeOfGame && Gamestate.w[tempx + tempy * Gamestate.sizeOfGame]->stateGetter() == playerOrAi)
            {
                tempx++;
                counter++;
                if (counter > 3)
                {
                    return true;
                }
            }
            tempy = ypos + 1;
            tempx = xpos - 1;
            counter = 0;
            while (tempy < Gamestate.sizeOfGame && tempx >= 0 && Gamestate.w[tempx + tempy * Gamestate.sizeOfGame]->stateGetter() == playerOrAi)
            {
                tempy++;
                tempx--;
                counter++;
                if (counter > 3)
                {
                    return true;
                }
            }
        }

        xpos++;
        if ((i + 1) % Gamestate.sizeOfGame == 0 && i != 1)
        {
            xpos = 0;
            ypos++;
        }
    }
    return false;
}

V3 <int> EndCellFinder(gamestate Gamestate, uint32_t playerOrAi, V2 <int> startingCell, V2 <int> moveDirection)
{
    int xpos = startingCell.x, ypos = startingCell.y;
    uint32_t counter = 0;
    int tempy = ypos;
    int tempx = xpos;
    while ((tempy < Gamestate.sizeOfGame && tempx < Gamestate.sizeOfGame) && Gamestate.w[tempx + tempy * Gamestate.sizeOfGame]->stateGetter() == playerOrAi)
    {
        tempy += moveDirection.y;
        tempx += moveDirection.x;
    }
    V3 <int> product = { tempx, tempy };
    if (product.x != startingCell.x)
    {
        product.z = product.x - startingCell.x;
    }
    if (product.y != startingCell.y)
    {
        product.z = product.y - startingCell.y;
    }
    return product;
}

void DoMove(gamestate Gamestate, uint32_t movement, cellState state)
{
    int cellXPos = movement % Gamestate.sizeOfGame;
    int cellYPos = movement / Gamestate.sizeOfGame;
    /*SetCellClickability({ cellXPos - 1 , cellYPos - 1 }, Gamestate, true);
    SetCellClickability({ cellXPos     , cellYPos - 1 }, Gamestate, true);
    SetCellClickability({ cellXPos + 1 , cellYPos - 1 }, Gamestate, true);
    SetCellClickability({ cellXPos - 1 , cellYPos }, Gamestate, true);
    SetCellClickability({ cellXPos     , cellYPos }, Gamestate, false);
    SetCellClickability({ cellXPos + 1 , cellYPos }, Gamestate, true);
    SetCellClickability({ cellXPos - 1 , cellYPos + 1 }, Gamestate, true);
    SetCellClickability({ cellXPos     , cellYPos + 1 }, Gamestate, true);
    SetCellClickability({ cellXPos + 1 , cellYPos + 1 }, Gamestate, true);*/
    Gamestate.w[movement]->stateSetter(state);
}


/*bool Finishable(gamestate Gamestate, uint32_t playerOrAi, V2 <int> startingCell, V2 <int> moveDirection)
{
    V2<int> cell = startingCell;

    uint32_t length0 = 0;
    for (;; length0++) {
        if (cell.x > Gamestate.sizeOfGame || cell.x < 0 || cell.y > Gamestate.sizeOfGame || cell.y < 0) break;

        uint32_t index = cell.x + cell.y * Gamestate.sizeOfGame;
        auto state = Gamestate.w[index]->stateGetter();
        if (state != 0 && state != playerOrAi) break;

        cell = cell + moveDirection;
    }
    cell = startingCell;
    uint32_t length1 = 0;
    for (;; length1++) {
        if (cell.x > Gamestate.sizeOfGame || cell.x < 0 || cell.y > Gamestate.sizeOfGame || cell.y < 0) break;

        uint32_t index = cell.x + cell.y * Gamestate.sizeOfGame;
        auto state = Gamestate.w[index]->stateGetter();
        if (state != 0 && state != playerOrAi) break;

        cell = cell - moveDirection;
    }
    return (length0 > 4) || (length1 > 4);
}*/

bool Finishable(gamestate Gamestate, uint32_t playerOrAi, V2 <int> startingCell, V2 <int> moveDirection)
{
    bool finishable1 = false, finishable2 = false;
    for (int i = 0; i < 5; i++)
    {
        startingCell = startingCell + moveDirection;
        uint32_t index = startingCell.x + startingCell.y * Gamestate.sizeOfGame;
        if ((startingCell.x < Gamestate.sizeOfGame && startingCell.y < Gamestate.sizeOfGame) && (Gamestate.w[index]->stateGetter() == playerOrAi || Gamestate.w[index]->stateGetter() == 0))
        {
            finishable1 = true;
        }
    }
    for (int i = 0; i < 5; i++)
    {
        startingCell = startingCell - moveDirection;
        uint32_t index = startingCell.x + startingCell.y * Gamestate.sizeOfGame;
        if ((startingCell.x < Gamestate.sizeOfGame && startingCell.y < Gamestate.sizeOfGame) && (Gamestate.w[index]->stateGetter() == playerOrAi || Gamestate.w[index]->stateGetter() == 0))
        {
            finishable2 = true;
        }
    }
    return (finishable1 || finishable2);
}

uint32_t PointCounter(gamestate Gamestate, uint32_t playerOrAi, int* scratchMemory)
{
    uint32_t allocator = 0;
    uint32_t size = 0;
    int xpos = 0, ypos = 0;
    uint32_t max = 0;
    bool finishable = true;
    for (int i = 0; i < Gamestate.w.size(); i++)
    {
        V2 <int> direction1 = { -1, 1 };
        V3 <int> endCell1 = EndCellFinder(Gamestate, playerOrAi, { xpos, ypos }, direction1);
        finishable = Finishable(Gamestate, playerOrAi, { xpos, ypos }, direction1);
        if (endCell1.z > max && finishable)
        {
            allocator = 0;
            int index1 = xpos + ypos * Gamestate.sizeOfGame;
            int index2 = endCell1.x + endCell1.y * Gamestate.sizeOfGame;
            scratchMemory[allocator++] = index1;
            scratchMemory[allocator++] = index2;
            max = endCell1.z;
        }
        else if (endCell1.z == max && finishable)
        {
            scratchMemory[allocator++] = xpos + ypos * Gamestate.sizeOfGame;
            scratchMemory[allocator++] = endCell1.x + endCell1.y * Gamestate.sizeOfGame;
        }

        V2 <int> direction2 = { 0, 1 };
        V3 <int> endCell2 = EndCellFinder(Gamestate, playerOrAi, { xpos, ypos }, direction2);
        finishable = Finishable(Gamestate, playerOrAi, { xpos, ypos }, direction2);
        if (endCell2.z > max && finishable)
        {
            allocator = 0;
            int index1 = xpos + ypos * Gamestate.sizeOfGame;
            int index2 = endCell2.x + endCell2.y * Gamestate.sizeOfGame;
            scratchMemory[allocator++] = index1;
            scratchMemory[allocator++] = index2;
            max = endCell2.z;
        }
        else if (endCell2.z == max && finishable)
        {
            scratchMemory[allocator++] = xpos + ypos * Gamestate.sizeOfGame;
            scratchMemory[allocator++] = endCell2.x + endCell2.y * Gamestate.sizeOfGame;
        }

        V2 <int> direction3 = { 1, 1 };
        V3 <int> endCell3 = EndCellFinder(Gamestate, playerOrAi, { xpos, ypos }, direction3);
        finishable = Finishable(Gamestate, playerOrAi, { xpos, ypos }, direction3);
        if (endCell3.z > max && finishable)
        {
            allocator = 0;
            int index1 = xpos + ypos * Gamestate.sizeOfGame;
            int index2 = endCell3.x + endCell3.y * Gamestate.sizeOfGame;
            scratchMemory[allocator++] = index1;
            scratchMemory[allocator++] = index2;
            max = endCell3.z;
        }
        else if (endCell3.z == max && finishable)
        {
            scratchMemory[allocator++] = xpos + ypos * Gamestate.sizeOfGame;
            scratchMemory[allocator++] = endCell3.x + endCell3.y * Gamestate.sizeOfGame;
        }

        V2 <int> direction4 = { 1, 0 };
        V3 <int> endCell4 = EndCellFinder(Gamestate, playerOrAi, { xpos, ypos }, direction4);
        finishable = Finishable(Gamestate, playerOrAi, { xpos, ypos }, direction4);
        if (endCell4.z > max && finishable)
        {
            allocator = 0;
            int index1 = xpos + ypos * Gamestate.sizeOfGame;
            int index2 = endCell4.x + endCell4.y * Gamestate.sizeOfGame;
            scratchMemory[allocator++] = index1;
            scratchMemory[allocator++] = index2;
            max = endCell4.z;
        }
        else if (endCell4.z == max && finishable)
        {
            scratchMemory[allocator++] = xpos + ypos * Gamestate.sizeOfGame;
            scratchMemory[allocator++] = endCell4.x + endCell4.y * Gamestate.sizeOfGame;
        }

        xpos++;
        if ((i + 1) % Gamestate.sizeOfGame == 0 && i != 1)
        {
            xpos = 0;
            ypos++;
        }
    }
    return allocator;
}

void Attack(gamestate Gamestate, uint32_t size)
{
    V2<int> beginning;
    V2<int> ending;

    beginning.x = ((int*)Gamestate.scratchMemory)[0] % Gamestate.sizeOfGame;
    beginning.y = ((int*)Gamestate.scratchMemory)[0] / Gamestate.sizeOfGame;
    ending.x = ((int*)Gamestate.scratchMemory)[1] % Gamestate.sizeOfGame;
    ending.y = ((int*)Gamestate.scratchMemory)[1] / Gamestate.sizeOfGame;
    V2 <int> moveDirection = {0,0};

    if (beginning != ending) {
        if (ending.x - beginning.x == 0) {
            moveDirection = { 0,1 };
        }
        else if (ending.x - beginning.x < 0) {
            moveDirection = { -1,1 };
        }
        else if (ending.x - beginning.x > 0 && ending.y - beginning.y > 0) {
            moveDirection = { 1,1 };
        }
        else if (ending.y - beginning.y == 0) {
            moveDirection = { 1,0 };
        }
    }
    else {
        if (Finishable(Gamestate, STATE_O, beginning, { 1,0 })) {
            moveDirection = { 1,0 };
        }
        else if (Finishable(Gamestate, STATE_O, beginning, { 1,1 })) {
            moveDirection = { 1,1 };
        }
        else if (Finishable(Gamestate, STATE_O, beginning, { 1,-1 })) {
            moveDirection = { 1,-1 };
        }
        else if (Finishable(Gamestate, STATE_O, beginning, { -1,1 })) {
            moveDirection = { -1,1 };
        }
    }
    std::cout << moveDirection[0] << " " << moveDirection[1] << " " ;
    V2 <int> startingCell = beginning;
    bool alreadyDrew = false;

    for (int i = 0; i < 5; i++) {
        startingCell = startingCell + moveDirection;
        uint32_t index = startingCell.x + startingCell.y * Gamestate.sizeOfGame;
        if (startingCell.x < Gamestate.sizeOfGame && startingCell.y < Gamestate.sizeOfGame) {
            auto state = Gamestate.w[index]->stateGetter();
            if (state == STATE_X) break;

            if (state == STATE_NON) {
                DoMove(Gamestate, index, STATE_O);
                alreadyDrew = true;
                break;
            }
        }
    }
    if (!alreadyDrew) {
        startingCell = ending;
        for (int i = 0; i < 5; i++) {
            startingCell = startingCell - moveDirection;
            uint32_t index = startingCell.x + startingCell.y * Gamestate.sizeOfGame;
            if (startingCell.x < Gamestate.sizeOfGame && startingCell.y < Gamestate.sizeOfGame) {
                auto state = Gamestate.w[index]->stateGetter();
                if (state == STATE_X) break;

                if (state == STATE_NON) {
                    DoMove(Gamestate, index, STATE_O);
                    alreadyDrew = true;
                    break;
                }
            }
        }
    }

}

/*void Attack(gamestate Gamestate, uint32_t size)
{
    int cordx1 = ((int*)Gamestate.scratchMemory)[0] % Gamestate.sizeOfGame;
    int cordy1 = ((int*)Gamestate.scratchMemory)[0] / Gamestate.sizeOfGame;
    int cordx2 = ((int*)Gamestate.scratchMemory)[1] % Gamestate.sizeOfGame;
    int cordy2 = ((int*)Gamestate.scratchMemory)[1] / Gamestate.sizeOfGame;
    V2 <int> moveDirection;
    if (cordx2 - cordx1 == 0)
    {
        moveDirection = { 0,1 };
    }
    if (cordx2 - cordx1 < 0)
    {
        moveDirection = { -1,1 };
    }
    if (cordx2 - cordx1 > 0 && cordy2 - cordy1 > 0)
    {
        moveDirection = { 1,1 };
    }
    if (cordy2 - cordy1 == 0)
    {
        moveDirection = { 1,0 };
    };
    V2 <int> startingCell;
    startingCell.x = ((int*)Gamestate.scratchMemory)[0] % Gamestate.sizeOfGame;
    startingCell.y = ((int*)Gamestate.scratchMemory)[0] / Gamestate.sizeOfGame;
    bool alreadyDrew = false;
    for (int i = 0; i < 5; i++)
    {
        startingCell.x += moveDirection.x;
        uint32_t index = startingCell.x + startingCell.y * Gamestate.sizeOfGame;
        if ((startingCell.x < Gamestate.sizeOfGame && startingCell.y < Gamestate.sizeOfGame) && Gamestate.w[index]->stateGetter() == 0)
        {
            Gamestate.w[index]->stateSetter(STATE_O);
            alreadyDrew = true;
            break;
        }
    }
    if (alreadyDrew = false)
    {
        for (int i = 0; i < 5; i++)
        {
            startingCell = startingCell - moveDirection;
            uint32_t index = startingCell.x + startingCell.y * Gamestate.sizeOfGame;
            if ((startingCell.x < Gamestate.sizeOfGame && startingCell.y < Gamestate.sizeOfGame) && Gamestate.w[index]->stateGetter() == 0)
            {
                Gamestate.w[index]->stateSetter(STATE_O);
            }
        }
    }

}*/

void Defend(gamestate Gamestate, uint32_t size)
{
    int cordx1 = ((int*)Gamestate.scratchMemory)[size] % Gamestate.sizeOfGame;
    int cordy1 = ((int*)Gamestate.scratchMemory)[size] / Gamestate.sizeOfGame;
    int cordx2 = ((int*)Gamestate.scratchMemory)[size + 1] % Gamestate.sizeOfGame;
    int cordy2 = ((int*)Gamestate.scratchMemory)[size + 1] / Gamestate.sizeOfGame;
    V2 <int> moveDirection;
    if (cordx2 - cordx1 == 0)
    {
        moveDirection = { 0,1 };
    }
    if (cordx2 - cordx1 < 0)
    {
        moveDirection = { -1,1 };
    }
    if (cordx2 - cordx1 > 0 && cordy2 - cordy1 > 0)
    {
        moveDirection = { 1,1 };
    }
    if (cordy2 - cordy1 == 0)
    {
        moveDirection = { 1,0 };
    };

    V2 <int> startingCell;
    startingCell.x = ((int*)Gamestate.scratchMemory)[size] % Gamestate.sizeOfGame;
    startingCell.y = ((int*)Gamestate.scratchMemory)[size] / Gamestate.sizeOfGame;
    bool alreadyDrew = false;
    for (int i = 0; i < 5; i++)
    {
        startingCell.x += moveDirection.x;
        uint32_t index = startingCell.x + startingCell.y * Gamestate.sizeOfGame;
        if ((startingCell.x < Gamestate.sizeOfGame && startingCell.y < Gamestate.sizeOfGame) && Gamestate.w[index]->stateGetter() == STATE_NON)
        {
            Gamestate.w[index]->stateSetter(STATE_O);
            alreadyDrew = true;
            break;
        }
    }
    if (alreadyDrew = false)
    {
        for (int i = 0; i < 5; i++)
        {
            startingCell = startingCell - moveDirection;
            uint32_t index = startingCell.x + startingCell.y * Gamestate.sizeOfGame;
            if ((startingCell.x < Gamestate.sizeOfGame && startingCell.y < Gamestate.sizeOfGame) && Gamestate.w[index]->stateGetter() == 0)
            {
                Gamestate.w[index]->stateSetter(STATE_O);
            }
        }
    }
}

void AIUpdate(gamestate Gamestate)
{
    V2 <int> aiFirstCoordinate, aiSecondCoordinate, playerFirstCoordinate, playerSecondCoordinate;
    uint32_t size = PointCounter(Gamestate, STATE_O, (int*)(Gamestate.scratchMemory));

    aiFirstCoordinate.y = ((int*)Gamestate.scratchMemory)[0] / Gamestate.sizeOfGame;
    aiFirstCoordinate.x = ((int*)Gamestate.scratchMemory)[0] % Gamestate.sizeOfGame;
    aiSecondCoordinate.y = ((int*)Gamestate.scratchMemory)[1] / Gamestate.sizeOfGame;
    aiSecondCoordinate.x = ((int*)Gamestate.scratchMemory)[1] % Gamestate.sizeOfGame;

    int aiChainLength = 0;
    if (aiFirstCoordinate.x != aiSecondCoordinate.x)
    {
        aiChainLength = aiSecondCoordinate.x - aiFirstCoordinate.x;
    }
    if (aiFirstCoordinate.y != aiSecondCoordinate.y)
    {
        aiChainLength = aiSecondCoordinate.y - aiFirstCoordinate.y;
    }
    PointCounter(Gamestate, STATE_X, ((int*)Gamestate.scratchMemory) + size);
    playerFirstCoordinate.y = ((int*)Gamestate.scratchMemory)[size] / Gamestate.sizeOfGame;
    playerFirstCoordinate.x = ((int*)Gamestate.scratchMemory)[size] % Gamestate.sizeOfGame;
    playerSecondCoordinate.y = ((int*)Gamestate.scratchMemory)[size + 1] / Gamestate.sizeOfGame;
    playerSecondCoordinate.x = ((int*)Gamestate.scratchMemory)[size + 1] % Gamestate.sizeOfGame;
    int playerChainLength = 0;
    if (playerFirstCoordinate.x != playerSecondCoordinate.x)
    {
        playerChainLength = playerSecondCoordinate.x - playerFirstCoordinate.x;
    }
    if (playerFirstCoordinate.y != playerSecondCoordinate.y)
    {
        playerChainLength = playerSecondCoordinate.y - playerFirstCoordinate.y;
    }

    for (uint32_t i = 0; i < size; i += 2) {
        int x0 = ((uint32_t*)Gamestate.scratchMemory)[i + 0] % Gamestate.sizeOfGame;
        int y0 = ((uint32_t*)Gamestate.scratchMemory)[i + 0] / Gamestate.sizeOfGame;
        int x1 = ((uint32_t*)Gamestate.scratchMemory)[i + 1] % Gamestate.sizeOfGame;
        int y1 = ((uint32_t*)Gamestate.scratchMemory)[i + 1] / Gamestate.sizeOfGame;
    }

    if (aiChainLength >= 0)
    {
        Attack(Gamestate, size);
    }
    else if (playerChainLength > 6)
    {
        Defend(Gamestate, size);
    }
}



class checkBox : public Widget
{
public:
    checkBox(int x, int y, int sx, int sy) : Widget(x, y, sx, sy)
    {
        clickable = false;
    }
    virtual void draw() override
    {
        gout << move_to(_x, _y) << color(r, g, b) << box(_sx, _sy);
        gout << move_to(_x+1, _y+1) << color(0,0,0) << box(_sx-2, _sy-2);
        if (state == STATE_X)
        {
            gout << move_to(_x + (_sx / 2), _y + (_sy / 2)) << color(255,0,0) << text("X");
        }

        if (state == STATE_O)
        {
            gout << move_to(_x + (_sx / 2), _y + (_sy / 2)) << color(255,0,0) << text("O");
        }
    }

    virtual void handle (genv::event ev, gamestate Gamestate, int i) override
    {
         if (Gamestate.Tick % 2 == 0)
         {
             state = STATE_X;
         }
         else if (Gamestate.Tick % 2 == 1)
         {
             state = STATE_O;
         }
         /*state = STATE_X;
         clickable = false;*/
         //AIUpdate(Gamestate);
    }
    virtual void handle(genv::event ev) override
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
    virtual void handle (genv::event ev, gamestate Gamestate, int i) override
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


void event_loop(gamestate Gamestate) {
    gout << refresh;
    event evt;
    int focus = -1;
    while(gin >> evt )
        {
        if (evt.type == ev_mouse && evt.button==btn_left) {
            for (size_t i=0;i<Gamestate.w.size();i++) {
                if (Gamestate.w[i]->is_selected(evt.pos_x, evt.pos_y) && Gamestate.w[i]->clickableGetter()) {
                        focus = i;
                        Gamestate.Clicked = true;
                        Gamestate.Tick++;
                        std::cout << Gamestate.Tick;
                }
            }
        }
        if (focus!=-1) {
            Gamestate.w[focus]->handle(evt,Gamestate, focus);
            Gamestate.w[focus]->rSetter(255);
            Gamestate.w[focus]->gSetter(0);
            Gamestate.w[focus]->bSetter(0);
        }
        for (Widget * a : Gamestate.w) {
            a->draw();
            a->rSetter(255);
            a->gSetter(255);
            a->bSetter(255);
        }
            if (Gamestate.level == 2)
            {
                int cellXPos = 0, cellYPos = 0;
                for (int i = 0; i < Gamestate.w.size(); i++)
                {
                    if (Gamestate.w[i]->stateGetter() == STATE_O)
                        {
                            Gamestate.w[i]->clickableSetter(false);
                            /*SetCellClickability({ cellXPos - 1, cellYPos - 1 }, Gamestate, true);
                            SetCellClickability({ cellXPos , cellYPos - 1 }, Gamestate, true);
                            SetCellClickability({ cellXPos + 1, cellYPos - 1 }, Gamestate, true);
                            SetCellClickability({ cellXPos - 1, cellYPos }, Gamestate, true);
                            SetCellClickability({ cellXPos + 1, cellYPos }, Gamestate, true);
                            SetCellClickability({ cellXPos - 1, cellYPos + 1 }, Gamestate, true);
                            SetCellClickability({ cellXPos , cellYPos + 1 }, Gamestate, true);
                            SetCellClickability({ cellXPos + 1, cellYPos + 1 }, Gamestate, true);*/
                        }
                    if  ((i + 1) % Gamestate.sizeOfGame == 0 && i != 1)
                        {
                            cellYPos += 1;
                            cellXPos = 0;
                        }
                }
                /*if (Gamestate.Clicked == true)
                {
                    AIUpdate(Gamestate);
                    Gamestate.Clicked = false;
                    std::cout << " AIUPDATE ";
                }*/
                if (CheckWinState(Gamestate, STATE_X) == true)
                {
                    Gamestate.gameEnd = true;
                    std::cout << "Player won!" << Gamestate.gameEnd;
                    break;
                }
               if (CheckWinState(Gamestate, STATE_O) == true)
                {
                    Gamestate.gameEnd = true;
                    std::cout << "AI won!" << Gamestate.gameEnd;
                    break;
                }
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

void Update(gamestate Gamestate)
{
    event evt;
    while (!Gamestate.gameEnd)
    {
        clear_screen(Gamestate);
        switch(Gamestate.level)
        {
        case 1:
            {

                    NumberSetter * asd = new NumberSetter(Gamestate.WIDTH / 2 - 30, Gamestate.HEIGHT / 2 + 40, 60,60,15);
                    gout << move_to (Gamestate.WIDTH / 2 - 77, Gamestate.HEIGHT / 2 + 35) << color (255,255,255) << text("Size of the map is :");
                    Gamestate.w.push_back(asd);
                    Gamestate.w[0]->clickableSetter(true);
                    gout << move_to (Gamestate.WIDTH / 2 -77, Gamestate.HEIGHT / 2 - 50) << color (255, 255, 255) << text("Press space to start");
                    event_loop(Gamestate);
                    Gamestate.w.pop_back();
                    Gamestate.sizeOfGame = asd->tempGetter();
                    Gamestate.level++;
                    clear_screen(Gamestate);
            }
        case 2:
            {
                for (int i = 0; i < Gamestate.sizeOfGame; i++)
                    {
                        for (int k = 0; k < Gamestate.sizeOfGame; k++)
                            {
                                checkBox * asd = new checkBox((Gamestate.WIDTH / Gamestate.sizeOfGame) * k, (Gamestate.HEIGHT / Gamestate.sizeOfGame) * i, (Gamestate.WIDTH / Gamestate.sizeOfGame), Gamestate.HEIGHT / Gamestate.sizeOfGame);
                                asd->clickableSetter(true);
                                asd->stateSetter(STATE_NON);
                                Gamestate.w.push_back(asd);
                            }
                    }
                int randomNumber = rand() % (Gamestate.sizeOfGame * Gamestate.sizeOfGame);
                DoMove(Gamestate, randomNumber, STATE_O);
                event_loop(Gamestate);
                Gamestate.level++;
                break;
            };
        case 3:
            {
                std::cout << "Game Over!";
                Gamestate.gameEnd = true;
                break;
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
    Gamestate.scratchMemory = (uint8_t*)malloc(16 * 1000 * 1000);
    event evt;
    init(Gamestate);
    std::vector <Widget *> w;
    Update(Gamestate);

    return 0;
};
