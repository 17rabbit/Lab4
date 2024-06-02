#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>

namespace fs = std::filesystem;

// ������� ��� ��������, �� ���� � ����������
bool isHidden(const fs::path& p) {
    DWORD attrs = GetFileAttributes(p.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_HIDDEN);
}

// ������� ��� ��������� ������� �����, ��������� �������� ������� � ������� �������
int countFiles(const fs::path& directory, const std::string& pattern, bool include_hidden, bool include_readonly, bool include_archive) {
    int count = 0;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (fs::is_regular_file(entry) && entry.path().filename().string().find(pattern) != std::string::npos) {
            auto perms = fs::status(entry).permissions();
            bool is_readonly = (perms & fs::perms::owner_write) == fs::perms::none;
            bool is_archive = (perms & fs::perms::owner_exec) == fs::perms::none;
            bool matches = true;

            // Գ�������� ����� �� ����������
            if ((isHidden(entry.path()) && !include_hidden) || (is_readonly && !include_readonly) || (is_archive && !include_archive)) {
                matches = false;
            }

            if (matches) {
                count++;
            }
        }
    }
    return count;
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251); 
    SetConsoleOutputCP(1251);

    int exit_code = 0;
    // �������� �������� ��������� ���������� ����� ��� ������ �������
    if (argc == 1) {
        std::cout << "������������: " << argv[0] << " <�������> [������] [�����]\n";
        std::cout << "�����:\n";
        std::cout << "  hidden      �������� �������� �����\n";
        std::cout << "  reading    �������� ����� ����� ��� �������\n";
        std::cout << "  archive     �������� ������ �����\n";
        return 2; // ��� ���������� ��� ������� ���������
    }

    // ������� ����� ���������� �����
    bool include_hidden = false;
    bool include_readonly = false;
    bool include_archive = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "help") {
            std::cout << "������������: " << argv[0] << " <�������> [������] [�����]\n";
            std::cout << "�����:\n";
            std::cout << "  hidden      �������� �������� �����\n";
            std::cout << "  reading    �������� ����� ����� ��� �������\n";
            std::cout << "  archive     �������� ������ �����\n";
            return 0; // ��� ���������� ��� �����
        }
        else if (arg == "hidden") {
            include_hidden = true;
        }
        else if (arg == "reading") {
            include_readonly = true;
        }
        else if (arg == "archive") {
            include_archive = true;
        }
    }

    // ��������� �������� �� ������� �� ��������� ���������� �����
    std::string directory;
    std::string pattern = "*"; // �� �������������, ������ '*' ������ �� �����

    if (argc >= 2) {
        directory = argv[1];
    }
    if (argc >= 3) {
        pattern = argv[2];
    }

    try {
        // ϳ�������� ������� ����� � �����������, �� ���������� �������� �������
        int fileCount = countFiles(directory, pattern, include_hidden, include_readonly, include_archive);

        // ���� ����������
        std::cout << "ʳ������ �����, �� ���������� ������� '" << pattern << "' � ������� '" << directory << "': " << fileCount << std::endl;
        exit_code = 0; // ��� ���������� ��� �����
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "������� ������� �������: " << e.what() << std::endl;
        exit_code = 3; // ��� ���������� ��� ������� ������� �������
    }
    catch (const std::exception& e) {
        std::cerr << "�������: " << e.what() << std::endl;
        exit_code = 1; // ��� ���������� ��� �������� �������
    }

    // ���� ���� ����������
    std::cout << "��� ����������: " << exit_code << std::endl;
    return exit_code;
}