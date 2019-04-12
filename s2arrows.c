#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

/* - start [   ] -> Do nothing
        - press  's' [s  ] -> Do nothing
            - lift   's' [   ] -> Send 's'
            - press  'f' [sf ] -> Do nothing
                - lift   's' [f  ] -> Do nothing
                - lift   'f' [s  ] -> Do nothing
                - press  'h' [sfh] -> Send 'home'
                    - lift   's' [fh ] -> Do nothing
                    - lift   'f' [sh ] -> Do nothing
                    - lift   'h' [sf ] -> Do nothing
                    - repeat 'h' [sfh] -> Repeat 'home'
            - press  'h' [sh ] -> Send 'left'
                - lift   's' [h  ] -> Do nothing
                - press  'f' [sfh] -> Do nothing
                - lift   'h' [s  ] -> Lift 'left'
                - repeat 'h' [sh ] -> Repeat 'left'
        - press  'f' [f  ] -> Do nothing
            - press  's' [sf ] -> Do nothing
            - lift   'f' [   ] -> Send 'f'
            - press  'h' [fh ] -> Do nothing
                - press  's' [sfh] -> Do noting
                - lift   'f' [h  ] -> Do nothing
                - lift   'h' [f  ] -> Do nothing
                - repeat 'h' [fh ] -> Do nothing
        - press  'h' [h  ] -> Send 'h'
            - press  's' [sh ] -> Do nothing
            - press  'f' [fh ] -> Do nothing
            - lift   'h' [   ] -> Lift 'h'
            - repeat 'h' [h  ] -> Repeat 'h' */

// clang-format off
// Modifiers
const struct input_event
    s_up     = {.type = EV_KEY, .code = KEY_S, .value = 0},
    s_down   = {.type = EV_KEY, .code = KEY_S, .value = 1},
    s_repeat = {.type = EV_KEY, .code = KEY_S, .value = 2},
    f_up     = {.type = EV_KEY, .code = KEY_F, .value = 0},
    f_down   = {.type = EV_KEY, .code = KEY_F, .value = 1},
    f_repeat = {.type = EV_KEY, .code = KEY_F, .value = 2};

// Inputs
const struct input_event
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
    l_repeat = {.type = EV_KEY, .code = KEY_L, .value = 2};

// s-mappings
const struct input_event
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
    right_repeat = {.type = EV_KEY, .code = KEY_RIGHT, .value = 2};

// sf-mappings
const struct input_event
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
    end_repeat      = {.type = EV_KEY, .code = KEY_END,      .value = 2};

// Other
const struct input_event
    syn = {.type = EV_SYN, .code = SYN_REPORT, .value = 0};
// clang-format on

const struct *modifiers[] = {&s_up, &s_down, &s_repeat, &f_up, &f_down,
                             &f_repeat};
const struct *inputs[] = {&h_up, &h_down, &h_repeat, &j_up, &j_down, &j_repeat,
                          &k_up, &k_down, &k_repeat, &l_up, &l_down,
                          &l_repeat};
const struct *s_mappings[] = {&left_up, &left_down, &left_repeat, &down_up,
                              &down_down, &down_repeat, &up_up, &up_down,
                              &up_repeat, &right_up, &right_down,
                              &right_repeat};
const struct *sf_mappings[] = {&home_up, &home_down, &home_repeat,
                               &pagedown_up, &pagedown_down, &pagedown_repeat,
                               &pageup_up, &pageup_down, &pageup_repeat,
                               &end_up, &end_down, &end_repeat};

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

// Globals
struct input_event input;
int s_is_down = 0, f_is_down = 0, h_is_down = 0, j_is_down = 0, k_is_down = 0,
    l_is_down = 0;

// Checks
int is_equal(const struct input_event *event) {
    return input.type == right->type && input.code == right->code &&
           input.value == right->value;
}

int hjkl_is_down() {
    return hjkl_is_down || j_is_down || k_is_down || l_is_down;
}

int hjkl_down() {
    return is_equal(&h_down) || is_equal(&j_down) || is_equal(&k_down) ||
           is_equal(&l_down);
}

int index_of(struct input_event **array) {
    for (int i = 0; i < 12; i++) {
        if (is_equal(array[i], &input)) {
            return i;
        }
    }
    return -1;
}

int main(void) {
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

        // [   ]
        if      (! s_is_down && ! f_is_down && ! hjkl_is_down()) {
            if (is_equal(&s_down)) {
                s_is_down = 1;
            }
            else if (is_equal(&f_down)) {
                f_is_down = 1;
                write_event($input);
            }
            else if (hjkl_down()) {
                if (is_equal(&h_down))      { h_is_down = 1; }
                else if (is_equal(&j_down)) { j_is_down = 1; }
                else if (is_equal(&k_down)) { k_is_down = 1; }
                else if (is_equal(&l_down)) { l_is_down = 1; }
                write_event(&input);
            }
            else {
                write_event(&input);
            }
        }
        // [s  ]
        else if (  s_is_down && ! f_is_down && ! hjkl_is_down()) {
            if (is_equal(&s_up)) {
                s_is_down = 0;
                write_event(&s_down);
                write_event(&syn);
                usleep(10000);
                write_event(&s_up);
            }
            else if (is_equal(&s_repeat)) {
                // Do nothing
            }
            else if (is_equal(&f_down)) {
                f_is_down = 1;
            }
            else if (hjkl_down()) {
                if (is_equal(&h_down))      { h_is_down = 1; }
                else if (is_equal(&j_down)) { j_is_down = 1; }
                else if (is_equal(&k_down)) { k_is_down = 1; }
                else if (is_equal(&l_down)) { l_is_down = 1; }
                write_event(s_mappings[index_of(inputs)]);
            }
            else {
                write_event($input)
            }
        }
        // [ f ]
        else if (! s_is_down &&   f_is_down && ! hjkl_is_down()) {
            if (is_equal(&s_down)) {}
            else if (is_equal(&f_up)) {}
            else if (is_equal(&f_repeat)) {}
            else if (hjkl_down()) {}
        }
        // [  h]
        else if (! s_is_down && ! f_is_down &&   hjkl_is_down()) {
        }
        // [sf ]
        else if (  s_is_down &&   f_is_down && ! hjkl_is_down()) {
        }
        // [s h]
        else if (  s_is_down && ! f_is_down &&   hjkl_is_down()) {
        }
        // [ fh]
        else if (! s_is_down &&   f_is_down &&   hjkl_is_down()) {
        }
        // [sfh]
        else if (  s_is_down &&   f_is_down &&   hjkl_is_down()) {
        }
        else {
            write_event(&input);
        }
    }
}
