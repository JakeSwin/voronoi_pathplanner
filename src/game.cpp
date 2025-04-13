#include "game.hpp"

#include <raylib.h>

#include "external/cyVector.hpp"
#include "image_loader.hpp"

Game::Game(const char* imagepath, const char* title) {
    InitWindow(100, 100, title);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    SetImage(imagepath);
}

Game::~Game() {
    CloseWindow();
}

bool Game::Step() {
    Render();
    return WindowShouldClose();
}

void Game::SetImage(const char* filepath) {
    image = ImageLoader::Create(filepath);
    SetWindowSize(image->Width(), image->Height());
    input_points.clear();
    output_points.clear();
}

// void Game::Screenshot(const char* filepath) {
//     TakeScreenshot(filepath);
// }
void Game::Screenshot(const char* filepath) {
    // Create a RenderTexture2D with the same size as the screen
    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    // Start drawing onto the RenderTexture2D
    BeginTextureMode(target);
    ClearBackground(RAYWHITE); // Clear the texture with a background color

    // Redraw everything onto the render texture
    DrawTexture(image->GetTexture(), 0, 0, WHITE); // Draw the image
    DrawCircle(50, 50, 25, RED);                  // Example: Draw a circle

    EndTextureMode(); // Finish rendering to the texture

    // Save the RenderTexture2D to an image file
    Image screenshot = LoadImageFromTexture(target.texture); // Convert texture to image

    // Flip the image vertically to correct OpenGL's coordinate system
    ImageFlipVertical(&screenshot);
    ExportImage(screenshot, filepath);                       // Save image to file
    UnloadImage(screenshot);                                 // Free memory

    // Clean up RenderTexture2D
    UnloadRenderTexture(target);
}

float randomFloat() {
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

void Game::Sample(int input_size, int output_size) {
    input_points.resize(input_size, cy::Vec2f(0.0f, 0.0f));
    output_points.resize(output_size, cy::Vec2f(0.0f, 0.0f));

    for (int i = 0; i < input_size; ++i)
    {
        input_points[i].x = randomFloat();
        input_points[i].y = randomFloat();
    }

    float d_max = d_max_mult * wse.GetMaxPoissonDiskRadius( 2, output_size );

    float ratio = static_cast<float>(output_size) / static_cast<float>(input_size);
    float weight_limit_frac = ( 1 - std::pow( ratio, wse.GetParamGamma() ) ) * std::max(wse.GetParamBeta(), 1.0f);
    float d_min = d_max * weight_limit_frac;

    auto imageWeighting =
        [image_data = image->Data(), image_width = image->Width(),
        image_height = image->Height(), alpha = wse.GetParamAlpha(), d_min,
        weight_factor = weight_factor,
        weight_mult = weight_mult](const cy::Vec2f &p0, const cy::Vec2f &p1,
                                   float dist2, float d_max) -> float {
        int x = (int)round(p0.x * image_width);
        int y = (int)round(p0.y * image_height);

        const unsigned char *pixelOffset0 = image_data + (x + image_width * y) * 3;
        unsigned char r = *(pixelOffset0 + 0);
        unsigned char g = *(pixelOffset0 + 1);
        unsigned char b = *(pixelOffset0 + 2);

        float brightness = (r + g + b) / 3.0f;

        float weight = brightness / 255.0f;

  		float d = cy::Sqrt(dist2);
        d = d * (weight_factor - (weight_mult * weight));

  		return std::pow( 1.0f - d/d_max, alpha );
    };

    wse.Eliminate(
        input_points.data(),
        input_size,
        output_points.data(),
        output_size,
        false,
        d_max,
        2,
        imageWeighting
    );
}

void Game::Render() {
    BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(image->GetTexture(), 0, 0, WHITE);
        if (!output_points.empty())
        {
            for (int i = 0; i < output_points.size(); ++i)
            {
                DrawCircle(
                    (int)round(output_points[i].x * image->Width()),
                    (int)round(output_points[i].y * image->Height()),
                    1.0, RED
                );
            }
        }
    EndDrawing();
}
