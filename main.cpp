#include <iostream>
#include <cstring>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
// draw line code attempt
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    // rasterlize
    float k = std::abs(float(y1 - y0) / (x1 - x0));
    float error = 0.f;
    int y = y0;
    
    for (int x = x0; x <= x1; ++x)
    {
        if (steep)
        {
            image.set(y, x, color);
        }
        else
        {
            image.set(x, y, color);
        }

        error += k;
        if (error > 0.5f)
        {
            y += (y0 < y1 ? 1 : -1);
            error -= 1.0f;
        }
    }
}
int main()
{
    Model* model = new Model("F:\\MyGithub\\softRenderer\\obj\\african_head\\african_head.obj");
    TGAColor green;
    green.bgra[0] = 0;
    green.bgra[1] = 255;
    green.bgra[2] = 0;
    green.bgra[3] = 255;
    TGAColor red;
    red.bgra[0] = 0;
    red.bgra[1] = 0;
    red.bgra[2] = 255;
    red.bgra[3] = 255;
    static constexpr int width = 800;
    static constexpr int height = 800;
    TGAImage image(width, height, TGAImage::RGB);

    for (int i = 0; i < model->nfaces(); i++) {
        for (int j = 0; j < 3; j++) {
            vec3 v0 = model->vert(i, j);
            vec3 v1 = model->vert(i, (j + 1) % 3);

            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;

            line(x0, y0, x1, y1, image, red);
        }
    }

    image.write_tga_file("F:\\MyGithub\\build\\softRenderer\\Debug\\output.tga");
    return 0;
}