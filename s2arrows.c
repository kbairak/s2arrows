#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

// clang-format off
const struct input_event
    s_up     = {.type = EV_KEY, .code = KEY_S, .value = 0},
    s_down   = {.type = EV_KEY, .code = KEY_S, .value = 1},
    s_repeat = {.type = EV_KEY, .code = KEY_S, .value = 2},
    f_up     = {.type = EV_KEY, .code = KEY_F, .value = 0},
    f_down   = {.type = EV_KEY, .code = KEY_F, .value = 1},
    f_repeat = {.type = EV_KEY, .code = KEY_F, .value = 2},

    h_up     = {.type = EV_KEY, .code = KEY_H, .value = 0},
    h_down   = {.type = EV_KEY, .code = KEY_H, .value = 1},
    h_repeat = {.type = EV_KEY, .code = KEY_H, .value = 2},
    j_up     = {.type = EV_KEY, .code = KEY_J, .value = 0},
    j_down   = {.type = EV_KEY, .code = KEY_J, .value = 1},
    j_repeat = {.type = EV_KEY, .code = KEY_J, .value = 2},
    k_up     = {.type = EV_KEY, .code = KEY_K, .value = 0},
    k_down   = {.type = EV_KEY, .code = KEY_K, .value = 1},
    k_repeat = {.type = EV_KEY, .code = KEY_K, .value = 2},
    l_up     = {.type = EV_KEY, .code = KEY_L, .value = 0},
    l_down   = {.type = EV_KEY, .code = KEY_L, .value = 1},
    l_repeat = {.type = EV_KEY, .code = KEY_L, .value = 2},

    left_up      = {.type = EV_KEY, .code = KEY_LEFT,  .value = 0},
    left_down    = {.type = EV_KEY, .code = KEY_LEFT,  .value = 1},
    left_repeat  = {.type = EV_KEY, .code = KEY_LEFT,  .value = 2},
    down_up      = {.type = EV_KEY, .code = KEY_DOWN,  .value = 0},
    down_down    = {.type = EV_KEY, .code = KEY_DOWN,  .value = 1},
    down_repeat  = {.type = EV_KEY, .code = KEY_DOWN,  .value = 2},
    up_up        = {.type = EV_KEY, .code = KEY_UP,    .value = 0},
    up_down      = {.type = EV_KEY, .code = KEY_UP,    .value = 1},
    up_repeat    = {.type = EV_KEY, .code = KEY_UP,    .value = 2},
    right_up     = {.type = EV_KEY, .code = KEY_RIGHT, .value = 0},
    right_down   = {.type = EV_KEY, .code = KEY_RIGHT, .value = 1},
    right_repeat = {.type = EV_KEY, .code = KEY_RIGHT, .value = 2},

    home_up         = {.type = EV_KEY, .code = KEY_HOME,     .value = 0},
    home_down       = {.type = EV_KEY, .code = KEY_HOME,     .value = 1},
    home_repeat     = {.type = EV_KEY, .code = KEY_HOME,     .value = 2},
    pagedown_up     = {.type = EV_KEY, .code = KEY_PAGEDOWN, .value = 0},
    pagedown_down   = {.type = EV_KEY, .code = KEY_PAGEDOWN, .value = 1},
    pagedown_repeat = {.type = EV_KEY, .code = KEY_PAGEDOWN, .value = 2},
    pageup_up       = {.type = EV_KEY, .code = KEY_PAGEUP,   .value = 0},
    pageup_down     = {.type = EV_KEY, .code = KEY_PAGEUP,   .value = 1},
    pageup_repeat   = {.type = EV_KEY, .code = KEY_PAGEUP,   .value = 2},
    end_up          = {.type = EV_KEY, .code = KEY_END,      .value = 0},
    end_down        = {.type = EV_KEY, .code = KEY_END,      .value = 1},
    end_repeat      = {.type = EV_KEY, .code = KEY_END,      .value = 2},

    syn = {.type = EV_SYN, .code = SYN_REPORT, .value = 0};
// clang-format on

int equal(const struct input_event *first, const struct input_event *second) {
    return first->type == second->type && first->code == second->code &&
           first->value == second->value;
}

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

int main(void) {
    int s_is_down = 0, f_is_down = 0, s_give_up = 0, f_give_up = 0;
    struct input_event input;
    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        // Fluff
        if (input.type == EV_MSC && input.code == MSC_SCAN) {
            continue;
        }
        if (input.type != EV_KEY) {
            write_event(&input);
            continue;
        }

        if (equal(&input, &s_down)) {
            // 's' was pressed, mark and supress
            s_is_down = 1;
            continue;
        }

        if (f_is_down && ! s_is_down) {
            if (equal(&input, &f_up)) {
                f_is_down = 0;
                f_give_up = 0;
                continue;
            }

        } else if (s_is_down) {
            // Something else happened while 's' was down
            if (equal(&input, &s_repeat)) {
                // More 's', suppress
                continue;
            }

            if (equal(&input, &s_up)) {
                // 's' was released
                if (! s_give_up) {
                    // Nothing else happened in the meantime, process 's'
                    // normally
                    write_event(&s_down);
                    write_event(&syn);
                    usleep(20000);
                    write_event(&s_up);
                }
                s_is_down = 0;
                s_give_up = 0;
                continue;
            }

            // Something other than 's' happened
            s_give_up = 1;

            if (equal(&input, &f_down)) {
                // 'f' was pressed, mark and supress
                f_is_down = 1;
                continue;
            }

            if (f_is_down) {
                if (equal(&input, &f_up)) {
                    if (! f_give_up) {
                        // Nothing else happened in the meantime, process 'f'
                        // normally
                        write_event(&f_down);
                        write_event(&syn);
                        usleep(20000);
                        write_event(&f_up);
                    }
                    f_is_down = 0;
                    f_give_up = 0;
                }

                f_give_up = 1;

                const struct input_event *mappings[12][2] = {
                    {&h_up,     &home_up},
                    {&h_down,   &home_down},
                    {&h_repeat, &home_repeat},
                    {&j_up,     &pagedown_up},
                    {&j_down,   &pagedown_down},
                    {&j_repeat, &pagedown_repeat},
                    {&k_up,     &pageup_up},
                    {&k_down,   &pageup_down},
                    {&k_repeat, &pageup_repeat},
                    {&l_up,     &end_up},
                    {&l_down,   &end_down},
                    {&l_repeat, &end_repeat},
                };
                int broke = 0;
                for (int i = 0; i < 12; i++) {
                    const struct input_event *from = mappings[i][0],
                                             *to   = mappings[i][1];
                    if (equal(&input, from)) {
                        write_event(to);
                        write_event(&syn);
                        usleep(20000);
                        broke = 1;
                        break;
                    }
                }
                if (broke) {
                    continue;
                }

            } else {
                const struct input_event *mappings[12][2] = {
                    {&h_up,     &left_up},
                    {&h_down,   &left_down},
                    {&h_repeat, &left_repeat},
                    {&j_up,     &down_up},
                    {&j_down,   &down_down},
                    {&j_repeat, &down_repeat},
                    {&k_up,     &up_up},
                    {&k_down,   &up_down},
                    {&k_repeat, &up_repeat},
                    {&l_up,     &right_up},
                    {&l_down,   &right_down},
                    {&l_repeat, &right_repeat},
                };

                int broke = 0;
                for (int i = 0; i < 12; i++) {
                    const struct input_event *from = mappings[i][0],
                                             *to   = mappings[i][1];
                    if (equal(&input, from)) {
                        write_event(to);
                        write_event(&syn);
                        usleep(20000);
                        broke = 1;
                        break;
                    }
                }
                if (broke) {
                    continue;
                }
            }
        }

        write_event(&input);
    }
}
