// Garant_Prog.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <filesystem>
#include<fstream>

#define lli long long

using namespace std;
namespace fs = std::filesystem;


vector <string> PathInit() {
    setlocale(LC_CTYPE, "rus");
    string logFolderName = "TopicFinderLog\\PathFile.txt";
    string pt = fs::current_path().generic_string() + '\\' + logFolderName;
    //cout << pt;
    ifstream fin(pt);
    
    string log; getline(fin, log);
    string nsrc; getline(fin, nsrc);
    string img; getline(fin, img);
    
    fin.close();

    string logPath = "", nsrcPath = "", imgPath = "";
    
    bool flag = 1;
    int ind = log.find("logs:");
    if (ind != -1) {
        logPath = log.substr(ind + 6);
        cout << log << "\n\n";
    }
    else {
        cout << "Неправильный формат записи пути к катологу с логами, пожалуйста проверьте файл PathFile.txt. \nСтрока должна иметь вид \"logs: /полный_путь_к_log_файлам\" \n\n";
        flag = 0;
    }

    ind = nsrc.find("nsrc files:");
    if (ind != -1) {
        nsrcPath = nsrc.substr(ind + 12);
        cout << nsrc << "\n\n";
    }
    else {
        cout << "Неправильный формат записи пути к катологу с *.nsrc файлами, пожалуйста проверьте файл PathFile.txt. \nСтрока должна иметь вид \"nsrc files: /полный_путь_к_nsrc_файлам\" \n\n";
        flag = 0;
    }

    ind = img.find("img:");
    if (ind != -1) {
        imgPath = img.substr(ind + 5);
        cout << img << "\n\n";
    }
    else {
        cout << "Неправильный формат записи пути к катологу с картинками, пожалуйста проверьте файл PathFile.txt. \nСтрока должна иметь вид \"img: /полный_путь_к_файлам\" \n\n";
        flag = 0;
    }
    
    if (!flag) {
        return {};
    }

    cout << "Если пути к каталогам верны, нажмите Enter. Иначе проверьте правильность данных в файле \"...\\TopicFinderLog\\PathFile.txt\"";
    char c = ' ';
    c = getchar();
    if (c != '\n') {
        return {};
    }

    return { logPath, nsrcPath, imgPath };
}

vector<string> ReadLogs(string logPath) {
    setlocale(LC_CTYPE, "rus");
    const int _primeConst = 100000;
    
    fs::path path;
    vector<string> logFilePath;
    for (auto& it : fs::directory_iterator(logPath)) {
        path = it;
        if (path.extension() == ".log") {
            logFilePath.push_back(path.fs::path::generic_string());
        }
    }

    vector<string> comandCodeID;

    for (lli it = 0; it < logFilePath.size(); it++) {
        ifstream fin(logFilePath[it].c_str());

        string temp;
        while (getline(fin, temp)) {
            temp.erase(temp.end() - 1);
            //cout << temp;
            comandCodeID.push_back("!OBJTOPIC " + to_string(stoi(temp.substr(0, temp.find(','))) - _primeConst));
        }
        fin.close();
    }

    /*for (int it = 0; it < comandCodeID.size(); it++) {
        cout << comandCodeID[it] << '\n';
    }*/

    return comandCodeID;
}

vector<string> FindComandLine(string nsrcPath, vector<string> comandCodeID) {
    
    setlocale(LC_CTYPE, "rus");

    fs::path path;
    vector<string> nsrcFilePath;
    for (auto& it : fs::directory_iterator(nsrcPath)) {
        path = it;
        if (path.extension() == ".nsr") {
            nsrcFilePath.push_back(path.fs::path::generic_string());
        }
    }

    /*for (int it = 0; it < nsrcFilePath.size(); it++) {
        cout << nsrcFilePath[it] << '\n';
    }*/

    vector<string> imgFilePath = {};
    vector<vector<string>> progLog (nsrcFilePath.size());
    
    for (lli it = 0; it < nsrcFilePath.size(); it++) {
        ifstream fin(nsrcFilePath[it].c_str());
        
        
        bool flag = 0;
        string temp;

        vector<string> newFile;

        lli strNum = 0;

        while (getline(fin, temp)) {

            int pos = temp.find("!OBJTOPIC ");
            
            if (pos == -1) {
                if (!flag) {
                    newFile.push_back(temp);
                }
                else {
                    if (temp.find("!OBJPATH ") != -1){
                        string ptemp = temp.substr(9);
                        progLog[it][strNum] += ptemp;
                        imgFilePath.push_back(ptemp);
                    }
                    if (temp == ";") {
                        strNum++;
                        flag = 0;
                    }
                }
            }
            else {
                bool fgs = 0;
                for (int it2 = 0; it2 < comandCodeID.size(); it2++) {
                    if (temp.find(comandCodeID[it2]) != -1) {
                        fgs = 1;
                        progLog[it].push_back("Топик: " + comandCodeID[it2] + "     Файл: ");
                        comandCodeID.erase(comandCodeID.begin() + it2);
                        break;
                    }
                }
                if (!fgs) {
                    newFile.push_back(temp);
                }
                else {
                    flag = 1;
                }
            }
        }
        fin.close();

        ofstream fout(nsrcFilePath[it].c_str());

        for (lli it2 = 0; it2 < newFile.size(); it2++) {
            fout << newFile[it2] << '\n';
        }

        fout.close();

        /*for (lli it2 = 0; it2 < imgFilePath.size(); it2++) {
            cout << imgFilePath[it2] << '\n';
        }*/

    }

    string logFolderName = "TopicFinderLog";
    ofstream fout(fs::current_path().generic_string() + '\\' + logFolderName + '\\' + "TopicFinderLog.txt");

    for (lli it = 0; it < progLog.size(); it++) {
        if (!progLog[it].empty()) {
            fout << nsrcFilePath[it] << "\n";
            for (lli it2 = 0; it2 < progLog[it].size(); it2++) {
                fout << ("          " + progLog[it][it2] + "\n\n");
            }
        }
    }

    fout.close();

    return imgFilePath;
}

void imgDelete(string imgPath, vector<string> imgToDelete) {

    setlocale(LC_CTYPE, "rus");

    for (lli it = 0; it < imgToDelete.size(); it++) {
        //cout << imgPath + "    " + imgToDelete[it] << '\n';
        fs::remove(imgPath + '\\' + imgToDelete[it]);
    }

}

bool FileExist() {

    setlocale(LC_CTYPE, "rus");
    
    string logFolderName = "TopicFinderLog";
    string path = fs::current_path().generic_string() + '\\' + logFolderName;

    //cout << path << '\n';

    if (!fs::exists(path)) {
        fs::create_directory(path);
    }

    path = path + '\\' + "PathFile.txt";

    if (!fs::exists(path)) {
        ofstream fout((path).c_str());

        fout << "logs: \nnsrc files: \nimg:";

        fout.close();

        cout << "Внесите пути к каталогам в соответсвующие строки, файла \"...\\TopicFinderLog\\PathFile.txt\", если это необходимо, после чего сохраните файл и перезапустите программу";
        system(("notepad.exe " + path).c_str());
        
        return 0;
    }

    return 1;
}

signed main()
{
    setlocale(LC_ALL, "Russian");
    
    if (!FileExist()) {
        return 0;
    }

    vector<string> path = PathInit();

    if (path.empty()) {
        cout << "Ошибка, неправильыне пути в фале PathFile.txt";
        return 0;
    }
    
    vector<string> comandCodeID = ReadLogs(path[0]);

    vector<string> imgToDelete = FindComandLine(path[1], comandCodeID);

    imgDelete(path[2], imgToDelete);

    cout << "\n\n ПРОГРАММА УСПЕШНО ЗАВЕРШИЛА СВОЮ РАБОТУ \n Логи можно найти тут \"...\\TopicFinderLog\\TopicFinderLog.txt\"\n\n";

    system("pause");

}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
