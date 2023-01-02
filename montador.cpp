#include <iostream>     // std::cout, std::endl
#include <fstream>      // std::ifstream, std::ofstream
#include <string>       // std::string, std::to_string, std::stoi
#include <vector>       // std::vector
#include <utility>      // std::pair, std::make_pair

using namespace std;

string c_lex = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
string n_lex = "0123456789";
string u_lex = "_";
vector<string> reserved = { "ADD","SUB","MUL","DIV","JMP","JMPN","JMPP","JMPZ","COPY",
                            "LOAD","STORE","INPUT","OUTPUT","STOP","SECTION","TEXT",
                            "DATA","CONST","SPACE","IF","EQU","MACRO","ENDMACRO" };

void print_tokens(vector<vector<string>> tokens)
{
    int line_size = 0;
    int token_size = 0;
    for(int i = 0; i < tokens.size(); i++) {
        if(tokens[i].size() >= line_size) {
            line_size = tokens[i].size();
            if(tokens[i][0].back() != ':') {
                line_size++;
            }
        }
        for(int j = 0; j < tokens[i].size(); j++) {
            if(tokens[i][j].size() > token_size) {
                token_size = tokens[i][j].size();
            }
        }
    }

    for(int i = 0; i < tokens.size(); i++) {
        cout << ((i+1 < 10)?"0":"") << i+1 << " |";
        int lbl = -1;
        for(int j = 0; j < tokens[i].size(); j++) {
            if(j == 0 && (tokens[i][j].back() == ':' || tokens[i][j] == "SECTION")) {
                for(int k = 0; k < token_size-tokens[i][j].size(); k++) {
                    cout << " ";
                }
                cout << " " << tokens[i][j];
                lbl = 0;
            }
            else if(j == 0) {
                for(int k = 0; k < token_size+1; k++) {
                    cout << " ";
                }
                cout << " | " << tokens[i][j];
                for(int k = 0; k < token_size-tokens[i][j].size(); k++) {
                    cout << " ";
                }
            }
            else {
                cout << " " << tokens[i][j];
                for(int k = 0; k < token_size-tokens[i][j].size(); k++) {
                    cout << " ";
                }
            }
            cout << " |";
        }
        for(int j = 0; j < line_size-tokens[i].size()+lbl; j++) {
            for(int k = 0; k < token_size; k++) {
                cout << " ";
            }
            cout << "  |";
        }
        cout << endl;
    }
    cout << endl;
}

char capitalize(char c)
{
    if(c > 96 && c < 123) {
        c -= 32;
    }
    return c;
}

int meta_macro(vector<vector<string>> MNT, vector<vector<string>> MDT)
{
    vector<vector<int>> rec;
    int macro = -1;
    for(int i = 0; i < MNT.size(); i++) {
        vector<int> ref;
        int end = 0;
        if(i < MNT.size()-1) {
            end = stoi(MNT[i+1][2]);
        }
        else {
            end = MDT.size();
        }
        for(int j = stoi(MNT[i][2]); j < end; j++) {
            for(int k = 0; k < MDT[j].size(); k++) {
                for(int l = 0; l < MNT.size(); l++) {
                    if(MDT[j][k] == MNT[l][0]) {
                        ref.push_back(l);
                    }
                }
            }
        }
        rec.push_back(ref);
    }

    for(int i = 0; i < rec.size(); i++) {
        for(int j = 0; j < rec.size(); j++) {
            for(int k = 0; k < rec[j].size(); k++) {
                if(rec[j][k] == i) {
                    for(int l = 0; l < rec[i].size(); l++) {
                        if(rec[i][l] == j) {
                            macro = j;
                        }
                    }
                }
            }
        }
    }

    return macro;
}

vector<vector<string>> tokenizer(string name, string extension)
{
    vector<vector<string>> tokens;

    ifstream file;
    file.open(name+extension);

    if(file.is_open()) {
        string line;
        getline(file, line);
        int lbl = 0;
        while(file) {
            if(line.size() > 0) {
                vector<string> newline;
                int i = 0;
                int j = 0;
                for(j = 0; j < line.size(); j++) {
                    if(line[j] == ' ' || line[j] == ',' || line[j] == '\t') {
                        if(i != j) {
                            newline.push_back(line.substr(i,j-i));
                        }
                        if(line[j] == ',') {
                            if(j > 0 && j+1 < line.size()) {
                                if(line[j-1] != ' ' && line[j-1] != ',' && line[j-1] != '\t' &&
                                   line[j+1] != ' ' && line[j+1] != ',' && line[j+1] != '\t') {
                                    newline.push_back(line.substr(j,1));
                                }
                            }
                        }
                        i = j+1;
                    }
                    else if(line[j] == ';' || line[j] == '\0' || line[j] == '\n' || line[j] == '\r') {
                        break;
                    }
                }
                if(i != j) {
                    newline.push_back(line.substr(i,j-i));
                }
                if(newline.size() > 0) {
                    if(newline.front().back() != ':' && lbl) {
                        newline.insert(newline.begin(),tokens.back().back());
                        tokens.pop_back();
                        lbl = 0;
                    }
                    for(int k = 0; k < newline.size(); k++) {
                        for(int l = 0; l < newline[k].size(); l++) {
                            newline[k][l] = capitalize(newline[k][l]);
                        }
                    }
                    tokens.push_back(newline);
                }
            }
            if(tokens.size() > 0) {
                if(tokens.back().size() == 1) {
                    if(tokens.back()[0].back() == ':') {
                        lbl = 1;
                    }
                }
            }
            getline(file, line);
        }

        file.close();
    }
    else {
        cout << "[ERRO] ARQUIVO "+name+extension+" NÃO FOI ENCONTRADO" << endl;
    }

    print_tokens(tokens);

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
                if(tokens[i][j] != "," && tokens[i][j+1] != ",") {
                    file << " ";
                }
            }
        }
        file << endl;
    }
    file.close();

    cout << "[INFO] "+name+extension+" GERADO COM SUCESSO" << endl;
}

string lexical_scanner(string token, int line_pos)
{
    int pos = token.find_first_not_of(c_lex+n_lex+u_lex);
    if(pos < token.size()) {
        if(token[pos] == ',') {
            if(token.size() > 1) {
                cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                cout << "       TENTE NO FORMATO <,>" << endl;
                return "ERROR";
            }
            else {
                return "SPACER";
            }
        }
        else if(token[pos] == ':') {
            if(token.size() < 2) {
                cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                cout << "       TENTE NO FORMATO <x:> <_x:> ou <x0:>" << endl;
                return "ERROR";
            }
            else if(pos != token.size()-1) {
                cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                cout << "       TENTE NO FORMATO <x:> <_x:> ou <x0:>" << endl;
                return "ERROR";
            }
            else if(token.find_first_not_of(c_lex+u_lex) == 0) {
                cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                cout << "       TENTE NO FORMATO <x:> <_x:> ou <x0:>" << endl;
                return "ERROR";
            }
            else {
                //TRATAR PALAVRAS RESERVADAS
                string temp = token;
                temp.pop_back();
                for(int i = 0; i < reserved.size(); i++) {
                    if(temp == reserved[i]) {
                        cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                        cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                        cout << "       USO DE PALAVRA RESERVADA COMO RÓTULO" << endl;
                        return "ERROR";
                    }
                }
                return "LABEL";
            }
        }
        else if(token[pos] == '+' || token[pos] == '-') {
            if(pos == token.size()-1) {
                cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                cout << "       TENTE NO FORMATO <x+1> <_x+1> ou <x0+1>" << endl;
                return "ERROR";
            }
            else if(pos == 0) {
                if(pos != token.find_last_not_of(n_lex)) {
                    cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                    cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                    cout << "       TENTE NO FORMATO <+1> ou <-2>" << endl;
                    return "ERROR";
                }
                else {
                    return "CONSTANT";
                }
            }
            else {
                if(token.find_last_of(c_lex+u_lex,pos-1) > pos) {
                    cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                    cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                    cout << "       TENTE NO FORMATO <x+1> <_x+1> ou <x0+1>" << endl;
                    return "ERROR";
                }
                else if(token.find_first_not_of(n_lex,pos+1) < token.size()) {
                    cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                    cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                    cout << "       TENTE NO FORMATO <x+1> <_x+1> ou <x0+1>" << endl;
                    return "ERROR";
                }
                else {
                    return "EXPRESSION";
                }
            }
        }
        else {
            cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
            cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
            cout << "       CHARACTERE " << token[pos] << " INVÁLIDO" << endl;
            return "ERROR";
        }
    }
    else {
        if(token.find_first_not_of(n_lex) < token.size()) {
            if(token == "SECTION") {
                return "SECTION";
            }
            else if(token == "TEXT") {
                return "TEXT";
            }
            else if(token == "DATA") {
                return "DATA";
            }
            else if(token == "STOP") {
                return "INSTRUCTION1";
            }
            else if(token == "ADD" || token == "SUB" || token == "MUL" || token == "DIV") {
                return "INSTRUCTION2";
            }
            else if(token == "JMP" || token == "JMPN" || token == "JMPP" || token == "JMPZ") {
                return "INSTRUCTION2";
            }
            else if(token == "LOAD" || token == "STORE" || token == "INPUT" || token == "OUTPUT") {
                return "INSTRUCTION2";
            }
            else if(token == "COPY") {
                return "INSTRUCTION3";
            }
            else if(token == "SPACE") {
                return "SPACE";
            }
            else if(token == "CONST") {
                return "CONST";
            }
            else {
                if(token.size() > 2) {
                    string sub = token.substr(2,token.size()-2);
                    if(token.substr(0,2) == "0X" && sub.find_first_not_of(n_lex) > sub.size()-1) {
                        return "CONSTANT";
                    }
                }
                if(token.find_first_not_of(c_lex+u_lex) == 0) {
                    cout << "[ERRO] LEXICO NA LINHA: " << line_pos << endl;
                    cout << "       TOKEN <" << token << "> MAL DEFINIDO" << endl;
                    cout << "       TENTE NO FORMATO <x> <_x> ou <x0>" << endl;
                    return "ERROR";
                }
                return "REFERENCE";
            }          
        }
        else {
            return "CONSTANT";
        }
    }
}

vector<pair<string,string>> syntax_scanner(vector<string> line, int line_pos)
{
    int i = 0;
    vector<pair<string,string>> linetable;
    for(int j = 0; j < line.size(); j++) {
        linetable.push_back(make_pair(line[j],lexical_scanner(line[j],line_pos)));
        if(linetable.back().second == "ERROR") {
            return vector<pair<string,string>>();
        }
    }
    while(i < linetable.size()) {
        if(linetable[0].second == "SECTION") {
            if(linetable.size() != 2) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       A DIRETIVA " << linetable[i].first << " ESPERA 1 ARGUMENTO"<< endl;
                cout << "       FORAM PASSADOS " << linetable.size()-1-i << " ARGUMENTOS" << endl;
                return vector<pair<string,string>>();
            }
            else {
                if(linetable[1].second != "TEXT" && linetable[1].second != "DATA") {
                        cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                        cout << "       TIPO DE ARGUMENTO INVÁLIDO PARA A DIRETIVA " << linetable[i].first << endl;
                        cout << "       ESPERA-SE TEXT OU DATA" << endl;
                    return vector<pair<string,string>>();
                }
                else{
                    return linetable;
                }
            }
        }
        else if(linetable[i].second == "LABEL") {
            if(i != 0) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       LINHA JÁ ROTULADA" << endl;
                return vector<pair<string,string>>();
            }
            else if(linetable.size() == 1) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       LINHA ROTULADA SEM INSTRUÇÃO" << endl;
                return vector<pair<string,string>>();
            }
        }
        else if(linetable[i].second == "INSTRUCTION3") {
            if(i+3 != linetable.size()-1) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       A INSTRUÇÃO " << linetable[i].first << " ESPERA 3 ARGUMENTOS (incluindo a vírgula de separador)"<< endl;
                cout << "       FORAM PASSADOS " << linetable.size()-1-i << " ARGUMENTOS" << endl;
                return vector<pair<string,string>>();
            }
            else {
                if((linetable[i+1].second != "REFERENCE" && linetable[i+1].second != "EXPRESSION") || linetable[i+2].second != "SPACER" ||
                   (linetable[i+3].second != "REFERENCE" && linetable[i+3].second != "EXPRESSION")) {
                    cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                    cout << "       TIPO DE ARGUMENTO INVÁLIDO PARA A INSTRUÇÃO " << linetable[i].first << endl;
                    return vector<pair<string,string>>();
                }
                else {
                    return linetable;
                }
            }
        }
        else if(linetable[i].second == "INSTRUCTION2") {
            if(i+1 != linetable.size()-1) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       A INSTRUÇÃO " << linetable[i].first << " ESPERA 1 ARGUMENTO"<< endl;
                cout << "       FORAM PASSADOS " << linetable.size()-1-i << " ARGUMENTOS" << endl;
                return vector<pair<string,string>>();
            }
            else if(linetable[i+1].second != "REFERENCE" && linetable[i+1].second != "EXPRESSION" && linetable[i+1].second != "CONSTANT") {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       TIPO DE ARGUMENTO INVÁLIDO PARA A INSTRUÇÃO " << linetable[i].first << endl;
                return vector<pair<string,string>>();
            }
            else {
                return linetable;
            }
        }
        else if(linetable[i].second == "INSTRUCTION1") {
            if(i+1 < linetable.size()) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       A INSTRUÇÃO " << linetable[i].first << " ESPERA 0 ARGUMENTOS"<< endl;
                cout << "       FORAM PASSADOS " << linetable.size()-1-i << " ARGUMENTOS" << endl;
                return vector<pair<string,string>>();
            }
            return linetable;
        }
        else if(linetable[i].second == "CONST") {
            if(i == 0) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       " << linetable[i].first << " DEVE SER PRECEDIDO DE UM RÓTULO" << endl;
                return vector<pair<string,string>>();
            }
            else if(i+1 != linetable.size()-1) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       A DIRETIVA " << linetable[i].first << " ESPERA 1 ARGUMENTO"<< endl;
                cout << "       FORAM PASSADOS " << linetable.size()-1-i << " ARGUMENTOS" << endl;
                return vector<pair<string,string>>();
            }
            else {
                if(linetable[i+1].second != "CONSTANT") {
                    cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                    cout << "       TIPO DE ARGUMENTO INVÁLIDO PARA A DIRETIVA " << linetable[i].first << endl;
                    return vector<pair<string,string>>();
                }
                else {
                    return linetable;
                }
            }
        }
        else if(linetable[i].second == "SPACE") {
            if(i == 0) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       " << linetable[i].first << " DEVE SER PRECEDIDO DE UM RÓTULO" << endl;
                return vector<pair<string,string>>();
            }
            else if(i+2 < linetable.size()) {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       A DIRETIVA " << linetable[i].first << " ESPERA de 0 a 1 ARGUMENTOS"<< endl;
                cout << "       FORAM PASSADOS " << linetable.size()-1-i << " ARGUMENTOS" << endl;
                return vector<pair<string,string>>();
            }
            else {
                if(i+1 < linetable.size()) {
                    if(linetable[i+1].second != "CONSTANT") {
                        cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                        cout << "       TIPO DE ARGUMENTO INVÁLIDO PARA A DIRETIVA " << linetable[i].first << endl;
                        return vector<pair<string,string>>();
                    }
                    else {
                        if(stoi(linetable[i+1].first) < 0) {
                            cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                            cout << "       TIPO DE ARGUMENTO INVÁLIDO PARA A DIRETIVA " << linetable[i].first << endl;
                            return vector<pair<string,string>>();
                        }
                        else {
                            return linetable;
                        }
                    }
                }
                else {
                    return linetable;
                }
            }
        }
        else {
                cout << "[ERRO] SINTATICO NA LINHA: " << line_pos << endl;
                cout << "       INSTRUÇÃO OU DIRETIVA " << linetable[i].first << " DESCONHECIDA" << endl;
                return vector<pair<string,string>>();
        }
        i++;
    }
    return linetable;
}

void pre_processing(vector<vector<string>> tokens, string name)
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
}

void macros(vector<vector<string>> tokens, string name)
{
    //GENERATE MACRO TABLES
    vector<vector<string>> MNT;
    vector<vector<string>> MDT;
    int l_offset = 1;
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
                for(int k = 0; k < MNT.size(); k++) {
                    if(MNT[k][0] == macro[0]) {
                        cout << "[ERRO] MACRO NA LINHA: " << i+l_offset << endl;
                        cout << "       " << MNT[k][0] << " JA DEFINIDA ANTERIORMENTE NA LINHA " << stoi(MNT[k][3])+l_offset << endl;
                        return;
                    }
                }
                MNT.push_back(macro);
                for(j = i+1; j < tokens.size(); j++) {
                    for(int k = 0; k < tokens[j].size(); k++) {
                        if(tokens[j][k] == "ENDMACRO") {
                            if(k != 0 || tokens[j].size() > 1) {
                                cout << "[ERRO] MACRO NA LINHA: " << j+l_offset << endl;
                                cout << "       ENDMACRO DEVE ESTAR SOZINHO NA LINHA" << endl;
                                return;
                            }
                            break;
                        }
                        else if(tokens[j][k] == "MACRO") {
                            cout << "[ERRO] MACRO NA LINHA: " << i+l_offset << endl;
                            cout << "       " << MNT.back()[0] << " SEM FINAL DEFINIDO" << endl;
                            return;
                        }
                    }
                    if(tokens[j][0] == "ENDMACRO") {
                        MDT.push_back(tokens[j]);
                        MNT.back().push_back(to_string(i));
                        MNT.back().push_back(to_string(j));
                        /*for(int k = j; k >= i; k--) {
                            tokens.erase(tokens.begin()+k);
                        }
                        i--;*/
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
                    if(j == tokens.size()-1) {
                        cout << "[ERRO] MACRO NA LINHA: " << i+l_offset << endl;
                        cout << "       " << MNT.back()[0] << " SEM FINAL DEFINIDO" << endl;
                        return;
                    }
                }

                for(j = 2+stoi(MNT.back()[1]); j > 2; j--) {
                    MNT.back().erase(MNT.back().begin()+j);
                }

                break;
            }
        }
    }

    //STOP MACRO RECURSION
    int rec = meta_macro(MNT,MDT);
    if(rec >= 0) {
        cout << "[ERRO] MACRO NA LINHA: " << stoi(MNT[rec][3])+l_offset << endl;
        cout << "       " << MNT[rec][0] << " SE INVOCA RECURSIVAMENTE" << endl;
        return;
    }

    //DELETE MACRO LINES
    for(int i = MNT.size()-1; i >= 0; i--) {
        for(int j = stoi(MNT[i][4]); j >= stoi(MNT[i][3]); j--) {
            tokens.erase(tokens.begin()+j);
            l_offset++;
        }
        MNT[i].pop_back();
        MNT[i].pop_back();
    }

    //REPLACE MACRO CALLS
    for(int i = 0; i < tokens.size(); i++) {
        for(int j = 0; j < tokens[i].size(); j++) {
            for(int k = 0; k < MNT.size(); k++) {
                if(tokens[i][j] == MNT[k][0]) {
                    vector<string> args;
                    for(int m = 0; j+1+m < tokens[i].size(); m++) {
                        if(tokens[i][j+1+m] != ",") {
                            args.push_back(tokens[i][j+1+m]);
                        }
                    }
                    if(args.size() != stoi(MNT[k][1])) {
                        cout << "[ERRO] MACRO LINHA " << i+l_offset << endl;
                        cout << "       ESPERA-SE " << MNT[k][1] << " ARGUMENTOS"<< endl;
                        cout << "       FORAM PASSADOS " << args.size() << " ARGUMENTOS" << endl;
                        return;
                    }
                    if(j == 0) {
                        tokens.erase(tokens.begin()+i);
                    }
                    else {
                        while(tokens[i].size() > j) {
                            tokens[i].pop_back();
                        }
                        i++;
                    }
                    for(int l = stoi(MNT[k][2]); l < MDT.size(); l++) {
                        if(MDT[l][0] == "ENDMACRO") {
                            break;
                        }
                        int it = i+l-stoi(MNT[k][2]);
                        tokens.insert(tokens.begin()+it, MDT[l]);
                        for(int m = 0; m < tokens[it].size(); m++) {
                            if(tokens[it][m][0] == '#') {
                                tokens[it][m].erase(tokens[it][m].begin());
                                tokens[it][m] = args[stoi(tokens[it][m])-1];
                            }
                        }
                    }
                    i--;
                }
            }
        }
    }

    writer(tokens, name, ".MCR");
}

void object(vector<vector<string>> tokens, string name)
{
    vector<pair<string,string>> linetable;
    vector<pair<int,int>> expr;
    vector<vector<string>> ST;
    vector<string> code;
    int memcounter = 0;
    string section = "NONE";
    string stopped = "ONGOING";
    for(int i = 0; i < tokens.size(); i++) {
        linetable = syntax_scanner(tokens[i], i+1);
        if(linetable.empty()) {
            return;
        }
        else {
            for(int j = 0; j < linetable.size(); j++) {
                if(linetable[j].second == "TEXT") {
                    if(section == "TEXT") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       SEÇÃO TEXT DECLARADA ANTERIORMENTE" << endl;
                        return;
                    }
                    else if(section == "DATA") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       SEÇÃO DATA DECLARADA ANTERIORMENTE" << endl;
                        return;
                    }
                    else {
                        section = "TEXT";
                    }
                }
                else if(linetable[j].second == "DATA") {
                    if(section == "TEXT") {
                        if(stopped == "ONGOING") {
                            cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                            cout << "       INSTRUÇÃO STOP NÃO ENCONTRADA" << endl;
                            cout << "       CÓDIGO SEM PARADA" << endl;
                            return;
                        }
                        section = "DATA";
                    }
                    else if(section == "DATA") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       SEÇÃO DATA DECLARADA ANTERIORMENTE" << endl;
                        return;
                    }
                    else {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       SEÇÃO TEXT NAO CHEGOU A SER DECLARADA" << endl;
                        return;
                    }
                }
                else if(linetable[j].second == "LABEL") {
                    if(section == "NONE") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE LABEL FORA DE QUALQUER SEÇÃO" << endl;
                        return;
                    }

                    string symbol = linetable[j].first;
                    symbol.pop_back();
                    int pos = -1;
                    for(int k = 0; k < ST.size(); k++) {
                        if(ST[k][0] == symbol) {
                            pos = k;
                        }
                    }
                    if(pos == -1) {
                        vector<string> s;
                        s.push_back(symbol);
                        s.push_back(to_string(memcounter));
                        s.push_back("-1");
                        s.push_back("-1");
                        ST.push_back(s);
                    }
                    else if(ST[pos][1] != "-1") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       " << symbol << " DECLARADO MAIS DE UMA VEZ" << endl;
                        return;
                    }
                    else {
                        ST[pos][1] = to_string(memcounter);
                        while(ST[pos][2] != "-1") {
                            string ref = code[stoi(ST[pos][2])];
                            code[stoi(ST[pos][2])] = ST[pos][1];
                            ST[pos][2] = ref;
                        }
                    }
                }
                else if(linetable[j].second == "INSTRUCTION1") {
                    if(section == "NONE") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE INSTRUÇÃO FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }
                    else if(section == "DATA") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE INSTRUÇÃO FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }

                    stopped = "STOPPED";
                    code.push_back("14");
                    memcounter++;
                }
                else if(linetable[j].second == "INSTRUCTION2") {
                    if(section == "NONE") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE INSTRUÇÃO FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }
                    else if(section == "DATA") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE INSTRUÇÃO FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }

                    if(linetable[j].first == "ADD") {
                        code.push_back("01");
                    }
                    else if(linetable[j].first == "SUB") {
                        code.push_back("02");
                    }
                    else if(linetable[j].first == "MUL") {
                        code.push_back("03");
                    }
                    else if(linetable[j].first == "DIV") {
                        code.push_back("04");
                    }
                    else if(linetable[j].first == "JMP") {
                        code.push_back("05");
                    }
                    else if(linetable[j].first == "JMPN") {
                        code.push_back("06");
                    }
                    else if(linetable[j].first == "JMPP") {
                        code.push_back("07");
                    }
                    else if(linetable[j].first == "JMPZ") {
                        code.push_back("08");
                    }
                    else if(linetable[j].first == "LOAD") {
                        code.push_back("10");
                    }
                    else if(linetable[j].first == "STORE") {
                        code.push_back("11");
                    }
                    else if(linetable[j].first == "INPUT") {
                        code.push_back("12");
                    }
                    else if(linetable[j].first == "OUTPUT") {
                        code.push_back("13");
                    }
                    memcounter++;
                }
                else if(linetable[j].second == "INSTRUCTION3") {
                    if(section == "NONE") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE INSTRUÇÃO FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }
                    else if(section == "DATA") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE INSTRUÇÃO FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }

                    code.push_back("09");
                    memcounter++;
                }
                else if(linetable[j].second == "REFERENCE") {
                    if(section == "NONE") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE REFERENCIA FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }
                    else if(section == "DATA") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE REFERENCIA FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }

                    int pos = -1;
                    for(int k = 0; k < ST.size(); k++) {
                        if(ST[k][0] == linetable[j].first) {
                            pos = k;
                        }
                    }
                    if(pos == -1) {
                        vector<string> s;
                        s.push_back(linetable[j].first);
                        s.push_back("-1");
                        s.push_back(to_string(memcounter));
                        s.push_back(to_string(i+1));
                        ST.push_back(s);
                        code.push_back(ST.back()[1]);
                        memcounter++;
                    }
                    else if(ST[pos][1] != "-1") {
                        code.push_back(ST[pos][1]);
                        memcounter++;
                    }
                    else {
                        code.push_back(ST[pos][2]);
                        ST[pos][2] = to_string(memcounter);
                        memcounter++;
                    }
                }
                else if(linetable[j].second == "EXPRESSION") {
                    if(section == "NONE") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE REFERENCIA FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }
                    else if(section == "DATA") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE REFERENCIA FORA DA SEÇÃO TEXT" << endl;
                        return;
                    }

                    int pos = linetable[j].first.find_first_not_of(c_lex+n_lex+u_lex);
                    int val = stoi(linetable[j].first.substr(pos,linetable[j].first.size()-pos));
                    expr.push_back(make_pair(memcounter,val));
                    string ref = linetable[j].first.substr(0,pos);
                    linetable[j].first = ref;
                    linetable[j].second = "REFERENCE";
                    j--;
                }
                else if(linetable[j].second == "SPACE") {
                    if(section == "NONE") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE DIRETIVA DE MEMORIA FORA DA SEÇÃO DATA" << endl;
                        return;
                    }
                    else if(section == "TEXT") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE DIRETIVA DE MEMORIA FORA DA SEÇÃO DATA" << endl;
                        return;
                    }

                    int n = 1;
                    if(linetable.size()-j > 1) {
                        if(linetable[j+1].first.find("0X") == 0) {
                            n = stoi(linetable[j+1].first,nullptr,16);
                        }
                        else {
                            n = stoi(linetable[j+1].first);
                        }
                    }
                    else {
                        n = 1;
                    }
                    for(int k = 0; k < n; k++) {
                        code.push_back("00");
                        memcounter++;
                    }
                }
                else if(linetable[j].second == "CONST") {
                    if(section == "NONE") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE DIRETIVA DE MEMORIA FORA DA SEÇÃO DATA" << endl;
                        return;
                    }
                    else if(section == "TEXT") {
                        cout << "[ERRO] SEMÂNTICO NA LINHA: " << i+1 << endl;
                        cout << "       DECLARAÇÃO DE DIRETIVA DE MEMORIA FORA DA SEÇÃO DATA" << endl;
                        return;
                    }

                    int n;
                    if(linetable[j+1].first.find("0X") == 0) {
                        n = stoi(linetable[j+1].first,nullptr,16);
                    }
                    else {
                        n = stoi(linetable[j+1].first);
                    }
                    code.push_back(to_string(n));
                    if(n > 0 && n < 10) {
                        code.back().insert(code.back().begin(),'0');
                    }
                    memcounter++;
                }
            }
        }
    }

    for(int i = 0; i < ST.size(); i++) {
        if(ST[i][1] == "-1") {
            cout << "[ERRO] SEMÂNTICO NA LINHA: " << stoi(ST[i][3]) << endl;
            cout << "       " << ST[i][0] << " NÃO DEFINIDO" << endl;
            return;
        }
    }

    //OFFSET REFERENCES WITH STORED EXPRESSIONS
    for(int i = 0; i < expr.size(); i++) {
        code[expr[i].first] = to_string(stoi(code[expr[i].first])+expr[i].second);
    }

    for(int i = 0; i < code.size(); i++) {
        if(code[i].find("0X") == 0) {
            int dec = stoi(code[i],nullptr,16);
            code[i] = to_string(dec);
            if(dec > 0 && dec < 10) {
                code[i].insert(code[i].begin(),'0');
            }
        }
    }

    vector<vector<string>> object;
    object.push_back(code);
    writer(object, name, ".OBJ");
}

int main(int argc, char* argv[])
{
    if(argc == 3) {
        if(string(argv[1]) == "-p") {
            vector<vector<string>> tokens = tokenizer(argv[2], ".ASM");
            if(!tokens.empty()) {
                pre_processing(tokens, argv[2]);
            }
            else {
                cout << "[ERRO] ARQUIVO FONTE " << argv[2] << ".asm VAZIO" << endl;
            }
        }
        else if(string(argv[1]) == "-m") {
            vector<vector<string>> tokens = tokenizer(argv[2], ".PRE");
            if(!tokens.empty()) {
                macros(tokens, argv[2]);
            }
            else {
                cout << "[ERRO] ARQUIVO FONTE " << argv[2] << ".PRE VAZIO" << endl;
            }
        }
        else if(string(argv[1]) == "-o") {
            vector<vector<string>> tokens = tokenizer(argv[2], ".MCR");
            if(!tokens.empty()) {
                object(tokens, argv[2]);
            }
            else {
                cout << "[ERRO] ARQUIVO FONTE " << argv[2] << ".MCR VAZIO" << endl;
            }
        }
        else {
            cout << "[ERRO] OPÇÃO " << argv[1] << " DESCONHECIDA" << endl;
            cout << "       TENTE EXECUTAR O COMANDO NO FORMATO: montador -<op> <arquivo>" << endl;
            cout << "       PARA MAIS INFORMAÇÕES DIGITE: montador -h" << endl;
        }
    }
    else if (argc > 3) {
        cout << "[ERRO] MUITOS ARGUMENTOS" << endl;
        cout << "       TENTE EXECUTAR O COMANDO NO FORMATO: montador -<op> <arquivo>" << endl;
        cout << "       PARA MAIS INFORMAÇÕES DIGITE: montador -h" << endl;
    }
    else { // argc < 3
        if(argc == 2) {
            if(string(argv[1]) == "-h") {
                cout << "Uso: montador -<op> <arquivo>" << endl;
                cout << "Opções:" << endl;
                cout << "    -p          Gera um arquivo .PRE à partir de um arquivo .asm," << endl;
                cout << "                realizando o pre-processamento do código." << endl;
                cout << "    -m          Gera um arquivo .MCR à partir de um arquivo .PRE," << endl;
                cout << "                realizando a substituição de macros no código." << endl;
                cout << "    -o          Gera um arquivo .OBJ à partir de um arquivo .MCR," << endl;
                cout << "                montando o código de máquina final após testar por" << endl;
                cout << "                erros léxicos, sintáticos e semânticos no código." << endl;
                cout << "    -h          Exibe especificações das opções de linha de comando." << endl;
            }
            else {
                cout << "[ERRO] FALTAM ARGUMENTOS" << endl;
                cout << "       TENTE EXECUTAR O COMANDO NO FORMATO: montador -<op> <arquivo>" << endl;
                cout << "       PARA MAIS INFORMAÇÕES DIGITE: montador -h" << endl;
            }
        }
        else {
            cout << "[ERRO] FALTAM ARGUMENTOS" << endl;
            cout << "       TENTE EXECUTAR O COMANDO NO FORMATO: montador -<op> <arquivo>" << endl;
            cout << "       PARA MAIS INFORMAÇÕES DIGITE: montador -h" << endl;
        }
    }

    return 0;
}
