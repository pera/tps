/*
 * Author: Brian Gomes Bascoy
 * Date: Fri Dec  2 14:26:06 ART 2011
 *
 */

#include <iostream>
#include <typeinfo>
#include <list>
#include <queue>
#include <boost/lexical_cast.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/barrier.hpp>

int demora; // tiempo de demora en milisegundos

void demorar()
{
	using namespace boost::chrono;
	steady_clock::time_point start = steady_clock::now();
	steady_clock::duration delay = milliseconds(demora);
	while (steady_clock::now() - start <= delay) {}
}

/*
 * Clase Persona y subclases Canibal y Misionero (identicos, usado para RTTI)
 *
 */
class Persona {
	private:
		boost::thread *myThread;
		bool fin;
		int id; // el "numero" de canibal o misionero
		const std::type_info &type; // contiene tipo actual para RTTI
		boost::mutex mutex; // este es usado unicamente para el condition
		boost::condition listo; // usado para esperar hasta proxima accion
	public:
		Persona(int,const std::type_info&);
		~Persona();
		int getId(){return id;}
		const std::type_info* getType(){return &type;}
		void avisar();
		void thread();
};

class Canibal: public Persona {
	public:
		Canibal(int id): Persona(id, typeid(this)){}
};

class Misionero: public Persona {
	public:
		Misionero(int id): Persona(id, typeid(this)){}
};

Persona::Persona(int n, const std::type_info &t):type(t)
{
	id = n;

	fin = false;
	myThread = new boost::thread(boost::bind(&Persona::thread, this));
}

Persona::~Persona()
{
	fin = true;
	{
		// Espero hasta que espere y notifico
		boost::mutex::scoped_lock lock(mutex);
		listo.notify_one();
	}

	myThread->join();
}

/*
 * Clase Monitor
 *
 */
class Monitor {
	private:
		boost::thread *myThread;
		bool fin;
		bool apagando;
		bool apagado; // lo uso para las barriers de los prelocks
		std::list<Persona*> personas;
		boost::mutex m_monitor; // usado para notificar c_preparado
		boost::mutex m_pre_barcoListo; // usado para bloquear barrier
		boost::mutex m_pre_barcoYaCruzo; // usado para bloquear barrier
		boost::barrier *safe_init; // sincronizar el init
		boost::barrier *safe_shutdown; // sincronizar el shutdown
		boost::barrier *b_barcoListo;
		boost::barrier *b_barcoYaCruzo;
		boost::condition c_preparado; // persona o barco preparado para salir
		boost::condition c_barco_cargable; // espera carga segura
		boost::condition c_barco_zarpo; // espera a que el barco zarpe
		boost::condition c_barco_termino; // barco llego a destino y sus tripulantes salieron
		bool carga_lista;
		bool barco_listo;
		enum { no, CCC, MMM, CMM } encontrado;
		int cruces;
		const int vueltas; // cantidad de vueltas a realizar
		void kernel();

		bool allow_personaLlegando;
		bool allow_barcoListo;
		bool allow_barcoYaCruzo;
	public:
		Monitor(int);
		~Monitor();
		void shutdown(bool);

		bool canibalLlegando(Canibal*);
		bool misioneroLlegando(Misionero*);
		bool barcoListo(Persona *tripulacion[3]);
		bool barcoYaCruzo(Persona *tripulacion[3]);
} *monitor;

Monitor::Monitor(int v): vueltas(v)
{
	std::cout << "Iniciando monitor... ";
	
	fin = false;
	carga_lista = false;
	barco_listo = false;
	apagando = false;
	apagado = false;
	cruces = 0;
	encontrado = no;
	safe_init = new boost::barrier(2);
	safe_shutdown = new boost::barrier(2);
	b_barcoListo = new boost::barrier(2);
	b_barcoYaCruzo = new boost::barrier(2);

	allow_personaLlegando = true;
	allow_barcoListo = true;
	allow_barcoYaCruzo = true;

	// Creo el thread del monitor y espero hasta que este activo
	myThread = new boost::thread(boost::bind(&Monitor::kernel, this));
	safe_init->wait();
	
	std::cout << "OK\n";
}

/**
 * Inserta un canibal en la fila de espera para navegar
 * Llamado por los threads de canibales.
 */
bool Monitor::canibalLlegando(Canibal *canibal)
{
	boost::mutex::scoped_lock lock(m_monitor);

	if (allow_personaLlegando) {
		personas.push_back(canibal);
		c_preparado.notify_one();
	} else {
		return false;
	}

	return true;
}

/**
 * Inserta un misionero en la fila de espera para navegar.
 * Llamado por los threads de misioneros.
 */
bool Monitor::misioneroLlegando(Misionero *misionero)
{
	boost::mutex::scoped_lock lock(m_monitor);

	if (allow_personaLlegando) {
		personas.push_back(misionero);
		c_preparado.notify_one();
	} else {
		return false;
	}

	return true;
}

/**
 * Intenta tomar las personas para ponerlas en el barco.
 * Llamado por el thread del barco.
 */ 
bool Monitor::barcoListo(Persona *tripulacion[3])
{
	{
		boost::mutex::scoped_lock pre_lock(m_pre_barcoListo);
		if (!apagado)
			b_barcoListo->wait();
	}

	boost::mutex::scoped_lock lock(m_monitor);

	if (allow_barcoListo) {
		std::list<Persona*>::iterator iteration;

		barco_listo = true;
		c_preparado.notify_one();
		c_barco_cargable.wait(lock);

		int c_tmp;
		int i=0;
		bool canibal_solo=false;
		switch (encontrado) {
			case CCC:
				iteration=personas.begin();
				while (iteration!=personas.end() && i<3) {
					if ((*(*iteration)->getType())==typeid(Canibal*)) {
						tripulacion[i]=(*iteration);
						i++;
						iteration=personas.erase(iteration);
					} else {
						iteration++;
					}
				}
				std::cout << "MONITOR("<<cruces<<"): se seleccionan tres canibales ("
					<< tripulacion[0]->getId() << ","
					<< tripulacion[1]->getId() << ","
					<< tripulacion[2]->getId() << ")\n";
				break;
			case MMM:
				iteration=personas.begin();
				while (iteration!=personas.end() && i<3) {
					if ((*(*iteration)->getType())==typeid(Misionero*)) {
						tripulacion[i]=(*iteration);
						i++;
						iteration=personas.erase(iteration);
					} else {
						iteration++;
					}
				}
				std::cout << "MONITOR("<<cruces<<"): se seleccionan tres misioneros ("
					<< tripulacion[0]->getId() << ","
					<< tripulacion[1]->getId() << ","
					<< tripulacion[2]->getId() << ")\n";
				break;
			case CMM:
				iteration=personas.begin();
				while (iteration!=personas.end() && i<3) {
					if ((*(*iteration)->getType())==typeid(Canibal*) && !canibal_solo) {
						tripulacion[i]=(*iteration);
						c_tmp=i; // guardo esta posicion
						i++;
						canibal_solo=true;
						iteration=personas.erase(iteration);
					} else if ((*(*iteration)->getType())==typeid(Misionero*)) {
						tripulacion[i]=(*iteration);
						i++;
						iteration=personas.erase(iteration);
					} else {
						iteration++;
					}
				}
				std::cout << "MONITOR("<<cruces<<"): se seleccionan un canibal ("
					<< tripulacion[c_tmp]->getId() << ") y dos misioneros ("
					<< tripulacion[(c_tmp!=1)]->getId() << ","
					<< tripulacion[(c_tmp!=2)<<1]->getId() << ")\n";
				break;
			default:
				break;
		}

		barco_listo = false;
		c_barco_zarpo.notify_one();
	} else {
		return false;
	}

	return true;
}

/**
 * Saca las personas del barco.
 * Llamado por el thread del barco.
 */ 
bool Monitor::barcoYaCruzo(Persona *tripulacion[3])
{
	{
		boost::mutex::scoped_lock pre_lock(m_pre_barcoYaCruzo);
		if (!apagado)
			b_barcoYaCruzo->wait();
	}

	boost::mutex::scoped_lock lock(m_monitor);

	if (allow_barcoYaCruzo) {
		cruces++;
		for(int i=0;i<3;i++){
			tripulacion[i]->avisar();
		}
		c_barco_termino.notify_one();
	} else {
		return false;
	}

	return true;
}

/**
 * Controla el monitor para sincronizar los eventos.
 */
void Monitor::kernel()
{
	boost::mutex::scoped_lock lock(m_monitor);
	safe_init->wait();

	std::list<Persona*>::iterator i;

	while (!fin) {
		// Cada vez que un objeto dice estar listo se verifican las condiciones
		// para poder navegar; esto se repite hasta que todas estas se cumplan.
		b_barcoListo->wait();

		encontrado = no;
		while (!(encontrado && barco_listo) ) {
			c_preparado.wait(lock);

			if (personas.size() >= 3) {
				int c=0;
				int m=0;
				for (i=personas.begin(); i!=personas.end() && !encontrado; i++) {
					if ((*(*i)->getType())==typeid(Canibal*))
						c++;
					else
						m++;

					if (c==3) {
						encontrado = CCC;
					} else if (m==3) {
						encontrado = MMM;
					} else if (c>=1 && m==2) {
						encontrado = CMM;
					}
				}
			}
		}

		c_barco_cargable.notify_one();
		c_barco_zarpo.wait(lock);

		b_barcoYaCruzo->wait();
		c_barco_termino.wait(lock);
		
		if (vueltas == cruces)
			fin = true;
	}

	allow_barcoListo = false;
	allow_barcoYaCruzo = false;

	// verifico que no haya barriers esperando
	apagado = true;

	if (m_pre_barcoListo.try_lock()) m_pre_barcoListo.unlock();
	else b_barcoListo->wait();

	if (m_pre_barcoYaCruzo.try_lock()) m_pre_barcoYaCruzo.unlock();
	else b_barcoYaCruzo->wait();

	std::cout << "MONITOR: se han hecho "<<cruces<<" cruces al rio.\n";
	std::cout << "MONITOR: este cruce se ha cerrado hasta nuevo aviso, por malas condiciones del clima.\n";

	safe_shutdown->wait();
}

/**
 * Primera etapa de destruccion del monitor: usado para avisar metodos de este.
 * El parametro now indica si se quiere esperar a que finalice las vueltas indicadas.
 * Llamado por el destructor de init.
 */
void Monitor::shutdown(bool now)
{
	if (now) {
		boost::mutex::scoped_lock lock(m_monitor);
		fin = true;
	}
	safe_shutdown->wait();
}

Monitor::~Monitor()
{
	assert(fin==true); // No deberia destruir el monitor sin apagarlo antes
	myThread->join();
}

/**
 * Usado para indicarle a algun thread de canibal/misionero que puede salir del barco.
 */
void Persona::avisar()
{
	boost::mutex::scoped_lock lock(mutex);
	listo.notify_one();
}

void Persona::thread()
{
	boost::mutex::scoped_lock lock(mutex);
	while (!fin) {
		demorar();
		
		if(type==typeid(Canibal*))
			monitor->canibalLlegando((Canibal*)this);
		else
			monitor->misioneroLlegando((Misionero*)this);

		// Espero (en fila y luego barco) hasta llegar a la otra orilla
		listo.wait(lock);
	}
}

/*
 * Clase Barco
 *
 */
class Barco {
	private:
		boost::thread *myThread;
		bool fin;
		boost::mutex mutex; // este es usado unicamente para el condition
		boost::condition listo; // usado para esperar hasta proxima accion
		Persona *tripulacion[3];
	public:
		Barco();
		~Barco(){fin=true; myThread->join();}
		void thread();
};

Barco::Barco()
{
	fin = false;
	myThread = new boost::thread(boost::bind(&Barco::thread, this));
}

void Barco::thread()
{
	while (!fin){
		demorar();
		monitor->barcoListo(tripulacion);

		demorar();
		monitor->barcoYaCruzo(tripulacion);
	}
}

/*
 * Clase inicializador: crea los threads de canibales, misioneros y el del barco
 *
 */
class init {
	private:
		std::list<Canibal*> canibales;
		std::list<Misionero*> misioneros;
		Barco *barco;
	public:
		init(int,int,int);
		~init();
		Barco* getBarco();
};

init::init(int cantidad_canibales, int cantidad_misioneros, int vueltas)
{
	monitor = new Monitor(vueltas);

	std::cout << "Creando " << cantidad_canibales << " canibales" << "... " << std::flush;
	for(int i=0; i<cantidad_canibales; i++){
		canibales.push_back(new Canibal(i));
	}
	std::cout << "OK" << std::endl;

	std::cout << "Creando " << cantidad_misioneros << " misioneros" << "... " << std::flush;
	for(int i=0; i<cantidad_misioneros; i++){
		misioneros.push_back(new Misionero(i));
	}
	std::cout << "OK" << std::endl;

	std::cout << "Creando barco... " << std::flush;
	barco = new Barco();
	std::cout << "OK" << std::endl;

	std::cout << std::endl;
}

init::~init()
{
	//std::cout << "* Iniciando apagado del monitor *" << std::endl;
	monitor->shutdown(false); // indico false para que espere a dar todas las vueltas
	std::cout << std::endl;

	std::cout << "Terminando barco... " << std::flush;
	delete barco;
	std::cout << "OK" << std::endl;

	std::cout << "Terminando canibales... " << std::flush;
	for (std::list<Canibal*>::iterator iteration=canibales.begin(); iteration!=canibales.end() ; iteration++)
		delete *iteration;
	std::cout << "OK" << std::endl;

	std::cout << "Terminando misioneros... " << std::flush;
	for (std::list<Misionero*>::iterator iteration=misioneros.begin(); iteration!=misioneros.end() ; iteration++)
		delete *iteration;
	std::cout << "OK" << std::endl;

	std::cout << "Terminando monitor... " << std::flush;
	delete monitor;
	std::cout << "OK" << std::endl;
}

/*
 * Main: verifica parametros, inicia el programa y lo finaliza.
 *
 */
int main(int argc, const char *argv[])
{
	init *program;
	if (argc != 5){
		std::cerr << "ERROR: Cantidad de argumentos invalida.\n";
		std::cerr << argv[0] << " cantidad_canibales cantidad_misioneros vueltas delay(ms)\n";
		return 1;
	}
	
	try {
		int cantidad_canibales = boost::lexical_cast<short>(argv[1]);
		int cantidad_misioneros = boost::lexical_cast<short>(argv[2]);
		int vueltas = boost::lexical_cast<short>(argv[3]);
		demora = boost::lexical_cast<short>(argv[4]);

		if(demora < 1) {
			std::cerr << "ERROR: El delay debe ser de por lo menos 1 milisegundo.\n";
			return 1;
		}

		if( (cantidad_canibales >= 1) && (cantidad_misioneros >= 2) ) {
			program = new init(cantidad_canibales, cantidad_misioneros, vueltas);
		} else {
			std::cerr << "ERROR: Cantidad de canibales o misioneros insuficiente.\n";
			return 1;
		}
	} catch(boost::bad_lexical_cast &) {
		std::cerr << "ERROR: Argumentos invalidos.\n";
		return 1;
	}

	delete program;

	return 0;
}

