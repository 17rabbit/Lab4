#include <iostream>
#include <filesystem>
#include <string>
#include <windows.h>

namespace fs = std::filesystem;

// Функція для перевірки, чи файл є прихованим
bool isHidden(const fs::path& p) {
    DWORD attrs = GetFileAttributes(p.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_HIDDEN);
}

// Функція для підрахунку кількості файлів, відповідних заданому шаблону в певному каталозі
int countFiles(const fs::path& directory, const std::string& pattern, bool include_hidden, bool include_readonly, bool include_archive) {
    int count = 0;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (fs::is_regular_file(entry) && entry.path().filename().string().find(pattern) != std::string::npos) {
            auto perms = fs::status(entry).permissions();
            bool is_readonly = (perms & fs::perms::owner_write) == fs::perms::none;
            bool is_archive = (perms & fs::perms::owner_exec) == fs::perms::none;
            bool matches = true;

            // Фільтрація файлів за атрибутами
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
    // Перевірка наявності параметрів командного рядка для режиму підказки
    if (argc == 1) {
        std::cout << "Використання: " << argv[0] << " <каталог> [шаблон] [опції]\n";
        std::cout << "Опції:\n";
        std::cout << "  hidden      Включити приховані файли\n";
        std::cout << "  reading    Включити файли тільки для читання\n";
        std::cout << "  archive     Включити архівні файли\n";
        return 2; // Код завершення для помилок аргументів
    }

    // Обробка опцій командного рядка
    bool include_hidden = false;
    bool include_readonly = false;
    bool include_archive = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "help") {
            std::cout << "Використання: " << argv[0] << " <каталог> [шаблон] [опції]\n";
            std::cout << "Опції:\n";
            std::cout << "  hidden      Включити приховані файли\n";
            std::cout << "  reading    Включити файли тільки для читання\n";
            std::cout << "  archive     Включити архівні файли\n";
            return 0; // Код завершення для успіху
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

    // Отримання каталогу та шаблону із аргументів командного рядка
    std::string directory;
    std::string pattern = "*"; // За замовчуванням, шаблон '*' означає всі файли

    if (argc >= 2) {
        directory = argv[1];
    }
    if (argc >= 3) {
        pattern = argv[2];
    }

    try {
        // Підрахунок кількості файлів в підкаталогах, що відповідають заданому шаблону
        int fileCount = countFiles(directory, pattern, include_hidden, include_readonly, include_archive);

        // Вивід результату
        std::cout << "Кількість файлів, що відповідають шаблону '" << pattern << "' у каталозі '" << directory << "': " << fileCount << std::endl;
        exit_code = 0; // Код завершення для успіху
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Помилка файлової системи: " << e.what() << std::endl;
        exit_code = 3; // Код завершення для помилок файлової системи
    }
    catch (const std::exception& e) {
        std::cerr << "Помилка: " << e.what() << std::endl;
        exit_code = 1; // Код завершення для загальної помилки
    }

    // Вивід коду завершення
    std::cout << "Код завершення: " << exit_code << std::endl;
    return exit_code;
}