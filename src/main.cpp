#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "packEngine.h"

#include "cGUI.h"
#include "sProblem.h"

typedef raven::pack::cItem box_t;

sQuadrant::sQuadrant()
{
    clear();
}

void sQuadrant::clear()
{
    myBoxes.clear();
    E.setSize(1000, 1000);
}

void sQuadrant::bestSpaceAlgo( raven::pack::cEngine::eBestSpaceAlgo algo )
{
    E.setBestSpaceAlgo( algo );
}

// int sQuadrant::findBestSpace(const cBox &box)
// {
//     if (!myBoxes.size())
//         return 0;

//     int bestSpaceIndex = 0;
//     double leastWastage = INT_MAX;
//     double leastDistance = INT_MAX;

//     for (int s = 0; s < mySpaces.size(); s++)
//     {
//         // check for remains of a split space
//         if (mySpaces[s].loc.x < 0)
//             continue;
//         // check that space is tall enough for box
//         if (mySpaces[s].wh.y < box.wh.y)
//             continue;

//         // the box could be fitted into this space
//         // apply best space algorithm

//         switch (sProblem::bestSpace())
//         {

//         case sProblem::eBestSpace::firstFit:
//             return s;
//             break;

//         case sProblem::eBestSpace::minGap:
//         {
//             double wastage = mySpaces[s].wh.y - box.wh.y;
//             if (wastage < leastWastage)
//             {
//                 leastWastage = wastage;
//                 bestSpaceIndex = s;
//             }
//         }
//         break;

//         case sProblem::eBestSpace::minDist:
//         {
//             double distance = mySpaces[s].loc.x + mySpaces[s].loc.y;
//             if (distance < leastDistance)
//             {
//                 leastDistance = distance;
//                 bestSpaceIndex = s;
//             }
//         }
//         break;
//         }
//     }


void sQuadrant::pack(raven::pack::cItem &box)
{
    E.pack(box);

    /* Add the box to the quadrant

    Each quadrant keeps a copy of the boxes packed into it
    This is profligate with memory,
    but makes the code simpler when rotating the quadrants into their final positions.
    It should be of no concern unless there are millions of boxes
    */
    myBoxes.push_back(box);
}

cxy sQuadrant::bottomright(const raven::pack::cItem &rect) const
{
    return cxy(
        rect.loc.x + rect.wlh.x,
        rect.loc.y + rect.wlh.y);
}
cxy sQuadrant::topright(const raven::pack::cItem &rect) const
{
    return cxy(
        rect.loc.x + rect.wlh.x,
        rect.loc.y);
}
cxy sQuadrant::bottomleft(const raven::pack::cItem &rect) const
{
    return cxy(
        rect.loc.x,
        rect.loc.y + rect.wlh.y);
}

int sQuadrant::maxDim() const
{
    if (!myBoxes.size())
        return 50;
    int ret = 0;
    for (auto &box : myBoxes)
    {
        cxy br = bottomright(box);
        if (br.x > ret)
            ret = br.x;
        if (br.y > ret)
            ret = br.y;
    }
    return ret;
}

void sQuadrant::rotate(int index)
{
    double temp;
    switch (index)
    {
    case 0:
    {
        for (auto &B : myBoxes)
        {
            cxy br = bottomright(B);
            B.move(-br.x, -br.y);
        }
    }
    break;

    case 1:
    {
        for (auto &B : myBoxes)
        {
            cxy tr = topright(B);
            B.move(tr.y, -tr.x);
            temp = B.wlh.x;
            B.wlh.x = B.wlh.y;
            B.wlh.y = temp;
        }
    }
    break;

    case 2:
        return;

    case 3:
    {
        for (auto &B : myBoxes)
        {
            cxy bl = bottomleft(B);
            B.move(-bl.y, bl.x);
            temp = B.wlh.x;
            B.wlh.x = B.wlh.y;
            B.wlh.y = temp;
        }
    }
    break;
    default:
        throw std::runtime_error(
            "sTriangle::rotate bad quadrant");
    }
}

void sProblem::input(const std::string &sin)
{
    std::istringstream is(sin);
    int w, h;
    myBoxes.clear();
    int userID = 0;
    is >> w >> h;
    while (is.good())
    {
        myBoxes.emplace_back( w, h);
        userID++;
        is >> w >> h;
    }
}

std::string sProblem::output() const
{
    std::stringstream ss;
    for (int q = 0; q < 4; q++)
    {
        for (auto &B : myQuads[q].myBoxes)
        {
            // ss << B.userID << " "
             ss  << B.loc.x << " "
               << B.loc.y << "\n";
        }
    }
    return ss.str();
}

void sProblem::genRandom(int min, int max, int count)
{
    srand(77);
    myBoxes.clear();
    for (int k = 0; k < count; k++)
    {
        double x = rand() % (max - min) + min;
        double y = rand() % (max - min) + min;
        myBoxes.emplace_back(x, y);

        std::cout << "( " << x << " " << y << "),";
    }
}

raven::pack::cEngine::eBestSpaceAlgo sProblem::myBestSpace;

sProblem::sProblem()
{
    myBestSpace = raven::pack::cEngine::eBestSpaceAlgo::firstFit;

    // construct 4 quadrants around central point
    myQuads.resize(4);
}

void sProblem::sort()
{
    std::sort(
        myBoxes.begin(), myBoxes.end(),
        [](const box_t &a, const box_t &b)
        {
            return a.volume() > b.volume();
        });
}
void sProblem::pack()
{
    /* Sort boxes into order of decreasing volume

    This permits the smaller boxes to be packed into
    the spaces left behind by the larger boxes previously packed.

    */
    sort();

    // clear the quadrants

    for (auto &q : myQuads) {
        q.clear();
        q.bestSpaceAlgo( myBestSpace);
    }

    /* Pack boxes, round robin fashion, into each quadrant

    This ensures that each quadrant packs
    boxes of approximately the same total

    One way I like to think of this is that it simulates crystal growth from a saturated solution.
    The boxes are molecules floating around in the solution.  Occasionally a box finds the
    perfect spot on the growing crystal where it fits perfectly.

    */
    for (int k = 0; k < myBoxes.size(); k += 4)
    {
        myQuads[0].pack(myBoxes[k]);
        if (k + 1 == myBoxes.size())
            break;
        myQuads[1].pack(myBoxes[k + 1]);
        if (k + 2 == myBoxes.size())
            break;
        myQuads[2].pack(myBoxes[k + 2]);
        if (k + 3 == myBoxes.size())
            break;
        myQuads[3].pack(myBoxes[k + 3]);
    }

    // Rotate the quadrants back into their correct positions

    for (int q = 0; q < 4; q++)
        myQuads[q].rotate(q);

    // maximum location along x or y axis
    // used to scale graphical display
    // calculated in bottom right quadrant so both x and y axis are positive
    // assumes algorithm works well to even out the spread of all quadrants

    mySpread = myQuads[2].maxDim();
}

bool sProblem::test()
{
    sProblem T;
    T.input(
        "5 8\n"
        "32 19\n");
    if (T.myBoxes.size() != 2)
        return false;
    if (T.myBoxes[1].wlh.x != 32)
        return false;

    T.pack();

    auto output = T.output();

    if (output != "-32 -19\n0 -5\n")
        return false;

    return true;
}

main()
{
    sProblem P;
    if (!P.test())
    {
        std::cout << "Unit tests failed\n";
        exit(1);
    }
    P.genRandom(2, 50, 50);
    P.pack();
    cGUI theGUI(P);
    return 0;
}
