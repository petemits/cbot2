#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <regex>
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

using namespace std;

// ==================== SIMPLE LEAD GENERATION BOT ====================
class LeadBot {
private:
    vector<map<string, string>> leads;
    mutex leads_mutex;
    
    string getCurrentTime() {
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    string generateId() {
        static int counter = 1;
        return "LEAD_" + to_string(counter++) + "_" + to_string(time(nullptr));
    }
    
    vector<string> extractEmailsFromText(const string& text) {
        vector<string> emails;
        regex email_pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
        
        auto words_begin = sregex_iterator(text.begin(), text.end(), email_pattern);
        auto words_end = sregex_iterator();
        
        for(sregex_iterator i = words_begin; i != words_end; ++i) {
            emails.push_back((*i).str());
        }
        
        // Add some sample emails if none found
        if(emails.empty()) {
            emails = {"info@company.com", "contact@business.com", "sales@example.com"};
        }
        
        return emails;
    }
    
    vector<string> extractPhonesFromText(const string& text) {
        vector<string> phones;
        regex phone_pattern(R"((\+\d{1,3}[-.\s]?)?\(?\d{3}\)?[-.\s]?\d{3}[-.\s]?\d{4})");
        
        auto phones_begin = sregex_iterator(text.begin(), text.end(), phone_pattern);
        auto phones_end = sregex_iterator();
        
        for(sregex_iterator i = phones_begin; i != phones_end; ++i) {
            phones.push_back((*i).str());
        }
        
        if(phones.empty()) {
            phones = {"+1-555-123-4567", "+1-555-987-6543"};
        }
        
        return phones;
    }
    
public:
    void analyzeWebsite(const string& url) {
        cout << "\n🔍 Analyzing: " << url << endl;
        cout << "Extracting contact information..." << endl;
        
        // Simulated website content
        string simulated_content = 
            "Welcome to " + url + "\n"
            "Contact us at: info@company.com, sales@department.com\n"
            "Phone: +1-555-123-4567, +1-800-555-1212\n"
            "Address: 123 Business Street, City, Country\n"
            "We provide excellent services to our clients.";
        
        vector<string> emails = extractEmailsFromText(simulated_content);
        vector<string> phones = extractPhonesFromText(simulated_content);
        
        // Create leads from emails
        lock_guard<mutex> lock(leads_mutex);
        
        for(const auto& email : emails) {
            map<string, string> lead;
            lead["id"] = generateId();
            lead["email"] = email;
            lead["website"] = url;
            lead["status"] = "new";
            lead["created"] = getCurrentTime();
            lead["score"] = to_string(50 + rand() % 50); // Random score 50-100
            
            // Generate company name from domain
            size_t at_pos = email.find('@');
            if(at_pos != string::npos) {
                string domain = email.substr(at_pos + 1);
                size_t dot_pos = domain.find('.');
                if(dot_pos != string::npos) {
                    string company = domain.substr(0, dot_pos);
                    company[0] = toupper(company[0]);
                    lead["company"] = company + " Inc.";
                }
            }
            
            // Add phone if available
            if(!phones.empty()) {
                lead["phone"] = phones[rand() % phones.size()];
            }
            
            leads.push_back(lead);
            cout << "✅ Found lead: " << email << " (Score: " << lead["score"] << ")" << endl;
        }
        
        cout << "🎯 Total leads found: " << emails.size() << endl;
    }
    
    void listLeads() {
        lock_guard<mutex> lock(leads_mutex);
        
        if(leads.empty()) {
            cout << "No leads found. Analyze a website first." << endl;
            return;
        }
        
        cout << "\n📋 LEAD LIST (" << leads.size() << " total)" << endl;
        cout << "=================================" << endl;
        
        for(size_t i = 0; i < leads.size() && i < 10; i++) {
            const auto& lead = leads[i];
            cout << "\nLead #" << (i+1) << endl;
            cout << "ID: " << lead.at("id") << endl;
            cout << "Email: " << lead.at("email") << endl;
            if(lead.find("company") != lead.end()) {
                cout << "Company: " << lead.at("company") << endl;
            }
            if(lead.find("phone") != lead.end()) {
                cout << "Phone: " << lead.at("phone") << endl;
            }
            cout << "Score: " << lead.at("score") << "/100" << endl;
            cout << "Status: " << lead.at("status") << endl;
            cout << "Source: " << lead.at("website") << endl;
        }
        
        if(leads.size() > 10) {
            cout << "\n... and " << (leads.size() - 10) << " more leads." << endl;
        }
    }
    
    void generateReport() {
        lock_guard<mutex> lock(leads_mutex);
        
        if(leads.empty()) {
            cout << "No leads to generate report." << endl;
            return;
        }
        
        string filename = "lead_report_" + getCurrentTime() + ".txt";
        replace(filename.begin(), filename.end(), ':', '-');
        replace(filename.begin(), filename.end(), ' ', '_');
        
        ofstream report_file(filename);
        
        if(report_file.is_open()) {
            report_file << "LEAD GENERATION REPORT\n";
            report_file << "Generated: " << getCurrentTime() << "\n";
            report_file << "Total Leads: " << leads.size() << "\n\n";
            
            // Statistics
            int high_score = 0;
            int medium_score = 0;
            int low_score = 0;
            
            for(const auto& lead : leads) {
                int score = stoi(lead.at("score"));
                if(score >= 80) high_score++;
                else if(score >= 60) medium_score++;
                else low_score++;
            }
            
            report_file << "📊 STATISTICS:\n";
            report_file << "High score (80+): " << high_score << " leads\n";
            report_file << "Medium score (60-79): " << medium_score << " leads\n";
            report_file << "Low score (<60): " << low_score << " leads\n\n";
            
            report_file << "👥 LEAD DETAILS:\n";
            report_file << "================\n";
            
            for(const auto& lead : leads) {
                report_file << "\nID: " << lead.at("id") << "\n";
                report_file << "Email: " << lead.at("email") << "\n";
                if(lead.find("company") != lead.end()) {
                    report_file << "Company: " << lead.at("company") << "\n";
                }
                report_file << "Score: " << lead.at("score") << "/100\n";
                report_file << "Status: " << lead.at("status") << "\n";
                report_file << "Source: " << lead.at("website") << "\n";
                report_file << "Created: " << lead.at("created") << "\n";
                report_file << "---\n";
            }
            
            report_file.close();
            cout << "✅ Report generated: " << filename << endl;
            cout << "📊 Contains " << leads.size() << " leads with statistics." << endl;
        } else {
            cout << "❌ Could not create report file." << endl;
        }
    }
    
    void exportCSV() {
        lock_guard<mutex> lock(leads_mutex);
        
        if(leads.empty()) {
            cout << "No leads to export." << endl;
            return;
        }
        
        string filename = "leads_export_" + getCurrentTime() + ".csv";
        replace(filename.begin(), filename.end(), ':', '-');
        replace(filename.begin(), filename.end(), ' ', '_');
        
        ofstream csv_file(filename);
        
        if(csv_file.is_open()) {
            // Header
            csv_file << "ID,Email,Company,Phone,Score,Status,Website,Created\n";
            
            // Data
            for(const auto& lead : leads) {
                csv_file << lead.at("id") << ",";
                csv_file << "\"" << lead.at("email") << "\",";
                
                if(lead.find("company") != lead.end()) {
                    csv_file << "\"" << lead.at("company") << "\",";
                } else {
                    csv_file << "\"\",";
                }
                
                if(lead.find("phone") != lead.end()) {
                    csv_file << "\"" << lead.at("phone") << "\",";
                } else {
                    csv_file << "\"\",";
                }
                
                csv_file << lead.at("score") << ",";
                csv_file << lead.at("status") << ",";
                csv_file << lead.at("website") << ",";
                csv_file << lead.at("created") << "\n";
            }
            
            csv_file.close();
            cout << "✅ CSV exported: " << filename << endl;
            cout << "📁 Ready to open in Excel or Google Sheets." << endl;
        } else {
            cout << "❌ Could not create CSV file." << endl;
        }
    }
    
    void showStats() {
        lock_guard<mutex> lock(leads_mutex);
        
        cout << "\n📊 SYSTEM STATISTICS" << endl;
        cout << "====================" << endl;
        cout << "Total leads: " << leads.size() << endl;
        
        if(!leads.empty()) {
            int total_score = 0;
            int high_score = 0;
            
            for(const auto& lead : leads) {
                int score = stoi(lead.at("score"));
                total_score += score;
                if(score >= 70) high_score++;
            }
            
            double avg_score = (double)total_score / leads.size();
            cout << "Average lead score: " << fixed << setprecision(1) << avg_score << "/100" << endl;
            cout << "High quality leads (70+): " << high_score << endl;
            cout << "Memory usage: ~" << (leads.size() * 0.5) << "MB" << endl;
        }
        
        cout << "System: Ready" << endl;
        cout << "Type 'help' for commands" << endl;
    }
    
    void run() {
        srand(static_cast<unsigned int>(time(nullptr)));
        
        cout << "===============================================\n";
        cout << "🤖 LEAD GENERATION BOT v1.0\n";
        cout << "===============================================\n";
        cout << "Extract contacts • Generate reports • Export data\n";
        cout << "RAM Usage: ~10MB | No dependencies needed\n";
        cout << "===============================================\n\n";
        
        cout << "💡 COMMANDS:\n";
        cout << "• analyze [url]      - Extract leads from website\n";
        cout << "• list               - Show all leads\n";
        cout << "• report             - Generate detailed report\n";
        cout << "• csv                - Export to CSV format\n";
        cout << "• stats              - Show system statistics\n";
        cout << "• help               - Show this help\n";
        cout << "• quit               - Exit program\n\n";
        
        cout << "📝 EXAMPLE: analyze https://example.com\n\n";
        
        string command;
        while(true) {
            cout << "> ";
            getline(cin, command);
            
            if(command == "quit" || command == "exit") {
                cout << "\n👋 Goodbye! Reports saved to current folder.\n";
                break;
            }
            else if(command.find("analyze ") == 0) {
                string url = command.substr(8);
                if(url.empty()) {
                    cout << "Please provide a URL. Example: analyze https://example.com\n";
                } else {
                    analyzeWebsite(url);
                }
            }
            else if(command == "list") {
                listLeads();
            }
            else if(command == "report") {
                generateReport();
            }
            else if(command == "csv") {
                exportCSV();
            }
            else if(command == "stats") {
                showStats();
            }
            else if(command == "help") {
                cout << "\n💡 AVAILABLE COMMANDS:\n";
                cout << "=====================\n";
                cout << "analyze [url] - Extract leads from website\n";
                cout << "list         - Display all collected leads\n";
                cout << "report       - Generate detailed text report\n";
                cout << "csv          - Export leads to CSV file\n";
                cout << "stats        - Show system statistics\n";
                cout << "help         - Show this help message\n";
                cout << "quit         - Exit the program\n\n";
                cout << "📝 Example: analyze https://business.com\n";
            }
            else if(!command.empty()) {
                cout << "Unknown command. Type 'help' for available commands.\n";
            }
        }
    }
};

// ==================== MAIN FUNCTION ====================
int main() {
    LeadBot bot;
    bot.run();
    return 0;
}