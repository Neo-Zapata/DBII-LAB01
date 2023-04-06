#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
using namespace std;
// get the complexity (number of accesses to disk)
void Clear();

struct Alumno
{
  char codigo[5];
  char nombre[11];
  char apellidos[20];
  char carrera[15];
};

void fill_spaces(char *&arr)
{
  int length = std::strlen(arr); // Reads a strigns until a null character
  // cout << length << endl;
  // Fill remaining spaces with spaces
  for (int i = length; i < 21; i++)
  {
    arr[i] = ' ';
  }
}

Alumno create_alumno()
{
  char max_atrribute[21];
  char *char_ptr =
      max_atrribute; // Useful because I tried to pass by reference an arr
  Alumno result;

  cout << "\nCodigo: ";
  cin.getline(max_atrribute, 5); // 5 because add null character
  fill_spaces(char_ptr);
  strncpy(result.codigo, max_atrribute, 4);
  result.codigo[4] = '\0';

  cout << "\nNombre: ";
  cin.getline(max_atrribute, 11);
  fill_spaces(char_ptr);
  strncpy(result.nombre, max_atrribute, 10);
  result.nombre[10] = '\0';

  cout << "\nApellidos: ";
  cin.getline(max_atrribute, 20);
  fill_spaces(char_ptr);
  strncpy(result.apellidos, max_atrribute, 19);
  result.apellidos[19] = '\0';

  cout << "\nCarrera: ";
  cin.getline(max_atrribute, 15);
  fill_spaces(char_ptr);
  strncpy(result.carrera, max_atrribute, 14);
  result.carrera[14] = '\0';
  return result;
}

void show_alumno(Alumno al)
{
  cout << al.codigo << " " << al.nombre << " " << al.apellidos << " " << al.carrera << " " << endl;
}

void show_all(vector<Alumno> vec_alumno)
{
  for (int i = 0; i < (int)vec_alumno.size(); i++)
  {
    show_alumno(vec_alumno[i]);
  }
}

class FixedRecord
{
  string filename;
  int accesses;

public:
  FixedRecord(string _name);
  string getfilename();
  vector<Alumno> load();
  void add(Alumno record);
  Alumno readRecord(int pos);
  int getNumLines();
};

FixedRecord::FixedRecord(string _name)
{
  filename = _name;
};

string FixedRecord::getfilename()
{
  return filename;
}

int FixedRecord::getNumLines(){ // we want to know the number of record in the file (nothing more)
    ifstream infile(filename);
    int size;
    if(infile.is_open()){
        infile.seekg(0, ios::end);
        int nBytes = int(infile.tellg());
        if(nBytes > 0){
            size = nBytes/sizeof(Alumno);
        } else size = 0;
    } else cerr << "Error reading file - getNumLines\n";
    infile.close();
    return size;
}

vector<Alumno> FixedRecord::load()
{
  this->accesses = 0;
  ifstream infile;
  vector<Alumno> final;
  infile.open(filename);
  if (infile.is_open())
  {
    Alumno rslt;
    string line;
    while (!infile.eof())
    {
      this->accesses++;
      infile.read((char *)&rslt, sizeof(Alumno));
      rslt.codigo[4] = '\0';
      rslt.nombre[10] = '\0';
      rslt.apellidos[19] = '\0';
      rslt.carrera[14] = '\0';
      infile.ignore(2, '\n');
      final.push_back(rslt);
    }
    infile.close();
  }
  else
    cerr << "Error - could not open file (load)\n";
  cout << "\nThere was a total of " << this->accesses << " accesses to disk\n\n";
  return final;
}

void FixedRecord::add(Alumno record)
{
  this->accesses = 0;
  ofstream writefile;
  writefile.open(filename, ios::app);
  if (!writefile.is_open())
  {
    cerr << "Error" << endl;
    exit(1);
  }
  record.codigo[4] = ' ';
  record.nombre[10] = ' ';
  record.apellidos[19] = ' ';
  record.carrera[14] = ' ';
  writefile << "\r\n";
  writefile.write((char *)&record, sizeof(Alumno)); // O(1)
  this->accesses += 1;
  writefile.close();
  cout << "\nThere was a total of " << this->accesses << " accesses to disk\n\n";
}

Alumno FixedRecord::readRecord(int pos)
{
  Alumno result;
  int n_lines = getNumLines();
  if(pos < 0 or pos > n_lines-1){
    cerr << "INDEX OUT OF RANGE" << endl;
    result.codigo[0] = '\0';
    result.nombre[0] = '\0';
    result.apellidos[0] = '\0';
    result.carrera[0] = '\0';
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
  readfile.seekg(
      pos * sizeof(Alumno) + pos * 2,
      ios::beg); // I need sum pos*2 because \r\n are "invisible character"
  this->accesses += 1;
  readfile.read((char *)&result, sizeof(Alumno));
  result.codigo[4] = '\0';
  result.nombre[10] = '\0';
  result.apellidos[19] = '\0';
  result.carrera[14] = '\0';
  readfile.close();
  cout << "\nThere was a total of " << this->accesses << " accesses to disk\n\n";
  return result;
}

void show(Alumno &rslt)
{
  cout << rslt.codigo << " - " << rslt.nombre << " - " << rslt.apellidos << " - " << rslt.carrera << "\n";
}

void Clear()
{
  cout << "\x1B[2J\x1B[H";
}

void Interactive_Menu()
{
  FixedRecord prueba("datos1.txt");
  int opcion;
  bool repetir = true;
  vector<Alumno> datos1;
  // int pos;

  do
  {
    Clear();
    cout << "\n\n        Menu " << endl;
    cout << "--------------------" << endl;
    cout << "1. Leer todos los registros del archivo" << endl;
    cout << "2. Agregar un nuevo registro al archivo" << endl;
    cout << "3. Obtener el registro de la posicion x" << endl;
    cout << "0. End" << endl;

    cout << "\nEnter an option: ";
    cin >> opcion;
    cin.ignore();

    while (opcion != 0 and opcion != 1 and opcion != 2 and opcion != 3)
    {
      cerr << endl
           << "Enter a valid option";
      cout << "\nEnter an option: ";
      cin >> opcion;
      cin.ignore();
    }

    switch (opcion)
    {
    case 1:
      int V;
      datos1 = prueba.load();
      show_all(datos1);
      do
      {
        cout << endl
             << "Ingrese 0 para salir: ";
        cin >> V;
        Clear();
      } while (V != 0);
      break;
    case 2:
      Alumno a1;
      a1 = create_alumno();
      prueba.add(a1);
      do
      {
        cout << endl
             << "Ingrese 0 para salir: ";
        cin >> V;
        Clear();
      } while (V != 0);
      break;
    case 3:
      Alumno a2;
      int pos;
      cout << "Select a position (starting from 0): ";
      cin >> pos;
      a2 = prueba.readRecord(pos);
      show_alumno(a2);
      do
      {
        cout << endl
             << "Ingrese 0 para salir: ";
        cin >> V;
        Clear();
      } while (V != 0);
      break;
    case 0:
      repetir = false;
      break;
    default:
      cout << "Invalid option." << endl;
      break;
    }

  } while (repetir);
}

int main()
{
  Interactive_Menu();
}