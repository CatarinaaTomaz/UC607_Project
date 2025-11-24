
## RH Avançado - Catarina
Sistema de Gestão de Recursos Humanos em Consola (C++)

Este projeto é uma aplicação de consola desenvolvida em C++ para facilitar a gestão administrativa de colaboradores, controlo de assiduidade, formação e relatórios numa organização. O sistema utiliza persistência de dados em ficheiro de texto com funcionalidades de segurança básica.

## Funcionalidades Principais
1. Gestão de Colaboradores

Adicionar Colaborador: Registo de novos funcionários com geração automática de ID e atribuição de departamento.
Verificação de Duplicados: Deteta se um nome já existe e permite a atualização/substituição dos dados do colaborador existente.
Busca Inteligente: Localização de colaboradores por ID ou Nome.

2. Controlo de Assiduidade e Férias
Marcação de Férias/Faltas: Registo em calendário com validação de datas.

Regras de Negócio:

Impede marcações ao fim de semana.
Verifica limite anual de dias de férias (22 dias).
Deteção de Conflitos: Alerta se outro colega do mesmo departamento já tem férias marcadas para o mesmo dia.
Visualização de Calendário: Exibe o mês com dias úteis, fins de semana (*), férias (F) e faltas (X).

3. Gestão de Formação e Notas

Cursos/Formações: CRUD completo (Adicionar, Editar, Remover, Listar) para registo de formações concluídas.
Notas/Observações: Sistema de anotações datadas para registo de ocorrências ou avaliações do colaborador.
Validação de Dados: Garante que as datas inseridas estão no formato correto (YYYY-MM-DD) e são válidas (funcionalidade adicionada recentemente).

4. Relatórios e Dashboard

Relatórios Mensais: Resumo de férias e faltas por mês.
Estatísticas por Departamento: Análise de qual departamento tem maior taxa de ausência.
Dashboard Resumido: Visão geral de todos os colaboradores, saldo de férias e faltas anuais.

5. Exportação de Dados (Novo)

Exportar Calendário: Gera um ficheiro .txt individual com o histórico do colaborador.
Relatório de Departamento: Exporta uma lista completa de ausências filtrada por departamento ou geral.

6. Armazenamento e Segurança

Persistência: Todos os dados são guardados automaticamente em catarina_storage.txt ao sair.
Encriptação: Os nomes dos colaboradores são gravados de forma cifrada (Cifra de César progressiva) para privacidade básica dos dados.


## Como Compilar e Executar
Pré-requisitos
Compilador C++ (GCC, Clang ou MSVC).

Sistema operativo Windows (recomendado devido ao uso de localtime_s e comandos de consola) ou Linux (com pequenos ajustes).

No Visual Studio
Crie um novo projeto de "Console App".

Adicione os ficheiros main.cpp, employee.h, e storage.h.

Certifique-se de que a definição #define _CRT_SECURE_NO_WARNINGS está no topo do main.cpp para evitar erros com funções de tempo.

Compile e execute (F5).

Na Linha de Comandos (g++)
Bash
g++ main.cpp -o rh_sistema
./rh_sistema


## Estrutura do Menu
O sistema é navegado através de um menu numérico:

Listar colaboradores: Mostra ID, Nome e Departamento.
Adicionar colaborador: Cria novo perfil.
Marcar/Desmarcar ferias e faltas: Gestão diária.
Visualizar calendário: Visão mensal gráfica.
Buscar colaborador: Pesquisa por ID ou Nome.
Gerir formações/cursos: Adiciona histórico académico.
Gerir notas/observações: Adiciona anotações de RH.
Exportar Calendário / Relatório: Gera ficheiros externos.
Relatórios mensais: Totais de ausências num mês específico.
Estatísticas por departamento: Totais anuais por setor.
Dashboard resumido: Tabela geral de saldos de férias.
Sair e guardar: Grava os dados e encerra.

## Estrutura do Ficheiro de Dados (catarina_storage.txt)
O ficheiro de base de dados utiliza um formato textual personalizado com delimitadores:

***: Separador de registos de colaboradores.

ID|: Identificador numérico.
DEP|: Departamento.
VRD|: Dias de férias restantes.
FMT|: Formações (Formato: NomeCurso;Data).
NOT|: Notas (Formato: Texto;Data).
YYYY-MM-DD|T: Marcações de calendário (onde T é o tipo: F ou X).
(Linha sem prefixo): Nome do colaborador (Cifrado).

👤 Autor
Desenvolvido no âmbito do projeto RH Avançado - Catarina.
