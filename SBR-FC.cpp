#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>
#include <iomanip>

#define DELIM " ,:="
#define OPERATIONS "yo"

using namespace std;

ofstream logfile;
int tab = -4;
string goal;

/* Clase "Regla" que representa una regla: 'id': Si 'lhs' Entonces 'rhs', FC='fc' */
class Regla {
  private:
      string id;          // Identificador de la regla
      list<string> lhs;   // Antecedente
      string rhs;         // Consecuente
      float fc;           // Factor de certeza
  public:
      Regla(string id, list<string> lhs, string rhs, float fc) {    // Constructor
          this->id = id;
          this->lhs = lhs;
          this->rhs = rhs;
          this->fc = fc;
      }

      string getId() {
        return this->id;
      }

      list<string> getLhs() {
        return this->lhs;
      }

      string getRhs() {
        return this->rhs;
      }

      float getFc() {
        return this->fc;
      }

      list<string> getAntecedentes() {
        list<string> antecedentes;
        string a;
        list<string>::iterator it = this->lhs.begin();
        while(it != this->lhs.end()) {
          if((*it).compare("o") != 0 && (*it).compare("y") != 0) {
            a.insert(0, (*it).c_str());
            antecedentes.push_back(a);
            a.clear();
          }
          it++;
        }
        return antecedentes;
      }

      void imprimir() {
        cout << this->id << ":";
        list<string>::iterator it = this->lhs.begin();
        while(it != this->lhs.end()) {
          cout << " " + *it;
          it++;
        }
        cout << " -> " << this->rhs << " ,FC=" << this->fc << endl;
      }
};

/* Clase que representa un hecho: 'h', FC='fc'*/
class Hecho {
  private:
    string h;   // Hecho
    float fc;   // Factor de certeza
  public:
    Hecho(string h, float fc) {
      this->h = h;
      this->fc = fc;
    }

    string getHecho() {
      return this->h;
    }

    float getFc() {
      return this->fc;
    }

    void setFc(float fc) {
      this->fc = fc;
    }

    void imprimir() {
      cout << this->h << " ,FC=" << this->fc << endl;
    }
};

/* Imprime el uso del programa */
void print_help() {
  cerr << "Uso: .\\SBR-FC BC-filename BH-filename" << endl;
}

void log_cc(list<Regla> *cc, bool indent) {
  list<Regla>::iterator it = cc->begin();
  if (indent) {
    logfile << string(tab, ' ');
  }
   logfile << "CC = {";
  while(it != cc->end()) {
    logfile << " " + (*it).getId();
    it++;
  }
  logfile << " }";
}

void log_nuevasMetas(list<string> *nm) {
  list<string>::iterator it = nm->begin();
  logfile << string(tab, ' ') + "NuevasMetas = {";
  while(it != nm->end()) {
    logfile << " " + *it;
    it++;
  }
  logfile << " }" << endl;
}

void log_verificar(string meta, list<Hecho> *BH) {
  logfile << string(tab, ' ') + "Verificar(" + meta + ", {";
  list<Hecho>::iterator it = BH->begin();
  logfile << (*it).getHecho();
  it++;
  while(it != BH->end()) {
    logfile << "," + (*it).getHecho();
    it++;
  }
  logfile << "})";
}

void log_BH(list<Hecho> *BH, bool indent) {
  if(indent) {
    logfile << string(tab, ' ');
  }
  list<Hecho>::iterator it = BH->begin();
   logfile << "BH = {";
  while(it != BH->end()) {
    logfile << " " + (*it).getHecho();
    it++;
  }
  logfile << " }" << endl;
}

/* True si 'meta' está en BH. False en caso contrario. */
bool contenida(string meta, list<Hecho> *BH) {
  list<Hecho>::iterator it = BH->begin();
  while(it != BH->end()) {
    if (it->getHecho().compare(meta) == 0) {
      return true;
    }
    it++;
  }
  return false;
}

/* Devuelve la lista de Reglas cuyo Consecuente coincide con 'meta'. */
list<Regla> equiparar(list<Regla> *BC, string meta) {
  list<Regla> cc;
  list<Regla>::iterator it = BC->begin();
  while(it != BC->end()) {
    if (it->getRhs().compare(meta) == 0) {
      cc.push_back(*it);
    }
    it++;
  }
  return cc;
}

/* Selecciona la primera Regla de la lista 'cc' como siguiente regla a resolver y la elimina de 'cc'. */
Regla resolver(list<Regla> *cc) {
  Regla first = cc->front();
  Regla r(first.getId(), first.getLhs(), first.getRhs(), first.getFc());
  cc->pop_front();
  return r;
}

/* Selecciona la primera cadena de 'metas' para usarla como próxima meta y la elimina de 'metas'. */
string seleccionarMeta(list<string> *metas) {
  string nMeta;
  nMeta.insert(0, metas->front());
  metas->pop_front();
  return nMeta;
}

/* Devuelve el factor de certeza del hecho 'a' de la Base de Hechos*/
float factorCerteza(string a, list<Hecho> *BH) {
  list<Hecho>::iterator it = BH->begin();
  float fc;
  while(it != BH->end()) {
    if((*it).getHecho().compare(a) == 0) {
      fc = (*it).getFc();
      return fc;
    }
    it++;
  }
  return 0.0;
}

/* Combinación de antecedentes y encadenamiento de evidencia */
float combinarFC(Regla *r, list<Regla> *BC, list<Hecho> *BH) {
  bool caso1 = false;
  list<string> antecedentes = r->getLhs();
  list<string>::iterator it = antecedentes.begin();
  float fc = factorCerteza(*it, BH);
  it++;
  while(it != antecedentes.end()) {
    if ((*it).compare("o") == 0) {
      caso1 = true;
      it++;
      fc = max(fc, factorCerteza(*it, BH));
    } else if ((*it).compare("y") == 0) {
      caso1 = true;
      it++;
      fc = min(fc, factorCerteza(*it, BH));
    }
    it++;
  }
  if (caso1) {
    logfile << string(tab, ' ') + "Caso 1: " + r->getRhs() + ", " << fc << endl; 
  }
  fc = fc*r->getFc();
  logfile << string(tab, ' ') + "Caso 3: " + r->getRhs() + ", " << fc << endl; 
  return fc;
}

/* Añade nueva evidencia a la Base de Hechos y resuelve los conflictos, si los hay. */
void nuevaEvidencia(string meta, float fc1, list<Hecho> *BH) {
  list<Hecho>::iterator it = BH->begin();
  while(it != BH->end()) {
    if ((*it).getHecho().compare(meta) == 0) {
      float fc2 = (*it).getFc();
      if (fc1 >= 0 && fc2 >= 0) {
        (*it).setFc(fc1 + fc2*(1-fc1));
      } else if (fc1 <= 0 && fc2 <= 0) {
        (*it).setFc(fc1 + fc2*(1+fc1));
      } else {
        (*it).setFc((fc1 + fc2)/(1 - min(abs(fc1), abs(fc2))));
      }
      logfile << string(tab, ' ') + "Caso 2: " + meta + ", " << (*it).getFc() << endl;
      return;
    }
    it++;
  }
  Hecho nuevoHecho(meta, fc1);
  BH->push_back(nuevoHecho);
}

bool backwardChaining(string meta, list<Regla> *BC, list<Hecho> *BH) {
  bool verificado = false;
  if (!contenida(meta, BH)) {
    logfile << endl;
    tab += 4;
    list<Regla> cc = equiparar(BC, meta);
    log_cc(&cc, true);
    logfile << endl;
    while(!cc.empty()) {
      Regla r = resolver(&cc);
      logfile << string(tab, ' ') + r.getId() + " (regla activada)" << endl;
      logfile << string(tab, ' ') + "Eliminar " + r.getId() + " --> ";
      log_cc(&cc, false);
      logfile << endl;
      list<string> nuevasMetas = r.getAntecedentes();
      log_nuevasMetas(&nuevasMetas);
      verificado = true;
      tab += 4;
      while(!nuevasMetas.empty() && verificado) {
        string nuevaMeta = seleccionarMeta(&nuevasMetas);
        logfile << string(tab, ' ') + "Meta = " + nuevaMeta << endl;
        log_nuevasMetas(&nuevasMetas);
        log_verificar(nuevaMeta, BH);
        verificado = backwardChaining(nuevaMeta, BC, BH);
      }
      tab -= 4;
      float fc = combinarFC(&r, BC, BH); // Factor de certeza a partir de los antecedentes (Caso 1) y de la regla (Caso 3)
      nuevaEvidencia(meta, fc, BH);     // Comprobar si ya estaba el antecedente y calcular factor de certeza (Caso 2)
      log_BH(BH, true);
      log_cc(&cc, true);
      logfile << endl;
    }
    tab -= 4;
    return verificado;
  }
  logfile << " --> true" << endl;
  log_BH(BH, true);
  return true;
}

// Método para leer la BC
int leerBC(list<Regla> *BC, char *file) {
  // Abrimos el archivo
  ifstream BCfile(file);
  if(BCfile.fail()) {
      return 1;
  }
   // Leemos línea por línea y formateamos los datos
  int nreglas;
  string line, id, rhs;
  list<string> lhs;
  char *l;
  float fc;
  getline(BCfile, line);
  nreglas = atoi(line.c_str());
  for(int i = 0; i < nreglas; i++) {
    getline(BCfile, line);                  // Ri: Si 'Antecedentes' Entonces 'Consecuente', FC='fc'
    id = strtok(line.data(),  DELIM);       // Ri
    strtok(NULL, DELIM);                    // Si
    l = strtok(NULL, DELIM);
    while(strcmp(l, "Entonces") != 0) {     // Leemos antecedentes hasta encontrar 'Entonces'
      lhs.push_back(l);
      l = strtok(NULL, DELIM);
    }
    rhs = strtok(NULL, DELIM);              // Consecuente
    strtok(NULL, DELIM);                    // FC=
    fc = atof(strtok(NULL, DELIM));         // fc
    Regla r(id, lhs, rhs, fc);              // Creamos la regla 'r'
    (*BC).push_back(r);                     // Añadimos 'r' a la BC
    lhs.clear();
  }
  // Cerramos el archivo
  BCfile.close();
  if(BCfile.fail()) {
    return 1;
  }
  return 0;
}

// Método para leer la BH
int leerBH(list<Hecho> *BH, char *file, string *goal) {
  // Abrimos el archivo
  ifstream BHfile(file);
  if(BHfile.fail()) {
      return 1;
  }
  // Leemos línea por línea y formateamos los datos
  int nhechos;
  string line, h;
  float fc;
  getline(BHfile, line);
  nhechos = atoi(line.c_str());              
  for(int i=0; i < nhechos; i++) {
    getline(BHfile, line);
    h = strtok(line.data(), DELIM);
    strtok(NULL, DELIM);              // FC=
    fc = atof(strtok(NULL, DELIM));   // 'fc'
    Hecho hecho(h, fc);
    (*BH).push_back(hecho);
  }
  getline(BHfile, line);              // Objetivo
  getline(BHfile, line);              // 'goal'
  *goal = line;
  // Cerramos el archivo
  BHfile.close();
  if(BHfile.fail()) {
    return 1;
  }
  return 0;
}

/* Función principal del programa */
int main(int argc, char *argv[])
{
  if(argc != 3) {              // Comprobamos que haya 2 argumentos
      cerr << "Número de argumentos incorrecto." << endl;
      print_help();
      return EXIT_FAILURE;
  }

  logfile.open("log.txt");      // Archivo log.txt
  if(logfile.fail()) {
    cerr << "Hubo un error al crear el archivo 'log.txt'" << endl;
    return EXIT_FAILURE;
  }

  list<Regla> BC;   // Base de Conocimiento - Lista de Reglas
  list<Hecho> BH;   // Base de Hechos - Lista de Hechos

  if(leerBC(&BC, argv[1]) != 0) {
    cerr << "Hubo un error al leer la Base de Conocimiento del fichero: " << argv[1] << endl;
    return EXIT_FAILURE;
  }
  if(leerBH(&BH, argv[2], &goal) != 0) {
    cerr << "Hubo un error al leer la Base de Hechos del fichero: " << argv[2] << endl;
    return EXIT_FAILURE;
  }

  logfile << "SBR-FC con razonamiento hacia atrás." << endl;
  logfile << "Base de Conocimiento: " << argv[1] << endl;
  logfile << "Base de Hechos: " << argv[2] << endl;
  logfile << "Objetivo: " + goal << endl;

  if (backwardChaining(goal, &BC, &BH)) {
    logfile << "Return TRUE" << endl << endl;
    logfile << "Objetivo = " + goal + ", FC = " << factorCerteza(goal, &BH) << endl;
    logfile.close();
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;

}
