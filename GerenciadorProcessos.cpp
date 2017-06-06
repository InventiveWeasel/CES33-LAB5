#include <bits/stdc++.h>
using namespace std;

//Debug
#define DEBUG 1
#define debug if(DEBUG) printf

//Numero de processadores
#define N 16

//Parametros das metricas
#define RETRY 4
#define OVERCHARGE 2.0
#define UNDERCHARGE 0.5

//Tempo atual
double tempoDecorrido;
#define CONVERSAO_TEMPO 20.0

//
// Log de eventos
//
vector<string> logEventos;
char buffer[1000];
bool logEvento(string evento) {
	sprintf(buffer, "%.6f", tempoDecorrido);
	evento = "Tempo: " + string(buffer) + " | " + evento;
	logEventos.push_back(evento);
	return true;
}
bool escreverLogEventos(char* arquivo) {
	if (logEventos.empty()) return false;
	debug("Escrevendo log...\n");
	if (arquivo != NULL) freopen(arquivo, "w", stdout);
	for (int i = 0; i < (int)logEventos.size(); i++) {
		printf("%s\n", logEventos[i].c_str());
	}
	if (arquivo != NULL) fclose(stdout);
}

//
// Processo
//
class Processo {
public:
	int id, cpuCriada;
	double tempoDuracao, tempoCriacao, tempoInicio;
	Processo() : id(-1), cpuCriada(-1), tempoDuracao(-1.0), tempoCriacao(-1.0), tempoInicio(-1.0) {}
	Processo(int _id, int _cpuCriada, double _tempoCriacao, double _tempoDuracao) :
		id(_id), cpuCriada(_cpuCriada), tempoCriacao(_tempoCriacao), tempoDuracao(_tempoDuracao), tempoInicio(-1.0) {}
	bool valido() {
		return cpuCriada >= 0;
	}
	
};
bool comparadorTempoCriacao(const Processo &p1, const Processo &p2) {
	return p1.tempoCriacao < p2.tempoCriacao;
}
vector<Processo> processos;

//
// Processador
//
class Processador {
public:
	int numero;
	int msgTransmitidas, msgRecebidas;
	queue<Processo> filaProcessos;
	double tempoFila;
	Processo procAtual;
	Processador() {}
	Processador(int _numero) : numero(_numero), tempoFila(0.0), msgRecebidas(0), msgTransmitidas(0) {}
	bool inserirProcesso(Processo processo) {
		filaProcessos.push(processo);
		tempoFila += processo.tempoDuracao;
		return true;
	}
	bool atualizar() {
		//Verifica se vai finalizar processo atual
		if (procAtual.valido()) {
			if (procAtual.tempoInicio + procAtual.tempoDuracao < tempoDecorrido) {
				sprintf(buffer, "Processo %d encerrado na CPU %d: %d %.6f %.6f", procAtual.id, numero, procAtual.cpuCriada, procAtual.tempoCriacao, procAtual.tempoDuracao);
				logEvento(string(buffer));
				procAtual = Processo();
			}
		}

		//Se nao tiver processo atual, inicia o proximo
		if (!procAtual.valido() && !filaProcessos.empty()) {
			procAtual = filaProcessos.front();
			filaProcessos.pop();
			tempoFila -= procAtual.tempoDuracao;
			procAtual.tempoInicio = tempoDecorrido;
			sprintf(buffer, "Processo %d iniciado na CPU %d: %d %.6f %.6f", procAtual.id, numero, procAtual.cpuCriada, procAtual.tempoCriacao, procAtual.tempoDuracao);
			logEvento(string(buffer));
		}
		return true;
	}
	double getTempoRestante() {
		double tempoRestante = tempoFila;
		if (procAtual.valido() && procAtual.tempoInicio + procAtual.tempoDuracao > tempoDecorrido) {
			tempoRestante += procAtual.tempoInicio + procAtual.tempoDuracao - tempoDecorrido;
		}
		return tempoRestante;
	}
	bool sobrecarregado(double tempoMedio) {
		return getTempoRestante() > OVERCHARGE*tempoMedio;
	}
	bool subcarregado(double tempoMedio) {
		return getTempoRestante() < UNDERCHARGE*tempoMedio;
	}
};
vector<Processador> processadores(N);

//
// Algoritmo de escolha do próximo procesador
//
int escolherProcessador(int preferencia) {

	//Calcula o tempo total de processamento de todas as CPUs
	double tempoTotal = 0.0;
	for (int i = 0; i < N; i++) {
		tempoTotal += processadores[i].getTempoRestante();
	}
	double tempoMedio = tempoTotal / N;

	//Caso 1: preferencia esta subcaregada
	if (processadores[preferencia].subcarregado(tempoMedio)) {
		return preferencia;
	}

	//Caso 2: preferencia esta na faixa
	if (!processadores[preferencia].sobrecarregado(tempoMedio)) {
		for (int retry = 0; retry < RETRY; retry++) {
			int alvo = rand() % N;
			processadores[preferencia].msgTransmitidas++;
			processadores[alvo].msgRecebidas++;
			if (processadores[alvo].subcarregado(tempoMedio)) {
				return alvo;
			}
		}
		return preferencia;
	}

	//Caso 3: preferencia sobrecarregada
	for (int retry = 0; retry < RETRY; retry++) {
		int alvo = rand() % N;
		processadores[preferencia].msgTransmitidas++;
		processadores[alvo].msgRecebidas++;
		if (!processadores[alvo].sobrecarregado(tempoMedio)) {
			return alvo;
		}
	}
	return preferencia;
}

//
// Simulacao
//
bool simular() {
	
	int proxProcesso = 0;
	clock_t clockInicio, clockAtual;
	Processo procAtual;

	//Inicializa variaveis globais
	srand(time(NULL));
	logEventos.clear();
	for (int index = 0; index < N; index++) {
		processadores[index] = Processador(index);
	}
	debug("Variaveis globais iniciadas\n");

	clockInicio = clock();
	while (true) {
		//Calcula o tempo decorrido ate este ponto
		clockAtual = clock();
		tempoDecorrido = (clockAtual - clockInicio) * 1.0 * CONVERSAO_TEMPO * 1.0 / CLOCKS_PER_SEC;
		
		//Verifica se eh hora de acionar o proximo processo
		while (proxProcesso < (int)processos.size() && processos[proxProcesso].tempoCriacao <= tempoDecorrido) {
			procAtual = processos[proxProcesso];
			proxProcesso++;
			int cpuAlvo = escolherProcessador(procAtual.cpuCriada);
			processadores[cpuAlvo].inserirProcesso(procAtual);
			sprintf(buffer, "Processo %d criado na CPU %d: %d %.6f %.6f", procAtual.id, cpuAlvo, procAtual.cpuCriada, procAtual.tempoCriacao, procAtual.tempoDuracao);
			logEvento(string(buffer));
		}

		//Atualiza todos os processadores
		for (int index = 0; index < N; index++) {
			processadores[index].atualizar();
		}

		//Verifica se a simulacao acabou
		if (proxProcesso == (int)processos.size()) {
			bool parou = true;
			for (int index = 0; index < N && parou; index++) {
				if (processadores[index].procAtual.valido() || !processadores[index].filaProcessos.empty())
					parou = false;
			}
			if (parou) break;
		}
	}

	//Imprime o numero de mensagens
	for (int i = 0; i < N; i++) {
		sprintf(buffer, "Mensagens processador %d: transmitidas %d recebidas %d", i, processadores[i].msgTransmitidas, processadores[i].msgRecebidas);
		logEvento(string(buffer));
	}

	return true;
}

//
// Leitura do arquivo de entrada
//
bool lerEntrada(char* arquivo) {
	debug("Lendo entrada %s...\n", arquivo);
	if (arquivo != NULL) freopen(arquivo, "r", stdin);
	int idAtual = 1, cpuAtual;
	double tempoCriacao, tempoDuracao;
	while (!feof(stdin)) {
		scanf(" %d %lf %lf", &cpuAtual, &tempoCriacao, &tempoDuracao);
		processos.push_back(Processo(idAtual, cpuAtual, tempoCriacao, tempoDuracao));
		idAtual++;
	}
	if (arquivo != NULL) fclose(stdin);
	debug("Entrada lida com sucesso\n");
	return true;
}

int main(int argc, char* argv[]) {

	//Le o arquivo de entrada
	lerEntrada(NULL);

	//Ordena por tempo de criacao
	sort(processos.begin(), processos.end(), &comparadorTempoCriacao);

	//Simula o algoritmo
	simular();

	//Printa o log de eventos
	escreverLogEventos(NULL);

	return 0;
}