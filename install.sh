#!/bin/bash

set -e

install_fedora() {
    echo "Detected Fedora. Installing GTKmm 4 and Meson..."
    sudo dnf update -y
    sudo dnf install -y gtkmm4.0-devel meson ninja-build gcc-c++ make
    sudo dnf install -y libepoxy libepoxy-devel
}

install_apt_based() {
    echo "Detected Debian/Ubuntu-based system. Installing GTKmm 4 and Meson..."
    sudo apt update
    sudo apt install -y libgtkmm-4.0-dev meson ninja-build build-essential
    sudo apt install -y libepoxy-dev
}

main() {
    if [ -f /etc/fedora-release ]; then
        install_fedora
    elif [ -f /etc/debian_version ]; then
        install_apt_based
    else
        echo "Unsupported Linux distribution."
        exit 1
    fi

    echo "GTKmm 4 and Meson installed successfully."
}

main
