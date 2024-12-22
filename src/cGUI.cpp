#include <fstream>
#include <sstream>
#include "cGUI.h"

cGUI::cGUI(sProblem &P)
    : myP(P),
      fm(wex::maker::make())
{
    fm.move({50, 50, 1000, 500});
    fm.text("Packer");

    menus();

    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            draw(S);
        });

    fm.show();
    fm.run();
}

void cGUI::menus()
{
    wex::menubar mb(fm);
    myFileMenu = new wex::menu(fm);
    myFileMenu->append(
        "Load",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fn = fb.open();
            if (fn.empty())
                return;
            std::ifstream t(fn);
            std::stringstream buffer;
            buffer << t.rdbuf();
            myP.input(buffer.str());
            myP.pack();
            fm.update();
        });
    myFileMenu->append(
        "Save",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fn = fb.save();
            if (fn.empty())
                return;
            std::ofstream t(fn);
            t << myP.output();
        });

    mb.append("File", *myFileMenu);

    myAlgoMenu = new wex::menu(fm);
    myAlgoMenu->append(
        "First Fit",
        [&](const std::string &title)
        {
            myP.bestSpace(
                raven::pack::cEngine::eBestSpaceAlgo::firstFit);
            myP.pack();
            fm.text("Packer " + title);
            fm.update();
        });
    myAlgoMenu->append(
        "Min Gaps",
        [&](const std::string &title)
        {
            myP.bestSpace(
                raven::pack::cEngine::eBestSpaceAlgo::minGap);
            myP.pack();
            fm.text("Packer " + title);
            fm.update();
        });
    myAlgoMenu->append(
        "Min Spread",
        [&](const std::string &title)
        {
            myP.bestSpace(
                raven::pack::cEngine::eBestSpaceAlgo::minDist);
            myP.pack();
            fm.text("Packer " + title);
            fm.update();
        });
    mb.append("Algorithm", *myAlgoMenu);
}
void cGUI::draw(wex::shapes &S)
{
    double scale = 300 / (1.2 * myP.mySpread);
    int xoff = 2 * myP.mySpread;
    int yoff = 2 * myP.mySpread;

    for (int q = 0; q < 4; q++)
        for (auto &B : myP.myQuads[q].myBoxes)
            S.rectangle(
                cxy(scale * (B.loc.x + xoff), scale * (B.loc.y + yoff)),
                cxy(scale * B.wlh.x, scale * B.wlh.y));
}
