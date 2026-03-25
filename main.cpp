#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

// --- Global Constants & Arrays ---
const int MAX_VOTERS = 500; 
string partyNames[6]; 
int voteCounts[6];    
string registeredVoters[MAX_VOTERS]; 
int voterCount = 0;

// Admin Password Logic
// The real password is 'admin123' -> Shifted by 1 it looks like 'benjo234'
string secretPassword = "benjo234"; 

// --- Function to Encrypt/Decrypt (Shift Cipher) ---
string encryptDecrypt(string data) {
    for(int i = 0; i < data.length(); i++) {
        data[i] = data[i] + 1; 
    }
    return data;
}

// --- Function to handle "Rubbish" Input (Input Sanitization) ---
void clearRubbish() {
    cin.clear(); 
    string ignore;
    getline(cin, ignore); 
}

// --- Initialization ---
void initializeSystem() {
    partyNames[1] = "Raj's Permanent Absent Party (RPAP)";
    partyNames[2] = "Rumee Reform Party (RRP)";
    partyNames[3] = "Reeti Voice (RV)";
    partyNames[4] = "Roshan's Bright Ideas Party (RBIP)";
    partyNames[5] = "NOTA (None of the Above)";
    
    for(int i = 1; i <= 5; i++) {
        voteCounts[i] = 0;
    }
    voterCount = 0;
}

// --- Voter Panel ---
void startVoting() {
    string id;
    cout << "\n----------------------------------";
    cout << "\n       VOTER AUTHENTICATION       ";
    cout << "\n----------------------------------";
    cout << "\nEnter your 6-digit Voter ID: ";
    cin >> id;

    // 1. Validate Length (Exactly 6 digits)
    if(id.length() != 6) {
        cout << ">> SECURITY ERROR: ID must be exactly 6 digits!\n";
        return;
    }

    // 2. Validate Type (Numeric only)
    for(char c : id) {
        if(!isdigit(c)) {
            cout << ">> ERROR: Invalid ID format! Numeric digits only.\n";
            return;
        }
    }

    // 3. Duplicate Check (The "Ghost" Memory)
    for(int i = 0; i < voterCount; i++) {
        if(registeredVoters[i] == id) {
            cout << ">> ACCESS DENIED: ID " << id << " has already voted!\n";
            return;
        }
    }

    // Official Ballot
    cout << "\n--- OFFICIAL BALLOT ---\n";
    for(int i = 1; i <= 5; i++) {
        cout << i << ". " << partyNames[i] << endl;
    }

    int choice;
    cout << "\nCast your vote (1-5): ";
    
    // Rubbish Logic for voting choice
    if(!(cin >> choice)) {
        cout << ">> ERROR: Invalid input type! Session Terminated.\n";
        clearRubbish();
        return;
    }

    if(choice >= 1 && choice <= 5) {
        voteCounts[choice]++;
        registeredVoters[voterCount] = id; 
        voterCount++;
        cout << ">> SUCCESS: Vote cast for " << partyNames[choice] << "!\n";
    } else {
        cout << ">> ERROR: Choice out of range (1-5)!\n";
    }
}

// --- Admin Panel ---
void startAdmin() {
    int adminOption;
    while(true) {
        cout << "\n==================================";
        cout << "\n          ADMIN DASHBOARD         ";
        cout << "\n==================================";
        cout << "\n1. View Live Results Summary";
        cout << "\n2. View Voting Graph (Visual)";
        cout << "\n3. Export Audit Trail (.txt)";
        cout << "\n4. Reset System Data";
        cout << "\n5. Logout";
        cout << "\nSelect Option: ";

        if(!(cin >> adminOption)) {
            cout << ">> ERROR: Numeric input required.\n";
            clearRubbish();
            continue;
        }

        if(adminOption == 1) {
            int maxVotes = 0;
            string winner = "Tied/No Votes";
            
            cout << "\n" << left << setw(40) << "CANDIDATE/PARTY" << "VOTES" << endl;
            cout << "-------------------------------------------------------\n";
            for(int i = 1; i <= 5; i++) {
                cout << left << setw(40) << partyNames[i] << voteCounts[i] << endl;
                
                if(i < 5 && voteCounts[i] > maxVotes) {
                    maxVotes = voteCounts[i];
                    winner = partyNames[i];
                }
            }
            cout << "-------------------------------------------------------\n";
            cout << "TOTAL VOTERS RECORDED: " << voterCount << endl;
            cout << "CURRENT LEADER       : " << winner << endl;

        } else if(adminOption == 2) {
            cout << "\n--- STATISTICAL TRENDS (GRAPH) ---\n";
            for(int i = 1; i <= 5; i++) {
                cout << right << setw(35) << partyNames[i] << " | ";
                for(int j = 0; j < voteCounts[i]; j++) {
                    cout << "#"; // Bar chart representation
                }
                cout << " (" << voteCounts[i] << ")" << endl;
            }

        } else if(adminOption == 3) {
            ofstream report("Election_Audit_Trail.txt");
            if(report.is_open()) {
                report << "--- SECURE E-VOTING AUDIT REPORT ---\n\n";
                for(int i = 1; i <= 5; i++) {
                    report << partyNames[i] << " Total: " << voteCounts[i] << " votes\n";
                }
                report << "\nTotal Participation: " << voterCount;
                report << "\nVerified 6-Digit ID Log Count: " << voterCount;
                report.close();
                cout << ">> SUCCESS: Audit Trail 'Election_Audit_Trail.txt' generated.\n";
            } else {
                cout << ">> ERROR: File System Error.\n";
            }

        } else if(adminOption == 4) {
            char confirm;
            cout << "WARNING: Purge all election data? (y/n): ";
            cin >> confirm;
            if(confirm == 'y' || confirm == 'Y') {
                initializeSystem();
                cout << ">> DATA PURGE COMPLETE.\n";
            }

        } else if(adminOption == 5) {
            break; 
        } else {
            cout << ">> ERROR: Option not recognized.\n";
        }
    }
}

// --- Main System Entry ---
int main() {
    initializeSystem();
    int mainChoice;

    while(true) {
        cout << "\n**********************************";
        cout << "\n* SUPREME E-VOTING TERMINAL     *";
        cout << "\n**********************************";
        cout << "\n1. ACCESS VOTER PORTAL";
        cout << "\n2. ACCESS ADMIN PORTAL";
        cout << "\n3. SHUTDOWN";
        cout << "\n----------------------------------";
        cout << "\nCommand Selection: ";

        if(!(cin >> mainChoice)) {
            cout << ">> ERROR: System only accepts numeric commands.\n";
            clearRubbish();
            continue;
        }

        if(mainChoice == 1) {
            startVoting();
        } else if(mainChoice == 2) {
            string inputPass;
            cout << "ENTER SECURITY KEY: ";
            cin >> inputPass;

            // Encrypt user input and compare with stored cipher
            if(encryptDecrypt(inputPass) == secretPassword) {
                startAdmin();
            } else {
                cout << ">> SECURITY BREACH: Access Denied. Key Incorrect.\n";
            }
        } else if(mainChoice == 3) {
            cout << "Exiting System Architecture... Secure Shutdown Complete.\n";
            break;
        } else {
            cout << ">> ERROR: Command " << mainChoice << " is outside system scope.\n";
        }
    }

    return 0;
}
