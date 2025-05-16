#include "Window.h"

Window::Window() {
    set_title("Fourier Cam");
    set_default_size(300, 100);

    vbox.set_margin(20);
    vbox.set_spacing(10);

    button.set_label("Click me");
    button.signal_clicked().connect(sigc::mem_fun(*this, &Window::on_button_clicked));

    label.set_text("Hello World");
    label.set_visible(false);

    vbox.append(button);
    vbox.append(label);

    set_child(vbox);
}

void Window::on_button_clicked() {
    label.set_visible(true);
}
