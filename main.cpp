#include <iostream>
#include <cstring>
#include <random>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "hashHelper.h"

struct Vec2i
{
    int x, y;
    Vec2i operator-(const Vec2i& other)
    {
        return Vec2i(x - other.x, y - other.y);
    }
    Vec2i operator*(const float scale)
    {
        return Vec2i(scale * x, scale * y);
    }
    Vec2i operator+(const Vec2i& other)
    {
        return Vec2i(x + other.x, y + other.y);
    }
};

// breseham
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
#ifdef LOW_EFFiCIENCY
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
#else
    // diss miss float
    int k = 2 * std::abs(y1 - y0);
    int dx = x1 - x0;
    int error = 0;
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
        if (error > dx)
        {
            y += (y0 < y1 ? 1 : -1);
            error -= 2 * dx;
        }
    }
#endif

}

void line(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor color)
{
    line(t0.x, t0.y, t1.x, t1.y, image, color);
}
// scale line
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color)
{
    if (t0.y == t1.y && t1.y == t2.y)
    {
        return;
    }
    // sort y, bubble sort
    if (t0.y > t1.y)
    {
        std::swap(t0, t1);
    }
    if (t1.y > t2.y)
    {
        std::swap(t1, t2);
    }
    if (t0.y > t1.y)
    {
        std::swap(t1, t0);
    }
    assert(t0.y <= t1.y);
    assert(t1.y <= t2.y);
    // É¨ÃèÏß·¨
    int total_height = t2.y - t0.y;
    for (int y = t0.y; y <= t1.y; ++y)
    {
        int segment_height = t1.y - t0.y + 1;
        float alpha = (float)(y - t0.y) / total_height;
        float beta = (float)(y - t0.y) / segment_height;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t0 + (t1 - t0) * beta;
        if (A.x > B.x)
        {
            std::swap(A.x, B.x);
        }
        for (int i = A.x; i <= B.x; ++i)
        {
            image.set(i, y, color);
        }
    }

    for (int y = t1.y; y <= t2.y; ++y)
    {
        int segment_height = t2.y - t1.y + 1;
        float alpha = (float)(y - t0.y) / total_height;
        float beta = (float)(y - t1.y) / segment_height;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t1 + (t2 - t1) * beta;
        if (A.x > B.x)
        {
            std::swap(A.x, B.x);
        }
        for (int i = A.x; i <= B.x; ++i)
        {
            image.set(i, y, color);
        }
    }
}

int main()
{
    size_t seed = 0;
    int example = 0;

    HashSpace::hash_param(seed, example, example, std::string("example"));
    size_t seed1 = 0;
    HashSpace::hash_param(seed1, example, example, std::string("example"));
    if (seed == seed1)
    {
        std::cout << "hash function successful! \n";
    }

    Log::selfPrint(1, 2, 3, std::string("error"));

    Model* model = new Model("F:\\MyGithub\\softRenderer\\obj\\african_head\\african_head.obj");
    static constexpr int width = 800;
    static constexpr int height = 800;
    TGAImage image(width, height, TGAImage::RGB);
    std::default_random_engine rnd;
    std::uniform_int_distribution uniformEngine(0, 255);
    auto a = uniformEngine(rnd);
    const vec3 lightDir = vec3(0, 0, -1);
    for (int i = 0; i < model->nfaces(); i++) {
        vec3 v0 = model->vert(i, 0);
        vec3 v1 = model->vert(i, 1);
        vec3 v2 = model->vert(i, 2);
        Vec2i t0, t1, t2;
        t0.x = (v0.x + 1.) * width / 2.;
        t0.y = (v0.y + 1.) * height / 2.;
        t1.x = (v1.x + 1.) * width / 2.;
        t1.y = (v1.y + 1.) * height / 2.;
        t2.x = (v2.x + 1.) * width / 2.;
        t2.y = (v2.y + 1.) * height / 2.;
        // calcualte normal && lighting
        auto normal = cross(v1 - v0, v2 - v1);
        auto n = normal.normalized();
        auto theta = n * lightDir;
        theta = std::max(0.0, theta);
        uint8_t intensity = std::max((uint8_t)128, (uint8_t)(theta * 256.0));

        triangle(t0, t1, t2, image, { {intensity, intensity, intensity, 255}, 4 });
    }

    image.write_tga_file("F:\\MyGithub\\build\\softRenderer\\Debug\\output.tga");
    return 0;
}