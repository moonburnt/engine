#include <raylib.h>

#include <string>

class App;
class Button;
class Checkbox;

// Constructors of commonly used buttons.
// Two things to keep in mind:
// - These require shared::assets to be initialized and have default textures
// and sounds loaded in.
// - These use "new" under the hood, thus require manual deletion.
class GuiBuilder {
public:
    GuiBuilder(App* app);

    Button* make_close_button();
    Button* make_text_button(std::string txt);
    Checkbox* make_checkbox(bool default_state);

private:
    App* app;
};

// Returns random Vector2 with values between 0 and provided
Vector2 get_rand_vec2(int x, int y);
Vector2 get_rand_vec2(Vector2 size);

// Returns random non-white color
Color get_rand_color();

// Get current window's size, regardless if its fullscreen or not
int get_window_width();
int get_window_height();
