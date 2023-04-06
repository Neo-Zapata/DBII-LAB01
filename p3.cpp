#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
using namespace std;

//////////// STRUCT ALUMNO AND FUCTIONS RELATED TO THAT ///////////
struct Alumno
{
  string Nombre;
  string Apellidos;
  string Carrera;
  float mensualidad;
};

Alumno create_alumno()
{
  cin.ignore();
  Alumno result;
  string mensualidad;

  cout << "\nNombre: ";
  getline(std::cin, result.Nombre);

  cout << "\nApellidos: ";
  getline(std::cin, result.Apellidos);

  cout << "\nCarrera: ";
  getline(std::cin, result.Carrera);

  cout << "\nMensualidad: ";
  getline(std::cin, mensualidad);
  result.mensualidad = stof(mensualidad);
  return result;
}

void show_alumno(Alumno al)
{
  cout << al.Nombre << "|" << al.Apellidos << "|" << al.Carrera << "|" << al.mensualidad << '\n';
}

void show_all(vector<Alumno> vec_alumno)
{
  for (int i = 0; i < (int)vec_alumno.size(); i++)
  {
    show_alumno(vec_alumno[i]);
  }
}

///////////////////////////////////////////////////////////

class VariableRecord
{
  string filename;
  int accesses;
  int n_lines = 100; // an asumption
public:
  VariableRecord(string _name);
  string getfilename();
  vector<Alumno> load();
  void add(Alumno record);
  Alumno readRecord(int pos);
  int getNumLines();
};

VariableRecord::VariableRecord(string _name)
{
  filename = _name;
};

string VariableRecord::getfilename()
{
  return filename;
}

vector<Alumno> VariableRecord::load()
{
  this->n_lines = 0;
  this->accesses = 0;
  ifstream readfile;
  readfile.open(filename);
  if (!readfile.is_open())
  {
    cerr << "Error" << endl;
    exit(1);
  }

  vector<Alumno> result;

  Alumno tmp;
  string mensualidad;

  readfile.ignore(1000, '\n'); // Aumentar o suguerir un nuero razonable // ignore 1000 char or stop if a \n is found

  getline(readfile, tmp.Nombre, '|');
  this->accesses++;
  getline(readfile, tmp.Apellidos, '|');
  this->accesses++;
  getline(readfile, tmp.Carrera, '|');
  this->accesses++;
  getline(readfile, mensualidad, '\n');
  this->n_lines++;
  this->accesses++;
  tmp.mensualidad = stof(mensualidad);
  while (!readfile.eof())
  {
    result.push_back(tmp);
    getline(readfile, tmp.Nombre, '|');
    this->accesses++;
    getline(readfile, tmp.Apellidos, '|');
    this->accesses++;
    getline(readfile, tmp.Carrera, '|');
    this->accesses++;
    getline(readfile, mensualidad, '\n');
    this->n_lines++;
    this->accesses++;
    tmp.mensualidad = stof(mensualidad);
  }

  result.push_back(tmp);
  readfile.close();
  cout << "\nThere was a total of " << this->accesses << " accesses to disk - O(n)\n\n";
  return result;
}

void VariableRecord::add(Alumno record)
{
  this->accesses = 0;
  ofstream writefile;
  writefile.open(filename, ios::app);
  if (!writefile.is_open())
  {
    cerr << "Error" << endl;
    exit(1);
  }

  string mensualidad;

  mensualidad = to_string(record.mensualidad);

  writefile << "\n" << record.Nombre << "|" << record.Apellidos << "|" << record.Carrera << "|" << mensualidad;
  this->accesses += 8; // <?>

  writefile.close();
  cout << "\nThere was a total of " << this->accesses << " accesses to disk - O(1)\n\n";
  this->n_lines++;
}
Alumno VariableRecord::readRecord(int pos)
{
  Alumno result;
  if(pos < 0 or pos > this->n_lines-1){
    cerr << "INDEX OUT OF RANGE" << endl;
    result.Nombre = "";
    result.Apellidos = "";
    result.Carrera = "";
    result.mensualidad = 0;
    return result;
  }
  this->accesses = 0;
  ifstream readfile;
  readfile.open(filename);
  if (!readfile.is_open())
  {
    cerr << "Error" << endl;
    exit(1);
  }
  
  string mensualidad;

  readfile.ignore(1000, '\n'); // Agregar y justificar un numero razonable

  for (int i = 0; i < pos; i++)
  {
    readfile.ignore(1000, '\n'); // Agregar y justificar un numero razonable
  }

  getline(readfile, result.Nombre, '|');
  this->accesses++;
  getline(readfile, result.Apellidos, '|');
  this->accesses++;
  getline(readfile, result.Carrera, '|');
  this->accesses++;
  getline(readfile, mensualidad, '\n');
  this->accesses++;
  result.mensualidad = stof(mensualidad);
  cout << "\nThere was a total of " << this->accesses << " accesses to disk - O(1)\n\n";
  return result;
}

void Clear(){
    cout << "\x1B[2J\x1B[H";
}

void Interactive_Menu(){
  VariableRecord prueba("datos2.txt");
  int opcion;
  bool repetir = true;
  vector<Alumno> datos1;
  // int pos;

  do {
      Clear();
      cout << "\n\n        Menu " << endl;
      cout << "--------------------"<<endl;
      cout << "1. Leer todos los registros del archivo" << endl;
      cout << "2. Agregar un nuevo registro al archivo" << endl;
      cout << "3. Obtener el registro de la posicion x" << endl;
      cout << "0. End" << endl;

      cout << "\nEnter an option: ";
      cin >> opcion;

      while(opcion != 0 and opcion != 1 and opcion != 2 and opcion != 3){
          cerr << endl << "Enter a valid option";
          cout << "\nEnter an option: ";
          cin >> opcion;
      }

      int V;

      switch (opcion) {
        case 1:{
          datos1 = prueba.load();
          show_all(datos1);
          do{
            cout << endl << "Ingrese 0 para salir: ";
            cin >> V;
            Clear();
          }while(V!=0);
        }break;
        case 2:{
          Alumno a1;
          a1 = create_alumno();
          prueba.add(a1);
          do{
            cout << endl << "Ingrese 0 para salir: ";
            cin >> V;
            Clear();
          }while(V!=0);
        }break;
        case 3:{
          Alumno a2;
          int pos;
          cout << "Select a position (starting from 0): ";
          cin >> pos;
          a2 = prueba.readRecord(pos);
          show_alumno(a2);
          do{
            cout << endl << "Ingrese 0 para salir: ";
            cin >> V;
            Clear();
          }while(V!=0);
        }break;
        case 0:{
          repetir = false;
        }break;
        default:{
          cout << "Invalid option." << endl;
        }break;
      }

  } while (repetir);
}

int main()
{
  Interactive_Menu();
}