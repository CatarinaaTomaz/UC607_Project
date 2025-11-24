#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <map>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cctype>
#include "employee.h"
#include "storage.h"

using namespace std;

// Funções Auxiliares (Para CRUD e Input Seguro) -----------------------------------------------------------

void get_line(string& s, const string& prompt) {
    cout << prompt;
    // Limpar o buffer antes de ler a linha completa (evita problemas após cin >> int)
    if (cin.peek() == '\n') cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, s);
}

int get_int(const string& prompt) {
    int val;
    cout << prompt;
    if (!(cin >> val)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return -1;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return val;
}

// Funções do Menu (Lógica de Negócio) ---------------------------------------------------------------------

map<char, int> calculate_absences_monthly(const Employee& e, int month, int year) {
    map<char, int> counts = { {'F', 0}, {'X', 0} };
    string month_str = (month < 10 ? "0" : "") + to_string(month);
    string year_str = to_string(year);

    for (const auto& pair : e.marks) {
        if (pair.first.substr(0, 7) == year_str + "-" + month_str) {
            if (pair.second == 'F' || pair.second == 'X') {
                counts[pair.second]++;
            }
        }
    }
    return counts;
}

// Requisito 2 & 4: Adiciona Colaborador (Com ID e Departamento)
void add_employee(vector<Employee>& team) {
    Employee e;
    cout << "Nome do colaborador: ";
    getline(cin, e.fullName);

    // 1. Verificar Duplicado
    auto it = find_if(team.begin(), team.end(),
        [&](const Employee& x) { return x.fullName == e.fullName; });

    if (it != team.end()) {
        cout << "Ja existe esse nome. Substituir? (s/n): ";
        char r; cin >> r; cin.ignore();
        if (r == 's' || r == 'S') {
            e.id = it->id;
            e.marks = it->marks;
            e.formacoes = it->formacoes;
            e.notas = it->notas;
            cout << "Departamento atual: " << it->department << endl;
            cout << "Novo Departamento: ";
            getline(cin, e.department);
            *it = e;
            cout << "Colaborador " << e.fullName << " (ID: " << e.id << ") atualizado com sucesso!\n";
        }
        return;
    }

    // 2. Novo Colaborador
    cout << "Departamento: ";
    getline(cin, e.department);
    e.id = generate_next_id(team);
    team.push_back(e);
    cout << "Colaborador " << e.fullName << " (ID: " << e.id << ") adicionado com sucesso!\n";
}

// Requisito 4: Marcar Férias/Faltas (Com Aviso de Conflito)
void mark_absence(vector<Employee>& team, char type) {
    Employee* emp = find_employee_by_id_or_name(team);
    if (!emp) return;

    // pedir data
    int d, m, y;
    cout << "Dia Mes Ano (ex: 12 5 2025): ";
    if (!(cin >> d >> m >> y)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida\n"; return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!date_ok(d, m, y)) { cout << "Data invalida.\n"; return; }
    if (weekend(d, m, y)) { cout << "Nao e permitido marcar ao fim de semana.\n"; return; }

    string key = to_string(y) + "-" + (m < 10 ? "0" : "") + to_string(m)
        + "-" + (d < 10 ? "0" : "") + to_string(d);

    // Requisito 4: Aviso de Conflito de Férias (APENAS para FÉRIAS 'F')
    if (type == 'F') {
        if (emp->vacationDaysRemaining <= 0 && emp->marks.find(key) == emp->marks.end()) {
            cout << "ERRO: " << emp->fullName << " nao tem dias de ferias restantes.\n";
            return;
        }

        for (const auto& other_emp : team) {
            if (other_emp.id != emp->id && other_emp.department == emp->department) {
                auto mark_it = other_emp.marks.find(key);
                if (mark_it != other_emp.marks.end() && mark_it->second == 'F') {
                    cout << "AVISO DE CONFLITO: O colega " << other_emp.fullName
                        << " do seu departamento ja tem ferias marcadas para " << key << ".\n";
                    break;
                }
            }
        }
    }

    // Se for uma nova marcação de Férias, decrementa o contador
    if (type == 'F' && emp->marks.find(key) == emp->marks.end()) {
        emp->vacationDaysRemaining--;
    }

    emp->marks[key] = type;
    cout << "Marcado '" << type << "' no dia " << key << ".\n";
}

// Requisito 2, 4: Visualizar Calendário (Opcional: mês atual)
void view_calendar(vector<Employee>& team) {
    Employee* emp = find_employee_by_id_or_name(team);
    if (!emp) return;

    int m, y;
    time_t now = time(0);
    tm t_struct;

    // Usa localtime_s para segurança (para evitar C4996)
    if (localtime_s(&t_struct, &now) != 0) {
        cerr << "Erro ao obter tempo local.\n";
        return;
    }
    tm* ltm = &t_struct;

    cout << "Indique mes e ano (ex: 5 2025). Deixe em branco para mes atual (" << (1 + ltm->tm_mon) << " " << (1900 + ltm->tm_year) << "): ";
    string input;
    getline(cin, input);

    if (input.empty()) {
        m = 1 + ltm->tm_mon;
        y = 1900 + ltm->tm_year;
    }
    else {
        stringstream ss(input);
        if (!(ss >> m >> y)) {
            cout << "Entrada invalida. A usar mes atual.\n";
            m = 1 + ltm->tm_mon;
            y = 1900 + ltm->tm_year;
        }
    }

    show_month(*emp, m, y);
}

// Requisito 1: Relatórios Mensais
void monthly_report(const vector<Employee>& team) {
    int m, y;
    cout << "Indique o Mes e Ano do Relatorio (ex: 5 2025): ";
    if (!(cin >> m >> y)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida.\n"; return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\n=== Relatorio Mensal " << m << "/" << y << " ===\n";
    cout << setw(30) << left << "Colaborador" << " | " << "Ferias (F)" << " | " << "Faltas (X)" << endl;
    cout << string(52, '-') << endl;

    for (const auto& e : team) {
        map<char, int> counts = calculate_absences_monthly(e, m, y);
        cout << setw(30) << left << e.fullName << " | "
            << setw(10) << right << counts['F'] << " | "
            << setw(9) << right << counts['X'] << endl;
    }
}

// Requisito 3: Estatísticas de Departamento
void department_stats(const vector<Employee>& team) {
    map<string, int> total_absences;
    string busiest_dept = "";
    int max_absences = -1;

    for (const auto& e : team) {
        for (const auto& mark : e.marks) {
            if (mark.second == 'F' || mark.second == 'X') {
                total_absences[e.department]++;
            }
        }
    }

    cout << "\n=== Estatisticas de Ausencia por Departamento (Ano Todo) ===\n";
    for (const auto& pair : total_absences) {
        cout << setw(20) << left << pair.first << ": " << pair.second << " dias de ausencia.\n";
        if (pair.second > max_absences) {
            max_absences = pair.second;
            busiest_dept = pair.first;
        }
    }

    if (!busiest_dept.empty()) {
        cout << "\nDepartamento com mais ausencias: **" << busiest_dept << "** (" << max_absences << " dias)\n";
    }
}

// Requisito 5: Dashboard Resumido
void show_dashboard(const vector<Employee>& team) {
    cout << "\n=== Dashboard RH Resumido ===\n";
    cout << "Total de ferias anuais: 22 dias.\n\n";

    cout << setw(30) << left << "Nome" << " | "
        << setw(10) << "Ferias Usadas" << " | "
        << setw(10) << "Faltas (Ano)" << " | "
        << "Dias Restantes" << endl;
    cout << string(70, '-') << endl;

    for (const auto& e : team) {
        int total_ferias_usadas = 0;
        int total_faltas = 0;

        for (const auto& mark : e.marks) {
            if (mark.second == 'F') total_ferias_usadas++;
            if (mark.second == 'X') total_faltas++;
        }

        cout << setw(30) << left << e.fullName << " | "
            << setw(13) << right << total_ferias_usadas << " | "
            << setw(11) << right << total_faltas << " | "
            << setw(14) << right << e.vacationDaysRemaining << endl;
    }
}

// Função para validar se uma string está no formato YYYY-MM-DD e é uma data real
bool is_valid_date_string(const string& s) {
    // 1. Verifica tamanho (tem de ter 10 caracteres: 2025-01-01)
    if (s.length() != 10) return false;

    // 2. Verifica os hífens nas posições certas
    if (s[4] != '-' || s[7] != '-') return false;

    // 3. Tenta extrair os números
    try {
        int y = stoi(s.substr(0, 4));
        int m = stoi(s.substr(5, 2));
        int d = stoi(s.substr(8, 2));

        // 4. Usa a função date_ok que já tens no código para validar a lógica
        return date_ok(d, m, y);
    }
    catch (...) {
        return false; // Se der erro na conversão de números
    }
}

// Requisito 6: Gerir Formações/Cursos (CRUD)
void manage_formacoes(vector<Employee>& team) {
    Employee* emp = find_employee_by_id_or_name(team);
    if (!emp) return;

    int opc = -1;
    while (opc != 0) {
        cout << "\n--- FORMACOES / CURSOS DE " << emp->fullName << " ---\n";
        cout << "[1] Listar formacoes\n";
        cout << "[2] Adicionar formacao\n";
        cout << "[3] Editar formacao\n";
        cout << "[4] Remover formacao\n";
        cout << "[0] Voltar\n";
        opc = get_int("Escolha uma opcao: ");

        if (opc == 1) { // Listar
            if (emp->formacoes.empty()) {
                cout << "Nenhuma formacao registada.\n";
            }
            else {
                cout << "\n--- Lista de Formacoes ---\n";
                for (size_t i = 0; i < emp->formacoes.size(); ++i) {
                    cout << "[" << i + 1 << "] Curso: " << emp->formacoes[i].nome_curso
                        << " | Data: " << emp->formacoes[i].data_conclusao << "\n";
                }
            }
        }
        else if (opc == 2) { // Adicionar
            Formacao f;
            get_line(f.nome_curso, "Nome do curso: ");
            // NOVO CÓDIGO (Validado)
            while (true) {
                get_line(f.data_conclusao, "Data de conclusao (YYYY-MM-DD): ");
                if (is_valid_date_string(f.data_conclusao)) {
                    break; // Data válida, sai do loop
                }
                cout << "ERRO: Data invalida! Use o formato YYYY-MM-DD e valores reais.\n";
            }
            emp->formacoes.push_back(f);
            cout << "Formacao adicionada com sucesso!\n";
        }
        else if (opc == 3) { // Editar
            int id = get_int("ID da formacao a editar: ") - 1;
            if (id >= 0 && id < emp->formacoes.size()) {
                get_line(emp->formacoes[id].nome_curso, "Novo nome do curso (" + emp->formacoes[id].nome_curso + "): ");
                get_line(emp->formacoes[id].data_conclusao, "Nova data de conclusao (" + emp->formacoes[id].data_conclusao + "): ");
                cout << "Formacao editada com sucesso!\n";
            }
            else {
                cout << "ID invalido.\n";
            }
        }
        else if (opc == 4) { // Remover
            int id = get_int("ID da formacao a remover: ") - 1;
            if (id >= 0 && id < emp->formacoes.size()) {
                emp->formacoes.erase(emp->formacoes.begin() + id);
                cout << "Formacao removida com sucesso!\n";
            }
            else {
                cout << "ID invalido.\n";
            }
        }
    }
}

// Requisito 7: Gerir Notas/Observações (CRUD)
void manage_notas(vector<Employee>& team) {
    Employee* emp = find_employee_by_id_or_name(team);
    if (!emp) return;

    int opc = -1;
    while (opc != 0) {
        cout << "\n--- NOTAS / OBSERVACOES DE " << emp->fullName << " ---\n";
        cout << "[1] Listar notas\n";
        cout << "[2] Adicionar nota\n";
        cout << "[3] Editar nota\n";
        cout << "[4] Remover nota\n";
        cout << "[0] Voltar\n";
        opc = get_int("Escolha uma opcao: ");

        if (opc == 1) { // Listar
            if (emp->notas.empty()) {
                cout << "Nenhuma nota registada.\n";
            }
            else {
                cout << "\n--- Lista de Notas ---\n";
                for (size_t i = 0; i < emp->notas.size(); ++i) {
                    cout << "[" << i + 1 << "] Data: " << emp->notas[i].data
                        << " | Texto: " << emp->notas[i].texto << "\n";
                }
            }
        }
        else if (opc == 2) { // Adicionar
            Nota n;
            get_line(n.texto, "Texto da nota: ");
            n.data = get_current_date_str();
            emp->notas.push_back(n);
            cout << "Nota adicionada com sucesso (Data: " << n.data << ")!\n";
        }
        else if (opc == 3) { // Editar
            int id = get_int("ID da nota a editar: ") - 1;
            if (id >= 0 && id < emp->notas.size()) {
                cout << "Texto atual: " << emp->notas[id].texto << "\n";
                get_line(emp->notas[id].texto, "Novo texto da nota: ");
                emp->notas[id].data = get_current_date_str();
                cout << "Nota editada com sucesso!\n";
            }
            else {
                cout << "ID invalido.\n";
            }
        }
        else if (opc == 4) { // Remover
            int id = get_int("ID da nota a remover: ") - 1;
            if (id >= 0 && id < emp->notas.size()) {
                emp->notas.erase(emp->notas.begin() + id);
                cout << "Nota removida com sucesso!\n";
            }
            else {
                cout << "ID invalido.\n";
            }
        }
    }
}

// Funções de Menu Principal -------------------------------------------------------------------------------

void process_menu_option(int opc, vector<Employee>& team) {
    switch (opc) {
    case 1: // Listar colaboradores
        cout << "\nColaboradores registados:\n";
        for (const auto& p : team) {
            cout << " - ID: " << p.id << " | Nome: " << p.fullName << " | Dept: " << p.department << "\n";
        }
        break;
    case 2: // Adicionar colaborador
        add_employee(team);
        break;
    case 3: { // Marcar/Desmarcar ferias e faltas
        int sub_opc;
        cout << "[1] Registar FERIAS (F)\n";
        cout << "[2] Registar FALTA (X)\n";
        cout << "[3] Remover Marcacao num Dia\n";
        cout << "Opcao: ";
        if (!(cin >> sub_opc)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break; }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (sub_opc == 1) mark_absence(team, 'F');
        else if (sub_opc == 2) mark_absence(team, 'X');
        else if (sub_opc == 3) {
            Employee* emp = find_employee_by_id_or_name(team);
            if (!emp) break;
            int d, m, y;
            cout << "Dia Mes Ano para remover (ex: 12 5 2025): ";
            if (!(cin >> d >> m >> y)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break; }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (!date_ok(d, m, y)) { cout << "Data invalida.\n"; break; }

            string key = to_string(y) + "-" + (m < 10 ? "0" : "") + to_string(m)
                + "-" + (d < 10 ? "0" : "") + to_string(d);

            auto it_mark = emp->marks.find(key);
            if (it_mark != emp->marks.end()) {
                if (it_mark->second == 'F') {
                    emp->vacationDaysRemaining++;
                    if (emp->vacationDaysRemaining > 22) emp->vacationDaysRemaining = 22;
                }
                emp->marks.erase(key);
                cout << "Marcacao removida.\n";
            }
            else {
                cout << "Nenhuma marcacao encontrada neste dia.\n";
            }
        }
        else { cout << "Opcao invalida.\n"; }
        break;
    }
    case 4:
        view_calendar(team);
        break;
    case 5: { // Buscar colaborador por ID ou nome (Requisito 2)
        const Employee* emp = find_employee_by_id_or_name_const(team);
        if (emp) {
            cout << "\n--- Dados do Colaborador ---\n";
            cout << "ID: " << emp->id << "\n";
            cout << "Nome: " << emp->fullName << "\n";
            cout << "Departamento: " << emp->department << "\n";
            cout << "Ferias Restantes: " << emp->vacationDaysRemaining << "\n";
            // Requisito 2: Mostrar calendário do mês atual após a busca
            time_t now = time(0);
            tm t_struct;
            // Usa localtime_s para segurança
            if (localtime_s(&t_struct, &now) == 0) {
                show_month(*emp, 1 + t_struct.tm_mon, 1900 + t_struct.tm_year);
            }
        }
        break;
    }
    case 6:
        manage_formacoes(team);
        break;
    case 7:
        manage_notas(team);
        break;
    case 8: // NOVO: Exportação de dados (Requisito 8)
        export_data(team);
        break;
    case 9: // Relatórios mensais (Antigo 8)
        monthly_report(team);
        break;
    case 10: // Estatísticas por departamento (Antigo 9)
        department_stats(team);
        break;
    case 11: // Dashboard resumido (Antigo 10)
        show_dashboard(team);
        break;
    default:
        if (opc != 0) cout << "Opcao invalida.\n";
        break;
    }
}


int main() {
    vector<Employee> team;
    const string store = "catarina_storage.txt";

    if (load_all(store, team))
        cout << "Base de dados carregada (" << team.size() << " colaboradores)\n";
    else
        cout << "Nenhuma base existente. A iniciar nova...\n";

    int opc = -1;

    while (opc != 0) {
        cout << "\n=============================\n";
        cout << "      RH AVANCADO - CATARINA\n";
        cout << "=============================\n";
        cout << "[1] Listar colaboradores\n";
        cout << "[2] Adicionar colaborador\n";
        cout << "[3] Marcar/Desmarcar ferias e faltas\n";
        cout << "[4] Visualizar calendario do colaborador\n";
        cout << "[5] Buscar colaborador por ID ou nome\n";
        cout << "[6] Gerir formacoes/cursos\n";
        cout << "[7] Gerir notas/observacoes\n";
        cout << "[8] Exportar Calendario / Relatorio (NOVO)\n"; // Alterado: Novo 8
        cout << "[9] Relatorios mensais\n"; // Alterado: Era 8
        cout << "[10] Estatisticas por departamento\n"; // Alterado: Era 9
        cout << "[11] Dashboard resumido\n"; // Alterado: Era 10 (Novo maximo)
        cout << "[0] Sair e guardar\n";
        cout << "-----------------------------\n";
        cout << "Opcao: ";

        if (!(cin >> opc)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        process_menu_option(opc, team);
    }

    save_all(store, team);
    cout << "Dados guardados em '" << store << "'\n";
    return 0;
}