//
// Created by b on 6/9/23.
//

#include "Gui.hxx"

int main()
{
    Gui app(400, 400, "This is a sample title");
//    AmbientOcclusion ao(400, 400);
    app.run();
//    std::cout << ao.generateSample() << std::endl;
    return 1;
}