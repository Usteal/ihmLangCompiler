#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <limits.h>
#include <windows.h>

void readFile(const std::string& filePath, std::string& content) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        exit(1);
    };
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
};

void deleteCppFile(const std::string& filePath) {
    std::string cppFilePath = filePath.substr(0, filePath.find_last_of('.')) + ".cpp";
    std::filesystem::remove(cppFilePath);
};

bool fileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
};

std::string getFileName(const std::string& filePath) {
    return std::filesystem::path(filePath).stem().string();
};

std::string getLibPath() {
    char buffer[MAX_PATH];
    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) == 0) {
        std::cerr << "Error: Unable to determine executable path" << std::endl;
        exit(1);
    };

    std::filesystem::path exePath(buffer);
    std::filesystem::path libPath = exePath.parent_path() / ".." / "lib";
    return libPath.string();
};

std::vector<std::string> getLines(const std::string& codeStr) {
    std::vector<std::string> lines;
    std::stringstream ss(codeStr);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        };
    };
    return lines;
};

std::string compileCommand(const std::string& filePath, const std::string& extension) {
    std::string fileName = getFileName(filePath);
    std::string command = "g++ -c " + filePath;

    std::string libPath = getLibPath();
    command += " -I \"" + libPath + "\"";

    for (const auto& entry : std::filesystem::directory_iterator(libPath)) {
        if (entry.path().extension() == ".o") {
            command += " " + entry.path().string();
        };
    };

    command += " -o " + fileName + "." + extension;

    return command;
};

std::vector<std::string> compile(const std::vector<std::string>& lines) {

    if (lines[0] != "__Use__.ONSTART - __ONSTART__;") {
        std::cout << "Error: ONSTART Collision" << std::endl;
        return {};
    };

    std::vector<std::string> final_lines;
    bool in_onstart_block = false;
    std::vector<std::string> onstart_code;

    for (const auto& line : lines) {
        std::string trimmed_line = line;
        size_t first_non_space = trimmed_line.find_first_not_of(" \t");
        if (first_non_space != std::string::npos) {
            trimmed_line = trimmed_line.substr(first_non_space);
        };

        if (trimmed_line == "__ONSTART__") {
            in_onstart_block = true;
        } else if (trimmed_line == "__END__") {
            in_onstart_block = false;
        } else if (in_onstart_block) {
            onstart_code.push_back(trimmed_line);
        } else if (trimmed_line.substr(0, 5) == "use::") {
            std::string library_name = trimmed_line.substr(4);
            final_lines.push_back("#include \"" + library_name + "\"");
        } else if (trimmed_line.substr(0, 8) == "__Use__.") {
            std::string function_call = trimmed_line.substr(8);        // Remove "__Use__ "
            final_lines.push_back(function_call + ";");
        };
    };

    final_lines.push_back("int main() {");
    for (const auto& line : onstart_code) {
        if (line.substr(0, 8) == "__Use__.") {
            std::string function_call = line.substr(8);                // Remove "__Use__ "
            final_lines.push_back("    " + function_call + ";");
        } else {
            final_lines.push_back("    " + line);
        };
    };
    final_lines.push_back("    return 0;");
    final_lines.push_back("}");

    return final_lines;
};

void save(const std::vector<std::string>& output, const std::string& filePath) {
    std::string binaryPath = filePath.substr(0, filePath.find_last_of('.')) + ".cpp";
    std::ofstream binaryFile(binaryPath, std::ios::binary);
    if (!binaryFile) {
        std::cerr << "Error: Unable to write to file" << binaryPath << std::endl;
        exit(1);
    };
    for (const auto& line : output) {
        binaryFile << line << "\n";
    };
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "InputError: Input file missing" << std::endl;
        return 1;
    };

    std::string filePath = argv[1];
    if (filePath.substr(filePath.find_last_of('.') + 1) != "ihm") {
        std::cerr << "InputError: Unrecognized file type" << std::endl;
        return 1;
    };

    std::string extension;

    if (argc > 2) {
        extension = argv[2]; 
    } else {
        extension = "o";
    };

    std::string codeStr;
    readFile(filePath, codeStr);
    std::vector<std::string> lines = getLines(codeStr);
    std::vector<std::string> output = compile(lines);
    save(output, filePath);

    std::string cppFile = getFileName(filePath) + ".cpp";
    if (extension == "cpp") {
        std::cout << "Compiled to cpp" << std::endl;
        std::cout << "\n";
        return 0;
    };

    bool fileWas = false;
    if (!fileExists(getFileName(filePath) + ".cpp")) {
        fileWas = true;
    };

    std::string compilation = compileCommand(filePath, extension);
    system(compilation.c_str());

    if (!fileWas) {
        deleteCppFile(cppFile);
    };

    if (extension == "o" or extension == "obj") {
        std::cout << "\n";
        std::cout << "Compiled to object" << std::endl;
        std::cout << "\n";
        return 0;
    } else {
        std::cout << "Compiled to " << extension << std::endl;
        std::cout << "\n";
    };

    return 0;
};
