#pragma once
#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <map>
#include <vector>
#include <sstream>

// Estrutura para Cursos/Certificações (Requisito 6)
struct Formacao {
	std::string nome_curso;
	std::string data_conclusao; // Formato YYYY-MM-DD
};

// Estrutura para Notas/Observações (Requisito 7)
struct Nota {
	std::string texto;
	std::string data; // Data de registo (YYYY-MM-DD)
};

// estrutura que representa um colaborador
struct Employee {
	int id = 0; // ID único
	std::string fullName;
	std::string department; // Departamento
	int vacationDaysRemaining = 22; // Dias de férias restantes (Total 22)

	// mapa onde guardamos marcacoes:
	// key = "YYYY-MM-DD", value = 'F' (ferias) ou 'X' (falta)
	std::map<std::string, char> marks;

	// Listas de Formações e Notas
	std::vector<Formacao> formacoes;
	std::vector<Nota> notas;
};

// Declarações de Funções de Validação
bool leapYear(int year);
bool date_ok(int d, int m, int y);
bool weekend(int d, int m, int y);

// Funções utilitárias para datas
std::string get_current_date_str();

#endif // EMPLOYEE_H