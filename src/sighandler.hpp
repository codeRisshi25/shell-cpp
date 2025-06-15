#pragma once

#include <csignal>
#include <iostream>

/// Press CTRL + Z to invoke this signal handler.
/// @param signum Signal Number: 20
void handle_sigtstp(int signum) {
    std::cout << "\n^Z\nSIGTSTP INVOKED...\n";
    exit(signum);
}

/// Press CTRL + C to invoke this signal handler.
/// @param signum Signal Number: 2
void handle_sigint(int signum) {
    std::cout << "\n^C\n" << "SIGINT INVOKED..." << "\n";;
    exit(signum);
}

/// Termination Signal
/// @param signum Signal Number: 15
void handle_sigterm(int signum) {
    std::cout << "\nSIGTERM INVOKED...\n";
    exit(signum);
}