#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

string target_date;

string parse_date(const string& line) {
    if (line.size() < 10) return "";
    return line.substr(0, 10);
}

streampos find_start_position(const string& file_path) {
    ifstream file(file_path, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << file_path << endl;
        exit(1);
    }
    streampos file_size = file.tellg();
    streampos low = 0;
    streampos high = file_size;

    while (low < high) {
        streampos mid = (low + high) / 2;
        file.seekg(mid);

        // Adjust to start of line
        if (mid > 0) {
            char ch;
            do {
                mid--;
                file.seekg(mid);
                file.get(ch);
            } while (mid > 0 && ch != '\n');
            if (ch == '\n') {
                mid++;
            }
        }

        file.seekg(mid);
        string line;
        string current_date;
        if (getline(file, line)) {
            current_date = parse_date(line);
        } else {
            current_date = "9999-99-99";
        }

        if (current_date < target_date) {
            streampos after_line = file.tellg();
            if (after_line == streampos(-1)) {
                low = file_size;
            } else {
                low = after_line;
            }
        } else {
            high = mid;
        }
    }

    file.close();
    return low;
}

void extract_logs(const string& input_path, streampos start_pos, const string& output_path) {
    ifstream in(input_path, ios::binary);
    ofstream out(output_path);
    if (!in.is_open()) {
        cerr << "Failed to open input file: " << input_path << endl;
        exit(1);
    }
    if (!out.is_open()) {
        cerr << "Failed to open output file: " << output_path << endl;
        exit(1);
    }

    in.seekg(start_pos);
    string line;
    while (getline(in, line)) {
        string current_date = parse_date(line);
        if (current_date == target_date) {
            out << line << '\n';
        } else if (current_date > target_date) {
            break;
        }
    }

    in.close();
    out.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " YYYY-MM-DD" << endl;
        return 1;
    }
    target_date = argv[1];
    string input_path = "logs_2024.txt";  
    string output_dir = "output";

    fs::create_directories(output_dir);
    string output_path = output_dir + "/output_" + target_date + ".txt";

    streampos start_pos = find_start_position(input_path);
    extract_logs(input_path, start_pos, output_path);

    return 0;
}
