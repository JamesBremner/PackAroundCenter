#pragma once

#include "packEngine.h"

// class cBox
// {
// public:
//     cxy wh;     // width/height
//     cxy loc;    // location ( top right in quadrant 3 )
//     int userID; // zero-based line number from input string

//     /// @brief CTOR
//     /// @param id for user
//     /// @param ix width
//     /// @param iy height

//     cBox(int id, double ix, double iy);

//     cBox(double ix, double iy);

//     // locate box at point x,y
//     void locate(double x, double y);

//     // locate box on top of another ( no size change )
//     void locate(const cBox &other);

//     // rotate box from 3rd quadrant to specified quad
//     void rotate(int quadrant);

//     cxy topright() const;
//     cxy bottomright() const;
//     cxy bottomleft() const;
// };

/**
 * The 2D space around the central point is divided into 4 quadrants
 *
 * Quadrant indices:
 * 0 : top left
 * 1 : top right
 * 3 : bottom right
 * 4 : bottom left
 *
 * Each quadrant is packed individually.  So that the same code can be used
 * to pack all quadrants each quadrant is rotated into the orientation of the
 * bottom right quadrant, packed, and then rotated back into its final position.
 *
 * The 3rd quadrant was chosen because the distance of a point from the central packing point
 * is simply calculated by summing the x and y co-ordinates.
 */
struct sQuadrant
{
    std::vector<raven::pack::cItem> myBoxes;  // the boxes packed into this quadrant
 
    raven::pack::cEngine E;

    sQuadrant();

    void clear();
    void bestSpaceAlgo(
        raven::pack::cEngine::eBestSpaceAlgo algo );

    /// @brief pack a box into the quadrant
    /// @param box 
    void pack(raven::pack::cItem &box);

    /// @brief maximum location along x or y axis
    /// @return 
    ///
    /// Used to scale the graphical display
    int maxDim() const;

    /// @brief rotate quadrant, and all it's boxes, into final position
    /// @param index 
    void rotate(int index);


private:

    /// @brief find best space to pack box into
    /// @param box 
    /// @return index of space
    //int findBestSpace(const cBox &box);

    /// @brief split space into two smaller spaces remaining after box is packed 
    /// @param space 
    /// @param box 
    //void splitSpace(
        // int space,
        // const cBox &box);

    cxy bottomright( const raven::pack::cItem& rect ) const;
    cxy topright(const raven::pack::cItem &rect) const;
    cxy bottomleft(const raven::pack::cItem &rect) const;
};

struct sProblem
{
    std::vector<raven::pack::cItem> myBoxes;      // the boxes to be packed
    std::vector<sQuadrant> myQuads; // the quadrants around the central point

    static  raven::pack::cEngine::eBestSpaceAlgo myBestSpace;

    double mySpread;                // maximum location along x or y axis

    sProblem();

    static void bestSpace( raven::pack::cEngine::eBestSpaceAlgo ebs)
    {
        myBestSpace = ebs;
    }
    static  raven::pack::cEngine::eBestSpaceAlgo bestSpace()
    {
        return myBestSpace;
    }

    /** @brief Input box sizes from string
    /// @param sin the string to parse
    Each box on its own line: width height
    example:

\pre
5 8
32 19
...
\pre

    */
    void input(const std::string &sin);

    /**
     * @brief Output box packed locations
     *
     * @return std::string  One box per line: id x y
     *
     *  The id is the zero-based line number when the box dimensions
     * were input from string.
     * 
     * Note: the boxes are output in a different order than they were input.
     * The output is quadrant by quadrant.
     */
    std::string output() const;

    /// @brief generate randomly some boxes to be packed
    /// @param min minimum magnitude of any dimension
    /// @param max maximum magnitude of any dimension
    /// @param count number of boxes to generate

    void genRandom(int min, int max, int count);

    /// @brief sort boxes into decreasing volume
    void sort();

    /// @brief pack all the boxes
    void pack();

    /// @brief unit tests
    /// @return true if all passed
    bool test();
};
