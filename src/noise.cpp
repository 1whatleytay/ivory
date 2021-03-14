#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

#include <vector>
#include <numeric>
#include <iostream>

struct Color {
    uint8_t r = 0, g = 0, b = 0;

    bool isBlack() const {
        return r == 0 && g == 0 && b == 0;
    }

    int operator ->() {
        return isBlack();
    }

    Color() = default;
    Color(float x) {
        r = x * 255.0f;
        g = x * 255.0f;
        b = x * 255.0f;
    }
};

int main() {
    constexpr int64_t width = 800, height = 800;

    std::vector<Color> data(width * height);

    size_t j = time(nullptr);

    auto gen = [&data, &j]() {
        for (int64_t x = 0; x < width; x++) {
            for (int64_t y = 0; y < height; y++) {
                float a = x * 0.1, b = y * 0.1;



//                data[x + y * width] =
//                    (stb_perlin_noise3_seed(a, b, 0, 256, 256, 256, j)
//                    + stb_perlin_noise3_seed(a, b, 0, 256, 256, 256, j + 1)) / 2;
//                data[x + y * width] = stb_perlin_turbulence_noise3(a, b, 0, 2.0, 0.5, 6.0);
//                data[x + y * width] = stb_perlin_fbm_noise3(a, b, 0, 2.0, 0.5, 6.0);
                data[x + y * width] = 1 - stb_perlin_ridge_noise3(a, b, 0, 2.0, 0.5, 1.2, 6.0);
            }
        }

        j++;

        int x = std::reduce(data.begin(), data.end(), 0, [](int a, const Color &e) {
            return a + e.isBlack();
        });

        return x < width * height;
    };

//    auto smooth = [&data]() {
//        std::vector<Color> result(width * height);
//
//        for (int64_t x = 0; x < width; x++) {
//            for (int64_t y = 0; y < height; y++) {
//                int64_t total = 0;
//
//                auto get = [&total, &data](int64_t x, int64_t y) {
//                    if (x < 0 || y < 0 || x >= width || y >= height)
//                        return Color(0.0);
//
//                    total++;
//                    return data[x + y * width];
//                };
//
//                Color a = get(x - 1, y), b = get(x + 1, y), c = get(x, y - 1), d = get(x, y + 1);
//
//                result[x + y * width].r = ((int64_t)a.r + (int64_t)b.r + (int64_t)c.r + (int64_t)d.r) / total;
//                result[x + y * width].g = ((int64_t)a.g + (int64_t)b.g + (int64_t)c.g + (int64_t)d.g) / total;
//                result[x + y * width].b = ((int64_t)a.b + (int64_t)b.b + (int64_t)c.b + (int64_t)d.b) / total;
//            }
//        }
//
//        return result;
//    };

    while (!gen()) {
        std::cout << "Tried " << j << std::endl;
    }

//    for (size_t a = 0; a < 50; a++) {
//        data = smooth();
//    }

    if (!stbi_write_png("/Users/desgroup/Desktop/x.jpg", width, height, 3, data.data(), width * sizeof(Color))) {
        assert(false);
    }
}