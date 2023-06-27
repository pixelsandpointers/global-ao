//
// Created by b on 6/9/23.
//

#include "Gui.hxx"

int main()
{
    map<std::string, std::variant<std::string, bool, int, float>> params;
    params["# kernel samples"] = 64;
    params["# noise samples"] = 16;
    Gui app(1920, 1080, "Render View");
    app.run(params);
    return EXIT_SUCCESS;
}