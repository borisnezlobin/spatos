#include <orbital.h>
#include <stdint.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 480

int main() {
    void *window = orb_window_new_flags(10, 10, 640, 480, "Test window", ORB_WINDOW_ASYNC | ORB_WINDOW_RESIZABLE);

    orb_window_set_pos(window, 100, 100);
    orb_window_set_size(window, WIDTH, HEIGHT);
    orb_window_sync(window);

    printf("Display size: (%d, %d)\n", orb_display_width(), orb_display_height());
    printf("Window size: (%d, %d)\n", orb_window_width(window), orb_window_height(window));
    printf("Window position: (%d, %d)\n", orb_window_x(window), orb_window_y(window));

    char title[1024] = { 0 };
    int frame_count = 0;
    bool quit = false;

    while (!quit) {
        sprintf(title, "Frame #%d", frame_count);
        orb_window_set_title(window, title);

        uint32_t *frame_data = orb_window_data(window);
        uint32_t width = orb_window_width(window);
        uint32_t height = orb_window_height(window);

        for (uint32_t y = 0; y < height; ++y) {
            for (uint32_t x = 0; x < width; ++x) {
                frame_data[y * width + x] =
                    0xFF000000 |
                    (((x ^ y ^ frame_count) & 0xFF) << 16) |
                    ((y & 0xFF) << 8) |
                    (x & 0xFF);
            }
        }

        orb_window_sync(window);
        ++frame_count;

        void *event_iter = orb_window_events(window);
        OrbEventOption oeo = orb_events_next(event_iter);

        while (oeo.tag != OrbEventOption_None) {
            switch (oeo.tag) {
                case OrbEventOption_Key:
                    printf("Key { character: %c, scancode: %d, pressed: %d }\n",
                        oeo.key.character,
                        oeo.key.scancode,
                        oeo.key.pressed);

                    if (oeo.key.scancode == ORB_KEY_ESC) {
                        quit = true;
                    }

                    break;
                case OrbEventOption_Mouse:
                    printf("Mouse { x: %d, y: %d }\n",
                        oeo.mouse.x,
                        oeo.mouse.y);
                    break;
                case OrbEventOption_Button:
                    printf("Button { left: %d, middle: %d, right: %d }\n",
                        oeo.button.left,
                        oeo.button.middle,
                        oeo.button.right);
                    break;
                case OrbEventOption_Scroll:
                    printf("Scroll { x: %d, y: %d }\n",
                        oeo.scroll.x,
                        oeo.scroll.y);
                    break;
                case OrbEventOption_Quit:
                    printf("Quit { }\n");
                    quit = true;
                    break;
                case OrbEventOption_Focus:
                    printf("Focus { focused: %d }\n",
                        oeo.focus.focused);
                    break;
                case OrbEventOption_Move:
                    printf("Move { x: %d, y: %d }\n",
                        oeo.move.x,
                        oeo.move.y);
                    break;
                case OrbEventOption_Resize:
                    printf("Resize { width: %d, height: %d }\n",
                        oeo.resize.width,
                        oeo.resize.height);
                    break;
                case OrbEventOption_Screen:
                    printf("Screen { width: %d, height: %d }\n",
                        oeo.screen.width,
                        oeo.screen.height);
                    break;
                case OrbEventOption_Unknown:
                    printf("Unknown { code: %ld, a: %ld, b: %ld }\n",
                        oeo.unknown.code,
                        oeo.unknown.a,
                        oeo.unknown.b);
                    break;
                default:
                    break;
            }

            oeo = orb_events_next(event_iter);
        }

        orb_events_destroy(event_iter);
    }

    orb_window_destroy(window);
    return 0;
}
