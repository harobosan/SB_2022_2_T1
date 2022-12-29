#include <iostream>     // std::cout, std::endl
#include <fstream>      // std::ifstream, std::ofstream
#include <string.h>     // std::string, std::strcmp
#include <vector>       // std::vector
#include <utility>      // std::pair, std::make_pair

using namespace std;

void print_tokens(vector<vector<string>> tokens)
{
    for(int i = 0; i < tokens.size(); i++) {
        cout << "|";
        for(int j = 0; j < tokens[i].size(); j++) {
            cout << " " << ((tokens[i][j] != "\0") ? tokens[i][j] : "EOL") << " |";
        }
        cout << endl;
    }
    cout << endl;
}

vector<vector<string>> parser(string name, string extension)
{
    vector<vector<string>> tokens;

    ifstream file;
    file.open(name+extension);

    if(file.is_open()) {
        string line;
        getline(file, line);
        while(file) {
            if(line.size() > 0) {
                vector<string> newline;
                int i = 0;
                int j = 0;
                for(j = 0; j < line.size(); j++) {
                    if(line[j] == ' ' || line[j] == ',') {
                        if(i != j) {
                            newline.push_back(line.substr(i,j-i));
                        }
                        if(line[j] == ',') {
                            newline.push_back(line.substr(j,1));
                        }
                        i = j+1;
                    }
                    else if(line[j] == ';') {
                        break;
                    }
                }
                if(i != j) {
                    newline.push_back(line.substr(i,j-i));
                }
                if(newline.size() > 0) {
                    tokens.push_back(newline);
                }
            }
            getline(file, line);
        }

        file.close();
    }
    else {
        cout << "[ERRO] ARQUIVO " << name+extension << "NÃO FOI ENCONTRADO" << endl;
    }

    return tokens;
}

void writer(vector<vector<string>> tokens, string name, string extension)
{
    ofstream file;
    file.open(name+extension);
    for(int i = 0; i < tokens.size(); i++) {
        for(int j = 0; j < tokens[i].size(); j++) {
            file << tokens[i][j];
            if(j < tokens[i].size()-1) {
                if(tokens[i][j] != ",") {
                    file << " ";
                }
            }
        }
        file << endl;
    }
    file.close();
}

/*vector<vector<string>>*/ void pre_processing(vector<vector<string>> tokens, string name)
{
    vector<pair<string,string>> PET;
    for(int i = 0; i < tokens.size(); i++) {
        for(int j = tokens[i].size()-1; j >= 0; j--) {
            for(int k = 0; k < PET.size(); k++) {
                if(tokens[i][j] == PET[k].first) {
                    tokens[i][j] = PET[k].second;
                }
            }
            if(tokens[i][j] == "EQU") {
                if(j > 0 && j+1 < tokens[i].size()) {
                    PET.push_back(make_pair(tokens[i][j-1],tokens[i][j+1]));
                    PET.back().first.pop_back();
                    tokens.erase(tokens.begin()+i);
                    i--;
                    break;
                }
            }
            if(tokens[i][j] == "IF") {
                if(j+1 < tokens[i].size()) {
                    if(tokens[i][j+1] == "0") {
                        if(i+1 < tokens.size()) {
                            tokens.erase(tokens.begin()+i+1);
                        }
                    }
                }
                tokens.erase(tokens.begin()+i);
                i--;
                break;
            }
        }
    }

    writer(tokens, name, ".PRE");
    //return tokens;
}

/*vector<vector<string>>*/ void macros(vector<vector<string>> tokens, string name)
{
    vector<vector<string>> MNT;
    vector<vector<string>> MDT;
    for(int i = 0; i < tokens.size(); i++) {
        for(int j = 0; j < tokens[i].size(); j++) {
            if(tokens[i][j] == "MACRO" && j-1 >= 0) {
                vector<string> macro;
                macro.push_back(tokens[i][j-1]);
                macro.back().pop_back();
                macro.push_back("0");
                macro.push_back(to_string(MDT.size()));
                j++;
                while(j < tokens[i].size()) {
                    if(tokens[i][j] != ",") {
                        macro.push_back(tokens[i][j]);
                    }
                    j++;
                }
                macro[1] = to_string(macro.size()-3);
                MNT.push_back(macro);
                tokens.erase(tokens.begin()+i);

                for(j = i; j < tokens.size(); j++) {
                    for(int k = 0; k < tokens[j].size(); k++) {
                        if(tokens[j][k] == "ENDMACRO") {
                            break;
                        }
                    }
                    if(tokens[j][0] == "ENDMACRO") {
                        MDT.push_back(tokens[j]);
                        i--;
                        for(int k = j; k > i; k--) {
                            tokens.erase(tokens.begin()+k);
                        }
                        break;
                    }
                    else {
                        MDT.push_back(tokens[j]);
                        if(stoi(MNT.back()[1])) {
                            for(int k = 0; k < MDT.back().size(); k++) {
                                for(int l = 3; l < 3+stoi(MNT.back()[1]); l++) {
                                    if(MDT.back()[k] == MNT.back()[l]) {
                                        MDT.back()[k] = "#"+to_string(l-2);
                                    }
                                }
                            }
                        }
                    }
                }

                for(j = 2+stoi(MNT.back()[1]); j > 2; j--) {
                    MNT.back().erase(MNT.back().begin()+j);
                }
            }
        }
    }

    //meta macro check

    for(int i = 0; i < tokens.size(); i++) {
        for(int j = 0; j < tokens[i].size(); j++) {
            for(int k = 0; k < MNT.size(); k++) {
                if(tokens[i][j] == MNT[k][0]) {
                    vector<string> args;
                    if(stoi(MNT[k][1]) > 0) {
                        for(int l = 0; l < stoi(MNT[k][1]); l++) {
                            if(j+1+2*l < tokens[i].size()) {
                                args.push_back(tokens[i][j+1+2*l]);
                            }
                        }
                    }
                    tokens.erase(tokens.begin()+i);
                    for(int l = stoi(MNT[k][2]); l < MDT.size(); l++) {
                        if(MDT[l][0] == "ENDMACRO") {
                            break;
                        }
                        int it = i+l-stoi(MNT[k][2]);
                        tokens.insert(tokens.begin()+it, MDT[l]);
                        for(int m = 0; m < tokens[it].size(); m++) {
                            if(tokens[it][m][0] == '#') {
                                tokens[it][m].erase(tokens[it][m].begin());
                                if(stoi(tokens[it][m])-1 < args.size()) {
                                    tokens[it][m] = args[stoi(tokens[it][m])-1];
                                }
                                else {
                                    cout << "[ERRO] LINHA " << i << ": " << MNT[k][0] << " FORAM DADOS " << args.size();
                                    cout << " ARGUMENTOS, ESPERA-SE " << MNT[k][1] << endl;
                                    tokens[it][m].insert(tokens[it][m].begin(), '#');
                                }
                            }
                        }
                    }
                    i--;
                }
            }
        }
    }

    writer(tokens, name, ".MCR");
    //return tokens;
}

void object(vector<string> tokens)
{

}

int main(int argc, char* argv[])
{
    if(argc == 3) {
        if(!strcmp(argv[1], "-p")) {
            vector<vector<string>> tokens = parser(argv[2], ".ASM");
            if(!tokens.empty()) {
                pre_processing(tokens, argv[2]);
            }
            else {
                //cout << "DEU RUIM";
            }
        }
        else if(!strcmp(argv[1], "-m")) {
            vector<vector<string>> tokens = parser(argv[2], ".PRE");
            if(!tokens.empty()) {
                macros(tokens, argv[2]);
            }
            else {
                //cout << "DEU RUIM";
            }
        }
        else if(!strcmp(argv[1], "-o")) {
            vector<vector<string>> tokens = parser(argv[2], ".MCR");
        }
        else {
            cout << "[ERRO] OPERADOR " << argv[1] << " DESCONHECIDO" << endl;
            cout << "[INFO] TENTE EXECUTAR O COMANDO NO FORMATO: montador -<op> <arquivo>" << endl;
            cout << "[INFO] PARA MAIS INFORMAÇÕES DIGITE: montador -h" << endl;
        }
    }
    else if (argc > 3) {
        cout << "[ERRO] MUITOS ARGUMENTOS" << endl;
    }
    else { // argc < 3
        if(argc == 2) {
            if(!strcmp(argv[1], "-h")) {
               cout << "HELP";
            }
            else {
                cout << "[ERRO] FALTAM ARGUMENTOS" << endl;
            }
        }
        else {
            cout << "[ERRO] FALTAM ARGUMENTOS" << endl;
        }
    }

    return 0;
}
