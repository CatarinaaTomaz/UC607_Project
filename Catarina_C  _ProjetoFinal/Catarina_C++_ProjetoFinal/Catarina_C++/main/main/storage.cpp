#define _CRT_SECURE_NO_WARNINGS // Adicionado para desativar avisos de insegurança do Visual Studio (C4996)

#include "storage.h"
#include "employee.h"
#include <fstream>
#include <iostream>
#include <iomanip> // NECESSÁRIO para usar setw() e put_time
#include <cctype> 
#include <algorithm>
#include <limits> 
#include <ctime>   // NECESSÁRIO para time(), localtime(), e tm
#include <sstream> 

using namespace std;

// Esta função deve estar definida em main.cpp (ou utilitarios.cpp) e declarada em storage.h
// Se estiver a dar erro, verifique se a função get_int está implementada e acessível.
int get_int(const string& prompt);

// Esta função deve estar definida e acessível (e.g., em utilitarios.cpp ou main.cpp)
extern bool leapYear(int);


// Implementação da Cifragem Progressiva de César
string progressiveCipher(const string& s, int shiftBase) {
    string out = s;
    for (size_t i = 0; i < out.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(out[i]);
        if (isalpha(ch)) {
            bool up = isupper(ch);
            char base = up ? 'A' : 'a';
            int desloc = shiftBase + static_cast<int>(i);
            // O + 26 * 100 garante que o resultado é positivo antes do módulo
            out[i] = static_cast<char>(base + ((ch - base + desloc + 26 * 100) % 26));
        }
    }
    return out;
}

// Implementação de save_all (Grava todos os colaboradores no ficheiro)
void save_all(const string& path, const vector<Employee>& list) {
    ofstream ofs(path);
    if (!ofs) {
        cerr << "Erro: Nao foi possivel abrir o ficheiro para escrita.\n";
        return;
    }

    for (const auto& e : list) {
        // Campos Simples: ID, Departamento, Férias Restantes
        ofs << "ID|" << e.id << "\n";
        ofs << "DEP|" << e.department << "\n";
        ofs << "VRD|" << e.vacationDaysRemaining << "\n";

        // Nome (Cifrado)
        ofs << progressiveCipher(e.fullName, 1) << "\n";

        // Marcações (YYYY-MM-DD|TIPO)
        for (const auto& p : e.marks)
            ofs << p.first << "|" << p.second << "\n";

        // Formações: FMT|CURSO;DATA_CONCLUSAO
        for (const auto& f : e.formacoes)
            ofs << "FMT|" << f.nome_curso << ";" << f.data_conclusao << "\n";

        // Notas: NOT|TEXTO;DATA
        for (const auto& n : e.notas)
            ofs << "NOT|" << n.texto << ";" << n.data << "\n";

        // Separador de colaboradores
        ofs << "***\n";
    }
}

// Implementação de load_all (Carrega todos os colaboradores do ficheiro)
bool load_all(const string& path, vector<Employee>& list) {
    ifstream ifs(path);
    if (!ifs) return false;

    string line;
    Employee cur;
    bool hasName = false;

    while (getline(ifs, line)) {
        if (line == "***") {
            if (hasName) list.push_back(cur);
            cur = Employee{};
            hasName = false;
        }
        else if (line.find("ID|") == 0) { // ID
            try { cur.id = stoi(line.substr(3)); }
            catch (...) {}
        }
        else if (line.find("DEP|") == 0) { // Departamento
            try { cur.department = line.substr(4); }
            catch (...) {}
        }
        else if (line.find("VRD|") == 0) { // Dias de Férias Restantes
            try { cur.vacationDaysRemaining = stoi(line.substr(4)); }
            catch (...) {}
        }
        else if (line.find("FMT|") == 0) { // Formação
            string data = line.substr(4);
            size_t p = data.find(';');
            if (p != string::npos) {
                Formacao f;
                f.nome_curso = data.substr(0, p);
                f.data_conclusao = data.substr(p + 1);
                cur.formacoes.push_back(f);
            }
        }
        else if (line.find("NOT|") == 0) { // Nota
            string data = line.substr(4);
            size_t p = data.find(';');
            if (p != string::npos) {
                Nota n;
                n.texto = data.substr(0, p);
                n.data = data.substr(p + 1);
                cur.notas.push_back(n);
            }
        }
        else if (line.find('|') == string::npos && !line.empty()) { // Nome cifrado (assume que é a linha sem '|')
            cur.fullName = progressiveCipher(line, -1); // Descriptografa com shiftBase negativo
            hasName = true;
        }
        else if (line.find('|') != string::npos) { // Marcação (YYYY-MM-DD|X)
            size_t p = line.find('|');
            string d = line.substr(0, p);
            char t = line[p + 1];
            cur.marks[d] = t;
        }
    }

    if (hasName) list.push_back(cur);
    return true;
}

// Implementação de generate_next_id
int generate_next_id(const vector<Employee>& list) {
    if (list.empty()) return 1;
    int max_id = 0;
    for (const auto& e : list) {
        if (e.id > max_id) max_id = e.id;
    }
    return max_id + 1;
}

// Implementação de find_employee_by_id_or_name (Para modificação)
Employee* find_employee_by_id_or_name(std::vector<Employee>& team) {
    cout << "Buscar por [I]D ou [N]ome? ";
    char choice;

    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida.\n"; return nullptr;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    choice = static_cast<char>(std::toupper(static_cast<unsigned char>(choice)));

    if (choice == 'I') {
        int id;
        cout << "Digite o ID: ";
        if (!(cin >> id)) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "ID invalido.\n"; return nullptr;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        auto it = find_if(team.begin(), team.end(), [&](const Employee& e) { return e.id == id; });
        if (it == team.end()) {
            cout << "Colaborador com ID " << id << " nao encontrado.\n";
            return nullptr;
        }
        return &(*it);
    }
    else if (choice == 'N') {
        string name;
        cout << "Digite o Nome: ";
        getline(cin, name);
        // Usa a busca exata pelo nome completo
        auto it = find_if(team.begin(), team.end(), [&](const Employee& e) { return e.fullName == name; });
        if (it == team.end()) {
            cout << "Colaborador com nome '" << name << "' nao encontrado.\n";
            return nullptr;
        }
        return &(*it);
    }
    else {
        cout << "Opcao invalida. Por favor, digite apenas 'I' ou 'N'.\n";
        return nullptr;
    }
}

// Implementação de find_employee_by_id_or_name_const (Para visualização/exportação)
const Employee* find_employee_by_id_or_name_const(const std::vector<Employee>& team) {
    cout << "Buscar por [I]D ou [N]ome? ";
    char choice;

    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida.\n"; return nullptr;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    choice = static_cast<char>(std::toupper(static_cast<unsigned char>(choice)));

    if (choice == 'I') {
        int id;
        cout << "Digite o ID: ";
        if (!(cin >> id)) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "ID invalido.\n"; return nullptr;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        auto it = find_if(team.begin(), team.end(), [&](const Employee& e) { return e.id == id; });
        if (it == team.end()) {
            cout << "Colaborador com ID " << id << " nao encontrado.\n";
            return nullptr;
        }
        return &(*it);
    }
    else if (choice == 'N') {
        string name;
        cout << "Digite o Nome: ";
        getline(cin, name);
        auto it = find_if(team.begin(), team.end(), [&](const Employee& e) { return e.fullName == name; });
        if (it == team.end()) {
            cout << "Colaborador com nome '" << name << "' nao encontrado.\n";
            return nullptr;
        }
        return &(*it);
    }
    else {
        cout << "Opcao invalida. Por favor, digite apenas 'I' ou 'N'.\n";
        return nullptr;
    }
}


// Implementação de export_employee_calendar (TXT)
void export_employee_calendar(const Employee& e) {
    // Cria um nome de ficheiro com base no nome e ID do colaborador
    string filename = e.fullName + "_" + to_string(e.id) + "_calendario.txt";
    // Substitui espaços por underscores no nome do ficheiro para melhor compatibilidade
    replace(filename.begin(), filename.end(), ' ', '_');

    ofstream ofs(filename);
    if (!ofs) {
        cerr << "Erro: Nao foi possivel abrir o ficheiro de exportacao '" << filename << "'.\n";
        return;
    }

    // Código para obter a hora atual de forma compatível
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);

    // Título do Relatório
    ofs << "RELATORIO DE CALENDARIO DO COLABORADOR\n";
    ofs << string(50, '-') << "\n";
    ofs << "Nome: " << e.fullName << "\n";
    ofs << "ID: " << e.id << "\n";
    ofs << "Departamento: " << e.department << "\n";
    ofs << string(50, '-') << "\n";

    // Cabeçalho das Marcações
    ofs << left << setw(12) << "DATA" << " | " << "TIPO" << "\n";
    ofs << string(12, '-') << "---" << string(4, '-') << "\n";

    // Exporta cada marcação
    for (const auto& mark : e.marks) {
        string tipo;
        if (mark.second == 'F') tipo = "Férias";
        else if (mark.second == 'X') tipo = "Falta";
        else tipo = "Outro (" + string(1, mark.second) + ")";

        ofs << left << setw(12) << mark.first << " | " << tipo << "\n";
    }

    ofs << string(50, '-') << "\n";
    ofs << "Nota: Férias Restantes: " << e.vacationDaysRemaining << "\n";
    // Usa put_time para formatar a hora
    ofs << "Ficheiro gerado a " << put_time(ltm, "%Y-%m-%d %H:%M:%S") << "\n";

    cout << "Calendario de " << e.fullName << " exportado com sucesso para '" << filename << "'.\n";
}

// Implementação de export_department_report (TXT)
void export_department_report(const std::vector<Employee>& team) {
    string dept_name;
    cout << "Nome do Departamento a exportar (deixe em branco para TODOS): ";
    getline(cin, dept_name);
    // Remove espaços em branco antes e depois do nome do departamento
    size_t start = dept_name.find_first_not_of(" \t\n\r");
    if (string::npos == start) dept_name = "";
    else dept_name = dept_name.substr(start);
    size_t end = dept_name.find_last_not_of(" \t\n\r");
    if (string::npos != end) dept_name = dept_name.substr(0, end + 1);

    if (dept_name.empty()) {
        cout << "A exportar relatorio de TODOS os departamentos.\n";
    }

    // Filtra colaboradores
    vector<const Employee*> filtered_team;
    string filter_dept = dept_name;
    transform(filter_dept.begin(), filter_dept.end(), filter_dept.begin(), ::tolower);

    for (const auto& e : team) {
        // Compara ignorando maiúsculas/minúsculas para ser mais user-friendly
        string e_dept = e.department;
        transform(e_dept.begin(), e_dept.end(), e_dept.begin(), ::tolower);

        if (dept_name.empty() || e_dept == filter_dept) {
            filtered_team.push_back(&e);
        }
    }

    if (filtered_team.empty()) {
        cout << "Nenhum colaborador encontrado para o departamento '" << (dept_name.empty() ? "TODOS" : dept_name) << "'.\n";
        return;
    }

    string filename = (dept_name.empty() ? "geral" : dept_name) + "_relatorio_ausencias.txt";
    // Substitui espaços por underscores no nome do ficheiro
    replace(filename.begin(), filename.end(), ' ', '_');

    ofstream ofs(filename);
    if (!ofs) {
        cerr << "Erro: Nao foi possivel abrir o ficheiro de exportacao '" << filename << "'.\n";
        return;
    }

    // Código para obter a hora atual de forma compatível
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);

    // Título do Relatório
    ofs << "RELATORIO DE AUSENCIAS DO DEPARTAMENTO\n";
    ofs << string(90, '=') << "\n";
    ofs << "Departamento(s) Incluidos: " << (dept_name.empty() ? "TODOS" : dept_name) << "\n";
    // Usa put_time para formatar a hora
    ofs << "Ficheiro gerado a " << put_time(ltm, "%Y-%m-%d %H:%M:%S") << "\n";
    ofs << string(90, '=') << "\n";

    // Cabeçalho TXT (usando setw para colunas)
    ofs << left << setw(30) << "NOME"
        << setw(20) << "DEPARTAMENTO"
        << right << setw(10) << "FÉRIAS (F)"
        << setw(10) << "FALTAS (X)"
        << setw(10) << "DIAS VRD" << "\n";
    ofs << string(90, '-') << "\n";

    for (const auto* e_ptr : filtered_team) {
        const Employee& e = *e_ptr;
        int total_ferias_usadas = 0;
        int total_faltas = 0;

        for (const auto& mark : e.marks) {
            if (mark.second == 'F') total_ferias_usadas++;
            if (mark.second == 'X') total_faltas++;
        }

        ofs << left << setw(30) << e.fullName
            << setw(20) << e.department
            << right << setw(10) << total_ferias_usadas
            << setw(10) << total_faltas
            << setw(10) << e.vacationDaysRemaining << "\n";
    }

    ofs << string(90, '=') << "\n";

    cout << "Relatorio de departamento(s) exportado com sucesso para '" << filename << "'.\n";
}

// Implementação de Menu e Lógica de Exportação
void export_data(const std::vector<Employee>& team) {
    if (team.empty()) {
        cout << "Nao ha colaboradores para exportar.\n";
        return;
    }

    // Usa get_int (definida externamente) para ler a opção
    int opc = get_int("\n--- EXPORTACAO DE DADOS ---\n[1] Exportar Calendario de Colaborador Unico (TXT)\n[2] Exportar Relatorio de Departamento (TXT)\n[0] Voltar\nEscolha uma opcao: ");

    if (opc == 1) {
        // Encontra o colaborador (versão const, pois é só para leitura)
        const Employee* emp = find_employee_by_id_or_name_const(team);
        if (emp) {
            export_employee_calendar(*emp);
        }
    }
    else if (opc == 2) {
        // Exporta o relatório 
        export_department_report(team);
    }
    else if (opc != 0) {
        cout << "Opcao invalida.\n";
    }
}


// Implementação de show_month
void show_month(const Employee& e, int month, int year) {
    // Verifica se month e year são válidos antes de prosseguir
    if (month < 1 || month > 12 || year < 1900 || year > 2100) {
        cout << "Mes ou ano invalido.\n";
        return;
    }

    cout << "\n[" << e.fullName << " - Dept: " << e.department << "] " << month << "/" << year << "\n";
    // Colunas ajustadas: Do Se Te Qu Qu Se Sa (4 caracteres cada)
    cout << " Do Se Te Qu Qu Se Sa\n";

    // Preparar a estrutura tm para o primeiro dia do mês/ano
    tm t_struct{}; // Inicializa a zero
    t_struct.tm_mday = 1;
    t_struct.tm_mon = month - 1; // tm_mon é baseado em 0 (Jan=0)
    t_struct.tm_year = year - 1900; // tm_year é o ano desde 1900

    // Calcula o dia da semana e o número de dias no mês
    mktime(&t_struct);
    int start = t_struct.tm_wday; // Dia da semana do 1º dia (0=Dom, 6=Sáb)

    // Dias em cada mês (Assume que a função 'leapYear' está definida e acessível)
    int mdays[] = { 31, (leapYear(year) ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int days = mdays[month - 1];

    int col = 0;

    // Imprimir espaços até ao primeiro dia
    for (int i = 0; i < start; i++) {
        cout << "    "; // 4 espaços para alinhar com o formato de data (XXT )
        col++;
    }

    // Imprimir dias do mes com marcações
    for (int d = 1; d <= days; ++d) {

        // Chave YYYY-MM-DD para busca no map
        stringstream ss;
        ss << year << "-" << setfill('0') << setw(2) << month << "-" << setw(2) << d;
        string key = ss.str();

        char mark = ' ';
        auto it = e.marks.find(key);
        if (it != e.marks.end()) mark = it->second;

        // --- Verificação de Fim de Semana ---
        tm t_day{}; // Re-inicializa a zero para o dia atual
        t_day.tm_mday = d;
        t_day.tm_mon = month - 1;
        t_day.tm_year = year - 1900;
        mktime(&t_day); // Recalcula o tm_wday

        // Verifica se é fim de semana (Domingo=0, Sábado=6)
        bool wk = (t_day.tm_wday == 0 || t_day.tm_wday == 6);

        char display_char = ' ';

        if (mark != ' ') {
            // Prioridade: Marcações (Férias/Faltas)
            display_char = mark;
        }
        else if (wk) {
            // Se não houver marcação, marca com * se for Fim de Semana
            display_char = '*';
        }

        // Esta é a linha de impressão correta e limpa:
        cout << setfill(' ') << setw(2) << d << display_char << " ";

        // --- Controle de Linha ---

        col++;
        if (col % 7 == 0) cout << "\n";
    }

    // Se o mês não terminar num sábado, adiciona uma quebra de linha
    if (col % 7 != 0) cout << "\n";

    cout << "\nNota: * = Fim de Semana, F = Ferias, X = Falta\n";
}