// #include <vector>
#include <string>
#include <utility>

// Idea is the following:
// InputController is an abstract template that does some logic in its update()
// cycle with its puppets.
// Its puppet's responsibility to friend concrete controller realization, if
// necessary.
// Its also puppet's responsibility to ensure it won't leave dead pointer on
// destruction.
// TODO: consider switching from single-item controller to vector of puppets,
// to be able to control many items at once.
// template <typename T> class InputController {
//     protected:
//         T puppet;

//         bool enabled = true;

//     public:
//         InputController(T _puppet)
//             : puppet(_puppet) {};

//         virtual update(float) {};

//         // Statuses to control controller's availability
//         // Unsure if these should be there, but may come handy as its kind of
//         // common logic
//         void enable() {
//             enabled = true;
//         }

//         void disable() {
//             enabled = false;
//         }

//         bool is_enabled() {
//             return enabled;
//         }
// }

template <typename I> <typename O>
class AbstractDataProcessor {
    protected:
        std::function<I(void)> input_cb,
        std::function<void(O)> output_cb,

    public:
        DataProcessor(
            std::function<I(void)> _input_cb,
            std::function<void(O)> _output_cb)
            : input_cb(_input_cb)
            , output_cb(_output_cb) {}

        virtual O handler(I data) = 0;

        void update() {
            output_cb(handler(input_cb()));
        }
};

class TextInputProcessor : public AbstractDataProcessor <const std::string& data, std::pair<std::string, bool>> {
    public:
        std::pair<std::string, bool> handler(const std::string& text) override {
            // if (!is_enabled) {
            //     return;
            // }

            int key = GetCharPressed();

            bool text_changed = false;

            // Support for multiple characters pressed on the same frame
            while (key > 0) {
                // Only allow keys in between 32 to 125
                if ((key >= 32) && (key <= 125)) {
                    if (max_size != 0 && text.length() >= max_size) {
                        break;
                    }

                    text += static_cast<char>(key);
                    text_changed = true;
                }

                // Switching key to next char in queue (if there is such char, else 0)
                key = GetCharPressed();
            }

            // TODO: add support for backspace being held, as right now this require
            // it to be pressed manually for each symbol
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (text.length() > 0) {
                    text.pop_back();
                    text_changed = true;
                }
            }

            return std::pair(text, text_changed);
        };
}
