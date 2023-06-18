#include <stdlib.h>
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>

#define DELIM " ,:="

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

    void imprimir() {
        cout << this->h << " ,FC=" << this->fc << endl;
      }
};

bool verificar(string Meta, list<Regla> BH) {
  list<Regla>::iterator it;
  return false;
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
int leerBH(list<Hecho> *BH, char *file, string *objetivo) {
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
    fc = atof(strtok(NULL, DELIM));
    Hecho hecho(h, fc);
    (*BH).push_back(hecho);
  }
  getline(BHfile, line);
  getline(BHfile, line);
  *objetivo = line;
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

  ofstream log("log.txt");      // Archivo log.txt
  if(log.fail()) {
    cerr << "Hubo un error al crear el archivo 'log.txt'" << endl;
    return EXIT_FAILURE;
  }

  list<Regla> BC;   // Base de Conocimiento - Lista de Reglas
  list<Hecho> BH;   // Base de Hechos - Lista de Hechos
  string objetivo;

  if(leerBC(&BC, argv[1]) != 0) {
    cerr << "Hubo un error al leer la Base de Conocimiento del fichero: " << argv[1] << endl;
    return -1;
  }
  if(leerBH(&BH, argv[2], &objetivo) != 0) {
    cerr << "Hubo un error al leer la Base de Hechos del fichero: " << argv[2] << endl;
    return -1;
  }

  

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

  return EXIT_SUCCESS;
}
