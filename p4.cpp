#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
using namespace std;

struct Matricula
{
  int record_size;        // 4
  int codigo_size;        // 4
  string codigo;          // 2 x
  int ciclo;              // 4 x
  float mensualidad;      // 8 x
  int observaciones_size; // 4
  string observaciones;   // 2
};

Matricula create_matricula()
{
  cin.ignore();
  Matricula result;
  string ciclo;
  string mensualidad;
  int cont_size = 0;

  cout << "\nCodigo: ";
  getline(std::cin, result.codigo);
  result.codigo_size = result.codigo.size();
  cont_size += result.codigo.size();

  cout << "\nCiclo: ";
  getline(std::cin, ciclo);
  result.ciclo = stoi(ciclo);
  cont_size += sizeof(int);

  cout << "\nMensualidad: ";
  getline(std::cin, mensualidad);
  result.mensualidad = stof(mensualidad);
  cont_size += sizeof(float);

  cout << "\nObservaciones: ";
  getline(std::cin, result.observaciones);
  result.observaciones_size = result.observaciones.size();
  cont_size += result.observaciones.size();

  cont_size += (sizeof(int) * 3); // adding "codigo_size" and "observaciones_size" and record_size

  result.record_size = cont_size;

  return result;
}

void show_matricula(Matricula m1)
{
  cout << m1.codigo << "|" << m1.ciclo << "|" << m1.mensualidad << "|" << m1.observaciones << '\n';
}

void show_all(vector<Matricula> vec_m1)
{
  for (int i = 0; i < (int)vec_m1.size(); i++)
  {
    show_matricula(vec_m1[i]);
  }
}

void encapsular(const Matricula &m1, std::ofstream &file)
{
  file.write((char *)(&m1.record_size), sizeof(m1.record_size));
  file.write((char *)(&m1.codigo_size), sizeof(m1.codigo_size));
  file.write(m1.codigo.c_str(), m1.codigo_size);
  file.write((char *)(&m1.ciclo), sizeof(m1.ciclo));
  file.write((char *)(&m1.mensualidad), sizeof(m1.mensualidad));
  file.write((char *)(&m1.observaciones_size), sizeof(m1.observaciones_size));
  file.write(m1.observaciones.c_str(), m1.observaciones_size);
}

Matricula desencapsular(std::ifstream &file)
{

  Matricula m1;

  file.read((char *)(&m1.record_size), sizeof(m1.record_size));
  file.read((char *)(&m1.codigo_size), sizeof(m1.codigo_size));

  char *codigo = new char[m1.record_size + 1];
  file.read(codigo, m1.codigo_size);
  codigo[m1.record_size] = '\0';
  m1.codigo = codigo;
  delete[] codigo;

  file.read((char *)(&m1.ciclo), sizeof(m1.ciclo));
  file.read((char *)(&m1.mensualidad), sizeof(m1.mensualidad));
  file.read((char *)(&m1.observaciones_size), sizeof(m1.observaciones_size));

  char *observaciones = new char[m1.observaciones_size + 1];
  file.read(observaciones, m1.observaciones_size);
  observaciones[m1.observaciones_size] = '\0';
  m1.observaciones = observaciones;
  delete[] observaciones;

  return m1;
}
///////////////////////////////////////////////////////////

class VariableRecord
{
  string filename;
  int number_of_records;

public:
  VariableRecord(string _name);
  string getfilename();
  vector<Matricula> load();
  void add(Matricula record);
  Matricula readRecord(int pos);
};

VariableRecord::VariableRecord(string _name)
{
  filename = _name;

  ofstream metadata("metadataFile.bin", ios::app | ios::binary);
  metadata.close();

  ofstream data(filename, ios::app | ios::binary);
  if(data.is_open()){
    load();
  } else cerr << "error opening file - constructor\n";
  data.close();
};

string VariableRecord::getfilename()
{
  return filename;
}

void VariableRecord::add(Matricula record)
{
  fstream metadata("metadataFile.bin", ios::app | ios::binary);
  if (!metadata.is_open())
  {
    cerr << "Error al abrir el archivo de metadata gaa" << endl;
    exit(1);
  }
  ofstream data(filename, ios::app | ios::binary);
  if (!data.is_open())
  {
    cerr << "Error al abrir el archivo de registros" << endl;
    exit(1);
  }

  //// Metadata operations ///////////////
  int position = data.tellp();
  // cout << position;
  metadata.write((char *)&position, sizeof(position));
  metadata.close();

  ///////////// Add new record //////////////////////////////
  encapsular(record, data);
  this->number_of_records++;
}

vector<Matricula> VariableRecord::load()
{
  vector<Matricula> result;
  fstream metadata("metadataFile.bin", ios::in | ios::binary);
  if (!metadata.is_open())
  {
    cerr << "Error al abrir el archivo de metadata load" << endl;
    exit(1);
  }
  ifstream data(filename, ios::binary);
  if (!data.is_open())
  {
    cerr << "Error al abrir el archivo de registros" << endl;
    exit(1);
  }

  data.seekg(0, ios::end);
  if (data.tellg() == 0){
    this->number_of_records = 0;
    cout << "\nThere are no records in the file\n";
    return result;
  }

  
  this->number_of_records = 0;

  int real_pos;
  Matricula tmp_matricula;
  while (!metadata.eof())
  {
    metadata.read((char *)&real_pos, sizeof(real_pos));

    //////////// go to exact record ///////////////////////
    data.seekg(real_pos);

    /// Reading all data members in Matricula ///////////
    tmp_matricula = desencapsular(data);

    if (!metadata.eof()) // Prevent extra add in the end of the file (extra end record)
    {
      this->number_of_records++;
      result.push_back(tmp_matricula);
    }
  }

  metadata.close();
  data.close();

  return result;
}

Matricula VariableRecord::readRecord(int pos)
{
  Matricula result;
  // cout << this->number_of_records << endl;
  if(pos < 0 or pos > this->number_of_records-1 or this->number_of_records==0){
    cout << "\nIndex out of range\n";
    result.record_size = 0;
    result.codigo_size = 0;
    result.codigo = "";
    result.ciclo = 0;
    result.mensualidad = 0;
    result.observaciones_size = 0;
    result.observaciones = "";
    return result;
  }
  fstream metadata("metadataFile.bin", ios::in | ios::binary);
  if (!metadata.is_open())
  {
    cerr << "Error al abrir el archivo de metadata read" << endl;
    exit(1);
  }
  ifstream data(filename, ios::binary);
  if (!data.is_open())
  {
    cerr << "Error al abrir el archivo de registros" << endl;
    exit(1);
  }



  int real_pos;
  metadata.seekg(pos * sizeof(real_pos));
  metadata.read((char *)&real_pos, sizeof(real_pos));

  //////////// go to exact record ///////////////////////
  data.seekg(real_pos);

  /// Reading all data members in Matricula ///////////
  result = desencapsular(data);

  return result;
}

void Clear(){
    cout << "\x1B[2J\x1B[H";
}

void Interactive_Menu(){
  VariableRecord prueba("datos3.bin");
  int opcion;
  bool repetir = true;
  vector<Matricula> datos1;
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
          Matricula a1;
          a1 = create_matricula();
          prueba.add(a1);
          do{
            cout << endl << "Ingrese 0 para salir: ";
            cin >> V;
            Clear();
          }while(V!=0);
        }break;
        case 3:{
          Matricula a2;
          int pos;
          cout << "Select a position (starting from 0): ";
          cin >> pos;
          a2 = prueba.readRecord(pos);
          show_matricula(a2);
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
