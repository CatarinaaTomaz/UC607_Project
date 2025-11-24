#pragma once
#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include <string>
#include "employee.h" // Necessário para a struct Employee

// grava todos os colaboradores num ficheiro TXT
void save_all(const std::string& path, const std::vector<Employee>& list);

// carrega todos os colaboradores do ficheiro TXT
bool load_all(const std::string& path, std::vector<Employee>& list);

// Gera o próximo ID disponível (max ID + 1)
int generate_next_id(const std::vector<Employee>& list);

// cifra o nome do colaborador
std::string progressiveCipher(const std::string& s, int shiftBase);

// mostra o calendario mensal com marcacoes
void show_month(const Employee& e, int month, int year);

// Utilitário para encontrar colaborador por ID ou Nome (para edição/modificação)
Employee* find_employee_by_id_or_name(std::vector<Employee>& team);

// Utilitário para encontrar colaborador por ID ou Nome (const, para visualização/exportação)
const Employee* find_employee_by_id_or_name_const(const std::vector<Employee>& team);

// Requisito 8: Exportação de dados
void export_data(const std::vector<Employee>& team);

#endif