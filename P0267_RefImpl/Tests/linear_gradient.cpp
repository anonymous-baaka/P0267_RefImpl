#include "catch.hpp"
#include <io2d.h>
#include "comparison.h"

using namespace std;
using namespace std::experimental;
using namespace std::experimental::io2d;

TEST_CASE("IO2D properly draws a linear gradient")
{
    auto reference = "linear_gradient_aquamarine_dark_magenta_lime_300x200.png";
    auto image = image_surface{format::argb32, 300, 200};

    SECTION("Draw with abs coordinates") {
        auto b = brush{ {0.f, 100.f},
                        {300.f, 100.f},
                        {gradient_stop{0.0f, rgba_color::aquamarine},
                         gradient_stop{0.5f, rgba_color::dark_magenta},
                         gradient_stop{1.0f, rgba_color::lime}}};
        auto pb = path_builder{};
        pb.new_figure({0.f, 0.f});
        pb.rel_line({300.f, 0.f});
        pb.rel_line({0.f, 200.f});
        pb.rel_line({-300.f, 0.f});
        pb.rel_line({0.f, -200.f});
        pb.close_figure();
        image.fill(b, pb);
    }
    SECTION("Draw with a brush transformation - translate") {
        auto b = brush{ {100.f, 250.f},
                        {400.f, 250.f},
                        {gradient_stop{0.0f, rgba_color::aquamarine},
                         gradient_stop{0.5f, rgba_color::dark_magenta},
                         gradient_stop{1.0f, rgba_color::lime}}};
        auto bp = brush_props{wrap_mode::none, filter::good, fill_rule::winding, matrix_2d::init_translate({100.f, 150.f})};
        auto pb = path_builder{};
        pb.new_figure({0.f, 0.f});
        pb.rel_line({300.f, 0.f});
        pb.rel_line({0.f, 200.f});
        pb.rel_line({-300.f, 0.f});
        pb.rel_line({0.f, -200.f});
        pb.close_figure();
        image.fill(b, pb, bp);
    }
    SECTION("Draw with a brush transformation - horizontal mirror") {
        auto b = brush{ {300.f, 100.f},
                        {0.f, 100.f},
                        {gradient_stop{0.0f, rgba_color::aquamarine},
                         gradient_stop{0.5f, rgba_color::dark_magenta},
                         gradient_stop{1.0f, rgba_color::lime}}};
        auto m = matrix_2d::init_translate({-150.f, 0.}) * matrix_2d::init_scale({-1.f, 1.f}) * matrix_2d::init_translate({150.f, 0.});
        auto bp = brush_props{wrap_mode::none, filter::good, fill_rule::winding, m};
        auto pb = path_builder{};
        pb.new_figure({0.f, 0.f});
        pb.rel_line({300.f, 0.f});
        pb.rel_line({0.f, 200.f});
        pb.rel_line({-300.f, 0.f});
        pb.rel_line({0.f, -200.f});
        pb.close_figure();
        image.fill(b, pb, bp);
    }

    CHECK( ComparePNGWithTolerance(image, reference, 0.01f) == true );
}

TEST_CASE("IO2D properly handles wrapping modes for linear gradients")
{
    auto reference = "linear_gradient_wrapping_modes_300x200.png";
    auto image = image_surface{format::argb32, 300, 200};
    auto sector = [](bounding_box bb){
        auto pb = path_builder{};
        pb.new_figure({bb.x(), bb.y()});
        pb.rel_line({bb.width(), 0.f});
        pb.rel_line({0.f, bb.height()});
        pb.rel_line({-bb.width(), 0.f});
        pb.rel_line({0.f, -bb.height()});
        pb.close_figure();
        return pb;
    };
    
    auto b = brush{ {125.f, 100.f},
                    {175.f, 100.f},
                    {gradient_stop{0.0f, rgba_color::aquamarine},
                     gradient_stop{0.2f, rgba_color::dark_magenta},
                     gradient_stop{0.6f, rgba_color::gainsboro},
                     gradient_stop{1.0f, rgba_color::lime}}};
    
    image.fill(b, sector({0.f, 0.f,   300.f, 50.f}), brush_props{wrap_mode::none});
    image.fill(b, sector({0.f, 50.f,  300.f, 50.f}), brush_props{wrap_mode::repeat});
    image.fill(b, sector({0.f, 100.f, 300.f, 50.f}), brush_props{wrap_mode::reflect});
    image.fill(b, sector({0.f, 150.f, 300.f, 50.f}), brush_props{wrap_mode::pad});

    // there's a significat difference in color interpolation between Cairo backend and CoreGraphics backend.
    // need to invesitage it.
    CHECK( ComparePNGWithTolerance(image, reference, 0.08f) == true );
}
