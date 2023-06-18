#include <stdlib.h>
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>

#define DELIM " ,:="
#define OPERATIONS "yo"

using namespace std;

/* Clase "Regla" que representa una regla: 'id': Si 'lhs' Entonces 'rhs', FC='fc' */
class Regla {
  private:
      string id;          // Identificador de la regla
      string lhs;         // Antecedente
      string rhs;         // Consecuente
      float fc;           // Factor de certeza
  public:
      Regla(string id, string lhs, string rhs, float fc) {    // Constructor
          this->id = id;
          this->lhs = lhs;
          this->rhs = rhs;
          this->fc = fc;
      }

      string getId() {
        return this->id;
      }

      string getLhs() {
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
        char *a = new char[this->lhs.length()+1];
        strcpy(a, this->lhs.c_str());
        a = strtok(a, OPERATIONS);
        while(a != NULL) {
          antecedentes.push_back(a);
          a = strtok(NULL, OPERATIONS);
        }
        return antecedentes;
      }

      void imprimir() {
        cout << this->id << ": " << this->lhs << " -> " << this->rhs << " ,FC=" << this->fc << endl;
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
float factorCerteza(const char a, list<Hecho> *BH) {
  list<Hecho>::iterator it = BH->begin();
  float fc;
  while(it != BH->end()) {
    if(*((*it).getHecho().c_str()) == a) {
      fc = (*it).getFc();
      break;
    }
    it++;
  }
  return fc;
}

/* Combinación de antecedentes y encadenamiento de evidencia */
float combinarFC(Regla r, list<Regla> *BC, list<Hecho> *BH) {
  const char *a = r.getLhs().c_str();
  float fc = factorCerteza(*a, BH);
  for(int i = 1; i < r.getLhs().length(); i++) {
    if (a[i] == 'o') {
      i++;
      fc = max(fc, factorCerteza(*(a+i), BH));
    } else if (a[i] == 'y') {
      i++;
      fc = max(fc, factorCerteza(*(a+i), BH));
    }
  }
  fc = fc*r.getFc();
  return fc;
}

/* Añade nueva evidencia a la Base de Hechos y resuelve los conflictos, si los hay. */
void nuevaEvidencia(string meta, float fc1, list<Hecho> *BH) {
  list<Hecho>::iterator it = BH->begin();
  while(it != BH->end()) {
    if ((*it).getHecho().compare(meta) == 0) {
      float fc2 = (*it).getFc();
      if (fc1 >= 0 && fc2 >= 0) {
        (*it).setFc(fc1 + fc2*(1+fc1));
      } else if (fc1 <= 0 && fc2 <= 0) {
        (*it).setFc(fc1 + fc2*(1-fc1));
      } else {
        (*it).setFc((fc1 + fc2)/(1 - min(abs(fc1), abs(fc2))));
      }
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
    list<Regla> cc = equiparar(BC, meta);
    while(!cc.empty() && !verificado) {
      Regla r = resolver(&cc);
      list<string> nuevasMetas = r.getAntecedentes();
      verificado = true;
      while(!nuevasMetas.empty() && verificado) {
        string nuevaMeta = seleccionarMeta(&nuevasMetas);
        verificado = backwardChaining(nuevaMeta, BC, BH);
      }
      if (verificado) {
        float fc = combinarFC(r, BC, BH); // Factor de certeza a partir de los antecedentes (Caso 1) y de la regla (Caso 3)
        nuevaEvidencia(meta, fc, BH);   // Comprobar si ya estaba el antecedente y calcular factor de certeza (Caso 2)
      }
    return verificado;
    }
  }
  return true;
}

/* Imprime el uso del programa */
void print_help() {
  cerr << "Uso: .\\SBR-FC BC-filename BH-filename" << endl;
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
  string line, id, lhs, rhs;
  char *l;
  float fc;
  getline(BCfile, line);
  nreglas = atoi(line.c_str());
  for(int i = 0; i < nreglas; i++) {
    getline(BCfile, line);                  // Ri: Si 'Antecedentes' Entonces 'Consecuente', FC='fc'
    id = strtok(line.data(),  DELIM);       // Ri
    strtok(NULL, DELIM);                    // Si
    l = strtok(NULL, DELIM);
    lhs = "";
    while(strcmp(l, "Entonces") != 0) {     // Leemos antecedentes hasta encontrar 'Entonces'
      lhs = lhs + l;
      l = strtok(NULL, DELIM);
    }
    rhs = strtok(NULL, DELIM);              // Consecuente
    strtok(NULL, DELIM);                    // FC=
    fc = atof(strtok(NULL, DELIM));         // fc
    Regla r(id, lhs, rhs, fc);              // Creamos la regla 'r'
    (*BC).push_back(r);                     // Añadimos 'r' a la BC
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
  cout << "Iniciando...\n";
  if(argc != 3) {              // Comprobamos que haya 2 argumentos
      cerr << "Número de argumentos incorrecto." << endl;
      print_help();
      return EXIT_FAILURE;
  }

  ofstream log("log.txt");      // Archivo log.txt
  if(log.fail()) {
    cerr << "Hubo un error al crear el archivo 'log.txt'" << endl;
    return EXIT_FAILURE;
  }

  list<Regla> BC;   // Base de Conocimiento - Lista de Reglas
  list<Hecho> BH;   // Base de Hechos - Lista de Hechos
  string goal;

  if(leerBC(&BC, argv[1]) != 0) {
    cerr << "Hubo un error al leer la Base de Conocimiento del fichero: " << argv[1] << endl;
    return EXIT_FAILURE;
  }
  if(leerBH(&BH, argv[2], &goal) != 0) {
    cerr << "Hubo un error al leer la Base de Hechos del fichero: " << argv[2] << endl;
    return EXIT_FAILURE;
  }

  if (backwardChaining(goal, &BC, &BH)) {
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;

  /*
  cout << "Base de Conocimiento: " << argv[1] << endl;
  list<Regla>::iterator itBC = BC.begin();
  while(itBC != BC.end()) {
    (*itBC).imprimir();
    itBC++;
  }
  cout << endl;
  cout << "Base de Hechos: " << argv[2] << endl;
  list<Hecho>::iterator itBH = BH.begin();
  while(itBH != BH.end()) {
    (*itBH).imprimir();
    itBH++;
  }
  */
}
