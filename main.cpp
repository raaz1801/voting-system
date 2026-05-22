#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <windows.h>

using namespace std;

// --- Global Constants & Arrays ---
const int MAX_VOTERS = 500;
string partyNames[6];

// Bundle all persistent data into a clean structure for binary writing/reading
struct ElectionData {
    int voterCount;
    int voteCounts[6];
    char registeredVoters[MAX_VOTERS][7]; // 6 digits + null terminator
};

// Global instance of system state
ElectionData sysData;

const string DAT_FILE = "Election_Audit_Trail.dat";
const string TXT_FILE = "Election_Audit_Trail.txt";

// Admin Password
string secretPassword = "benjo234";

// --- Shift Cipher Encrypt ---
string encryptDecrypt(string data) {
    for (size_t i = 0; i < data.length(); i++) {
        data[i] = data[i] + 1;
    }
    return data;
}

// --- Clear bad input from cin ---
void clearRubbish() {
    cin.clear();
    string ignore;
    getline(cin, ignore);
}

// --- Save binary backup (.dat) ---
void saveBinaryData() {
    ofstream file(DAT_FILE, ios::out | ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&sysData), sizeof(ElectionData));
        file.close();
    }
}

// --- Export readable audit trail as locked read-only .txt ---
void exportAuditTrail() {
    // Remove read-only attribute first (in case file already exists as locked)
    SetFileAttributesA(TXT_FILE.c_str(), FILE_ATTRIBUTE_NORMAL);

    ofstream txt(TXT_FILE);
    if (!txt.is_open()) {
        cout << ">> FILE ERROR: Could not write audit trail.\n";
        return;
    }

    // Find winner (excluding NOTA at index 5)
    int maxVotes = 0;
    string winner = "No Votes Cast";
    for (int i = 1; i <= 4; i++) {
        if (sysData.voteCounts[i] > maxVotes) {
            maxVotes = sysData.voteCounts[i];
            winner = partyNames[i];
        }
    }

    txt << "==================================================\n";
    txt << "         OFFICIAL ELECTION AUDIT TRAIL            \n";
    txt << "==================================================\n\n";
    txt << left << setw(42) << "PARTY / CANDIDATE" << "VOTES\n";
    txt << "--------------------------------------------------\n";
    for (int i = 1; i <= 5; i++) {
        txt << left << setw(42) << partyNames[i] << sysData.voteCounts[i] << "\n";
    }
    txt << "--------------------------------------------------\n";
    txt << "TOTAL VOTERS RECORDED : " << sysData.voterCount << "\n\n";
    txt << "CURRENT LEADING PARTY : " << winner << "\n\n";
    txt << "==================================================\n";
    txt << "  This file is READ-ONLY and cannot be edited.    \n";
    txt << "==================================================\n";

    txt.close();

    // Lock the file as read-only at OS level
    SetFileAttributesA(TXT_FILE.c_str(), FILE_ATTRIBUTE_READONLY);

    cout << ">> SUCCESS: Audit trail exported to '" << TXT_FILE << "'\n";
    cout << ">> File is now READ-ONLY and protected from editing.\n";
    cout << ">> Binary backup also saved to '" << DAT_FILE << "'\n";
}

// --- Initialize System & Auto-Load ---
void initializeSystem() {
    partyNames[1] = "Raj's Permanent Absent Party (RPAP)";
    partyNames[2] = "Rumee Reform Party (RRP)";
    partyNames[3] = "Reeti Voice (RV)";
    partyNames[4] = "Roshan's Bright Ideas Party (RBIP)";
    partyNames[5] = "NOTA (None of the Above)";

    // Try to load existing binary backup
    ifstream incoming(DAT_FILE, ios::in | ios::binary);
    if (incoming.is_open()) {
        incoming.read(reinterpret_cast<char*>(&sysData), sizeof(ElectionData));
        incoming.close();
    } else {
        // Defaults if no file exists
        sysData.voterCount = 0;
        for (int i = 1; i <= 5; i++) {
            sysData.voteCounts[i] = 0;
        }
        memset(sysData.registeredVoters, 0, sizeof(sysData.registeredVoters));
    }
}

// --- Voter Panel ---
void startVoting() {
    system("cls");
    string id;
    cout << "\n----------------------------------";
    cout << "\n        VOTER AUTHENTICATION       ";
    cout << "\n----------------------------------";
    cout << "\nEnter your 6-digit Voter ID: ";
    cin >> id;

    if (id.length() != 6) {
        cout << ">> SECURITY ERROR: ID must be exactly 6 digits!\n";
        cout << "\nPress Enter to return to main menu...";
        clearRubbish();
        cin.get();
        return;
    }

    for (char c : id) {
        if (!isdigit(c)) {
            cout << ">> ERROR: Invalid ID format! Numeric digits only.\n";
            cout << "\nPress Enter to return to main menu...";
            clearRubbish();
            cin.get();
            return;
        }
    }

    // Duplicate check
    for (int i = 0; i < sysData.voterCount; i++) {
        if (string(sysData.registeredVoters[i]) == id) {
            cout << ">> ACCESS DENIED: ID " << id << " has already voted!\n";
            cout << "\nPress Enter to return to main menu...";
            clearRubbish();
            cin.get();
            return;
        }
    }

    cout << "\n--- OFFICIAL BALLOT ---\n";
    for (int i = 1; i <= 5; i++) {
        cout << i << ". " << partyNames[i] << endl;
    }

    int choice;
    cout << "\nCast your vote (1-5): ";

    if (!(cin >> choice)) {
        cout << ">> ERROR: Invalid input type! Session Terminated.\n";
        clearRubbish();
        cout << "\nPress Enter to return to main menu...";
        cin.get();
        return;
    }

    if (choice >= 1 && choice <= 5) {
        sysData.voteCounts[choice]++;
        strcpy(sysData.registeredVoters[sysData.voterCount], id.c_str());
        sysData.voterCount++;
        saveBinaryData();
        cout << ">> SUCCESS: Vote cast for " << partyNames[choice] << "!\n";
    } else {
        cout << ">> ERROR: Choice out of range (1-5)!\n";
    }

    cout << "\nPress Enter to return to main menu...";
    clearRubbish();
    cin.get();
}

// --- Admin Panel ---
void startAdmin() {
    int adminOption;
    while (true) {
        system("cls");
        cout << "\n==================================";
        cout << "\n          ADMIN DASHBOARD          ";
        cout << "\n==================================";
        cout << "\n1. View Live Results Summary";
        cout << "\n2. View Voting Graph (Visual)";
        cout << "\n3. Export Read-Only Audit Trail";
        cout << "\n4. Reset System Data";
        cout << "\n5. Logout";
        cout << "\nSelect Option: ";

        if (!(cin >> adminOption)) {
            cout << ">> ERROR: Numeric input required.\n";
            clearRubbish();
            cout << "\nPress Enter to continue...";
            cin.get();
            continue;
        }

        if (adminOption == 1) {
            int maxVotes = 0;
            string winner = "Tied/No Votes";

            cout << "\n" << left << setw(40) << "CANDIDATE/PARTY" << "VOTES" << endl;
            cout << "-------------------------------------------------------\n";
            for (int i = 1; i <= 5; i++) {
                cout << left << setw(40) << partyNames[i] << sysData.voteCounts[i] << endl;
                if (i < 5 && sysData.voteCounts[i] > maxVotes) {
                    maxVotes = sysData.voteCounts[i];
                    winner = partyNames[i];
                }
            }
            cout << "-------------------------------------------------------\n";
            cout << "TOTAL VOTERS RECORDED: " << sysData.voterCount << endl;
            cout << "CURRENT LEADER       : " << winner << endl;

            cout << "\nPress Enter to go back to dashboard...";
            clearRubbish();
            cin.get();

        } else if (adminOption == 2) {
            cout << "\n--- STATISTICAL TRENDS (GRAPH) ---\n";
            for (int i = 1; i <= 5; i++) {
                cout << right << setw(35) << partyNames[i] << " | ";
                for (int j = 0; j < sysData.voteCounts[i]; j++) {
                    cout << "#";
                }
                cout << " (" << sysData.voteCounts[i] << ")" << endl;
            }

            cout << "\nPress Enter to go back to dashboard...";
            clearRubbish();
            cin.get();

        } else if (adminOption == 3) {
            // Save binary backup first, then export locked txt
            saveBinaryData();
            exportAuditTrail();

            cout << "\nPress Enter to go back to dashboard...";
            clearRubbish();
            cin.get();

        } else if (adminOption == 4) {
            char confirm;
            cout << "WARNING: Purge all election data? (y/n): ";
            cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                sysData.voterCount = 0;
                for (int i = 1; i <= 5; i++) {
                    sysData.voteCounts[i] = 0;
                }
                memset(sysData.registeredVoters, 0, sizeof(sysData.registeredVoters));

                // Unlock txt before overwriting, then re-lock after
                SetFileAttributesA(TXT_FILE.c_str(), FILE_ATTRIBUTE_NORMAL);
                saveBinaryData();
                exportAuditTrail(); // Re-exports purged (zeroed) data and re-locks

                cout << ">> DATA PURGE COMPLETE.\n";
            }

            cout << "\nPress Enter to go back to dashboard...";
            clearRubbish();
            cin.get();

        } else if (adminOption == 5) {
            break;
        } else {
            cout << ">> ERROR: Option not recognized.\n";
            cout << "\nPress Enter to continue...";
            clearRubbish();
            cin.get();
        }
    }
}

// --- Main Entry Point ---
int main() {
    initializeSystem();
    int mainChoice;

    while (true) {
        system("cls");
        cout << "\n**********************************";
        cout << "\n*   SUPREME E-VOTING TERMINAL   *";
        cout << "\n**********************************";
        cout << "\n1. ACCESS VOTER PORTAL";
        cout << "\n2. ACCESS ADMIN PORTAL";
        cout << "\n3. SHUTDOWN";
        cout << "\n----------------------------------";
        cout << "\nCommand Selection: ";

        if (!(cin >> mainChoice)) {
            cout << ">> ERROR: System only accepts numeric commands.\n";
            clearRubbish();
            cout << "\nPress Enter to try again...";
            cin.get();
            continue;
        }

        if (mainChoice == 1) {
            startVoting();

        } else if (mainChoice == 2) {
            string inputPass;
            cout << "ENTER SECURITY KEY: ";
            cin >> inputPass;

            if (encryptDecrypt(inputPass) == secretPassword) {
                startAdmin();
            } else {
                cout << ">> SECURITY BREACH: Access Denied. Key Incorrect.\n";
                cout << "\nPress Enter to try again...";
                clearRubbish();
                cin.get();
            }

        } else if (mainChoice == 3) {
            system("cls");
            cout << "Exiting System Architecture... Secure Shutdown Complete.\n";
            break;

        } else {
            cout << ">> ERROR: Command " << mainChoice << " is outside system scope.\n";
            cout << "\nPress Enter to continue...";
            clearRubbish();
            cin.get();
        }
    }

    return 0;
}
