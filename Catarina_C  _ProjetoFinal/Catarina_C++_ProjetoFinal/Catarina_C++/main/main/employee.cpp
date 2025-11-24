#define _CRT_SECURE_NO_WARNINGS // Desativa o aviso C4996 do Visual Studio

#include "employee.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip> // ESSENCIAL para setfill/setw

using namespace std;

// Verifica se o ano é bissexto
bool leapYear(int year) {
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Valida se uma data existe realmente
bool date_ok(int d, int m, int y) {
	if (m < 1 || m > 12 || d < 1) return false;
	int mdays[] = { 31, (leapYear(y) ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	return d <= mdays[m - 1];
}

// Indica se aquela data cai num fim de semana
bool weekend(int d, int m, int y) {
	tm t = { 0, 0, 0, d, m - 1, y - 1900 };
	mktime(&t); // Preenche tm_wday
	return t.tm_wday == 0 || t.tm_wday == 6; // Domingo (0) ou Sábado (6)
}

// Obtém a data atual no formato YYYY-MM-DD
std::string get_current_date_str() {
	time_t now = time(0);
	tm t_struct;

	// Usa localtime_s para segurança (para evitar C4996)
	if (localtime_s(&t_struct, &now) != 0) {
		return "ERROR_DATE";
	}
	tm* ltm = &t_struct;

	stringstream ss;
	ss << (1900 + ltm->tm_year) << "-"
		<< setfill('0') << setw(2) << (1 + ltm->tm_mon) << "-"
		<< setfill('0') << setw(2) << ltm->tm_mday;
	return ss.str();
}