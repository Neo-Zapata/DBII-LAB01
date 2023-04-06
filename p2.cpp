#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <vector>
using namespace std;

struct Alumno {
    char codigo[5];
    char nombre[11];
    char apellidos[20];
    char carrera[15];
    int ciclo;
    float mensualidad;
    int next_del; // logical position
};

void show(Alumno& rslt){
    cout << rslt.codigo << " - " << rslt.nombre << " - " << rslt.apellidos << " - " << rslt.carrera << " - " << rslt.ciclo << " - " << rslt.mensualidad << "\n";    
}

class FixedRecord{
    private:
        string filename;
        vector<int> deletedRecords;
        int numberOfLines;
        int header;
        int accesses = 0;
    public:

    void print_header(){
        int header_to_print;
        cout << "Header in FixedRecord class: " << this->header << endl;
        ifstream infile(filename, ios::binary);
        if(infile.is_open()){
            infile.seekg(0, ios::beg);
            infile.read((char*)&header_to_print, sizeof(int));
            cout << "Header in the file: " << header_to_print << endl;
        } else cerr << "Error opening file - print header\n";
        infile.close();
    }

    FixedRecord(string filename){
        this->filename = filename;
        // CREATE FILE IF IT DOES NOT EXISTS
    
        // this->header = -1;
        int size;
        // if empty, create the header, else, don't do anything
        ofstream outfile(filename, ios::binary | ios::app);
        if(outfile.is_open()){
            outfile.seekp(0, ios::end);
            size = int(outfile.tellp());
        } else cerr << "Error opening the file - Class constructor 1\n";
        outfile.close();

        if(size <= 0){
            this->header = -1;
            ofstream outfile;
            outfile.open(filename, ios::binary);
            if(outfile.is_open()){
                outfile.write((char*)&header, sizeof(header));
                this->accesses++;
            } else cerr << "Error inserting header  - Class constructor 2\n";
            outfile.close();
        } else {
            ifstream infile;
            infile.open(filename, ios::binary);
            if(infile.is_open()){
                infile.seekg(0, ios::beg); // header pos
                infile.read((char*)&header, sizeof(header));
                this->accesses++;
            } else cerr << "Error inserting header  - Class constructor 2\n";
            infile.close();
        }
        cout << "\nThere was a total of " << this->accesses << " accesses to disk (1 for writing the header if needed) - O(1)\n\n";
    }

    int getNumLines(){ // we want to know the number of record in the file (nothing more)
        ifstream infile(filename, ios::binary);
        int size;
        if(infile.is_open()){
            infile.seekg(0, ios::end);
            int nBytes = int(infile.tellg()) - 4; // - 4 to substract the header
            if(nBytes > 0){
                size = nBytes/sizeof(Alumno);
            } else size = 0;
        } else cerr << "Error reading file - getNumLines\n";
        infile.close();
        return size;
    }

    void sortDeletedRecords(){
        sort(this->deletedRecords.begin(), this->deletedRecords.end());
    }

    int readDeleted(bool flag){
        int n_deleted = 0;
        this->accesses = 0;
        int temp = this->header;
        this->deletedRecords.clear();
        this->deletedRecords.push_back(temp);
        if(temp == -1){
            cout << "\nThere is no deleted records yet\n";
            return 0;
        } else {
            ifstream infile(filename, ios::binary);
            if(infile.is_open()){
                do{ // we have the logical position, so we use seekg(temp), to move, get the record and read only the next del (assign it to temp to met the while condition)
                    Alumno record; 
                    int r = int((sizeof(Alumno)*temp) + 4);
                    infile.seekg(r, ios::beg); // + 4 to avoid re-reading the header (an int has 4 bytes)
                    infile.read((char*)&record, sizeof(Alumno));
                    this->accesses++;
                    if(flag)
                        show(record);
                    n_deleted++;
                    temp = record.next_del;
                    this->deletedRecords.push_back(temp);
                } while(temp != -1);
            } else cerr << "Error opening file - readDeleted\n";
            infile.close();
        }
        cout << "\nThere was a total of " << this->accesses << " accesses to disk to read the deleted records - O(k) where k is the number of deleted records\n\n";
        return n_deleted;
    }

    vector<Alumno> load(){
        vector<Alumno> final;
        ifstream infile;
        Alumno record;
        infile.open(filename, ios::binary);
        if(infile.is_open()){
            if(this->header == -1){
                this->accesses = 0;
                infile.seekg(0, ios::end);
                if(infile.tellg() <= 4){ // for the header
                    cout << "\nNo data in the file\n";
                    return final;
                }
                infile.seekg(0 + 4, ios::beg);
                while(infile.read((char*)&record, sizeof(Alumno))){
                    this->accesses++;
                    final.push_back(record);
                }
            } else { // IF THERE ARE DELETED RECORDS, WE SKIP THEM
                readDeleted(false);
                sortDeletedRecords();
                this->accesses = 0;
                int counter = 1; // bc 0 = header = -1
                int size = getNumLines();
                for(int i = 0 ; i < size ; i++){
                    if(i == this->deletedRecords[counter]){
                        counter++;
                        continue;
                    } else {
                        Alumno record;
                        infile.seekg((sizeof(Alumno)*i) + 4, ios::beg);
                        infile.read((char*)&record, sizeof(Alumno));
                        this->accesses++;
                        final.push_back(record);
                    }
                }
            }
        } else cerr << "Error - could not open file (load)\n";
        infile.close();
        cout << "\nThere was a total of " << this->accesses << " accesses to disk to load the data - O(n)\n\n";
        return final;
    }

    void add(Alumno record){
        cout << this->header << endl;
        if(this->header == -1){
            this->accesses = 0;
            ofstream outfile(filename, ios::binary | ios::app);
            if(outfile.is_open()){
                outfile.write((char*)&record, sizeof(Alumno));
                this->accesses++;
            } else cerr << "Error opening file - add\n";
            outfile.close();
            cout << "\nThere was a total of " << this->accesses << " accesses to disk to add the data - O(1)\n\n";
        } else { // there is at least 1 deleted, so we overwrite it
            fstream ffile(filename, ios::binary | ios::in | ios::out);
            if(ffile.is_open()){
                Alumno temp_record;
                int new_header;
                this->accesses = 0;

                // GET THE FIRST DELETED RECORD
                int r = int(((sizeof(Alumno) * this->header) + 4));
                ffile.seekg(r, ios::beg);
                ffile.read((char*)&temp_record, sizeof(Alumno));
                this->accesses++;
                new_header = temp_record.next_del;

                // OVERWRITE THE DELETED RECORD WITH THE NEW RECORD
                ffile.seekp(((sizeof(Alumno) * this->header) + 4), ios::beg);
                ffile.write((char*)&record, sizeof(Alumno));
                this->accesses++;

                // UPDATE THE HEADER
                this->header = new_header;
                ffile.seekp(0, ios::beg);
                ffile.write((char*)&header, sizeof(int));
                this->accesses++;
            } else cerr << "Error opening file - add\n";
            ffile.close();
            cout << "\nThere was a total of " << this->accesses << " accesses to disk to add the data - O(1)\n\n";
        }   
    }

    Alumno readRecord(int pos){
        Alumno record;
        int n_lines = getNumLines();
        if(pos < 0 or pos > ((n_lines-1)-(readDeleted(false)))){
            cerr << "INDEX OUT OF RANGE" << endl;
            record.codigo[0] = '\0';
            record.nombre[0] = '\0';
            record.apellidos[0] = '\0';
            record.carrera[0] = '\0';
            record.ciclo = 0;
            record.mensualidad = 0;
            record.next_del = 0;
            return record;
        }
        ifstream infile(filename, ios::binary);
        if(infile.is_open()){
            if(this->header == -1){
                this->accesses = 0;
                infile.seekg((sizeof(Alumno)*pos) + 4, ios::beg); // + 4 bc of the header
                infile.read((char*)&record, sizeof(Alumno));
                this->accesses++;
                // show(record);
                cout << "\nThere was a total of " << this->accesses << " accesses to disk to read the record - O(1)\n\n";
            } else { // there are some deleted records
                readDeleted(false);
                sortDeletedRecords();
                this->accesses = 0;
                int counter = 1; // bc 0 = header = -1
                int size = getNumLines();
                int aux = 0;
                
                // OMIT OR SKIP THE DELETED RECORDS INDEXES
                for(int i = 0 ; i < size ; i++){
                    if(i == this->deletedRecords[counter]){
                        counter++;
                        continue;
                    } else {
                        if(aux == pos){ // THERE IS A MATCH IN THE INDEX THE USER SEES AND THE LOGICAL INDEX IN THE FILE
                            Alumno record;
                            infile.seekg((sizeof(Alumno)*i) + 4, ios::beg);
                            infile.read((char*)&record, sizeof(Alumno));
                            this->accesses++;
                            infile.close();
                            return record;
                        }
                        aux++;
                    }
                }
                cout << "\nThere was a total of " << this->accesses << " accesses to disk to read the record - O(1)\n\n";
            }
        } else cerr << "Error opening file - readRecord\n";
        infile.close();

        
        return record;
    }

    bool delete_record(int pos){ // LIFO
        this->accesses = 0;
        bool flag = false;
        Alumno record;
        int n_lines = getNumLines();
        if(pos < 0 or pos > ((n_lines-1)-(readDeleted(false)))){
            cerr << "INDEX OUT OF RANGE" << endl;
            return false;
        }
        fstream ffile(filename, ios::binary | ios::in | ios::out);
        if(ffile.is_open()){
            if(this->header == -1){ // IF THERE IS NO DELETED RECORD, WE DO A NORMAL DELETE
                // GET THE RECORD TO DELETE AND UPDATE ITS NEXT_DEL (FREE LIST - LIFO)
                ffile.seekg((sizeof(Alumno)*pos) + 4, ios::beg); // + 4 bc of the header
                ffile.read((char*)&record, sizeof(Alumno));
                this->accesses++;
                record.next_del = this->header;

                // WRITE THE UPDATED RECORD IN THE SAME POSITION WE FOUND IT
                ffile.seekp((sizeof(Alumno)*pos) + 4, ios::beg);
                ffile.write((char*)&record, sizeof(Alumno));
                this->accesses++;

                // UPDATE THE HEADER IN MEMORY (IN THIS CLASS) AND IN DISK (THE FILE)
                this->header = pos;
                ffile.seekp(0, ios::beg); // header
                ffile.write((char*)&header, sizeof(int));
                this->accesses++;
            } else { // THERE IS AT LEAST 1 DELETED RECORD, SO WE SHOULD SKIP IT (EXAMPLE: IF THERE IS 10 DELETED RECORD AND THEN 1 NORMAL RECORD RESPECTIVELY, THE USER ONLY SEES 1 RECORD, SO IF THE USER WANTS TO DELETE THAT RECORD, HE WOULD USE DELETE_RECORD(0) - INDEX 0)
                readDeleted(false);
                sortDeletedRecords();
                this->accesses = 0;
                int counter = 1; // bc 0 = header = -1
                int size = getNumLines();
                int aux = 0;
                
                // IF THE RECORD IS DELETED WE OMIT IT, ELSE, WE USE AUX AS A COUNNTER (AUX VALUE WOULD BE THE INDEX FROM THE POINT OF VIEW OF THE USER AND 'i' IS THE LOGICAL POSITION)
                for(int i = 0 ; i < size ; i++){
                    if(i == this->deletedRecords[counter]){
                        counter++;
                        continue;
                    } else {
                        if(aux == pos){ // THERE IS A MATCH IN THE INDEX THE USER SEES (AUX) AND THE LOGICAL INDEX 'i'
                            // GET THE RECORD TO DELETE AND UPDATE ITS NEXT_DEL (FREE LIST - LIFO)
                            ffile.seekg((sizeof(Alumno)*i) + 4, ios::beg); // + 4 bc of the header
                            ffile.read((char*)&record, sizeof(Alumno));
                            this->accesses++;
                            record.next_del = this->header;

                            // WRITE THE UPDATED RECORD IN THE SAME POSITION WE FOUND IT
                            ffile.seekp((sizeof(Alumno)*i) + 4, ios::beg);
                            ffile.write((char*)&record, sizeof(Alumno));
                            this->accesses++;

                            // UPDATE THE HEADER IN MEMORY (IN THIS CLASS) AND IN DISK (THE FILE)
                            this->header = i;
                            ffile.seekp(0, ios::beg); // header
                            ffile.write((char*)&header, sizeof(int));
                            this->accesses++;
                            return flag;
                        }
                        aux++;
                    }
                }
                cout << "\nThere was a total of " << this->accesses << " accesses to disk to read the record - O(1)\n\n";
            }
        } else cerr << "Error opening file - delete\n";
        ffile.close();
        cout << "\nThere was a total of " << this->accesses << " accesses to disk to delete the record - O(1)\n\n";
        return flag;
    }
};

Alumno createAlumno(){
    cin.ignore();
    Alumno record;
    int ciclo;
    float mensualidad;
    cout << "\nCodigo: "; cin.getline(record.codigo, 5);
    cout << "\nnombre: "; cin.getline(record.nombre, 11);
    cout << "\napellidos: "; cin.getline(record.apellidos, 20);
    cout << "\ncarrera: "; cin.getline(record.carrera, 15);
    cout << "\nciclo: "; cin >> ciclo;
    cout << "\nmensualidad: "; cin >> mensualidad;
    record.ciclo = ciclo;
    record.mensualidad = mensualidad;
    record.next_del = 0;
    return record;
}

void Clear(){
    cout << "\x1B[2J\x1B[H";
}

void show_all(vector<Alumno> vec_alumno)
{
  for (int i = 0; i < (int)vec_alumno.size(); i++)
  {
    show(vec_alumno[i]);
  }
}

void InteractiveMenu(){
    FixedRecord FR("datos1.bin");
    int opcion;
    bool repetir = true;
    vector<Alumno> datos1;

    do {
        Clear();
        cout << "\n\n        Menu " << endl;
        cout << "--------------------"<<endl;
        cout << "1. Leer todos los registros del archivo binario" << endl;
        cout << "2. Agregar un nuevo registro al archivo binario" << endl;
        cout << "3. Obtener el registro de la posicion x" << endl;
        cout << "4. Eliminar el registro de la posicion x" << endl;
        cout << "5. Leer los registros eliminados" << endl;
        cout << "6. Leer los headers" << endl;
        cout << "0. End" << endl;

        cout << "\nEnter an option: ";
        cin >> opcion;

        while(opcion != 0 and opcion != 1 and opcion != 2 and opcion != 3 and opcion != 4 and opcion != 5 and opcion != 6){
            cerr << endl << "Enter a valid option";
            cout << "\nEnter an option: ";
            cin >> opcion;
        }

        switch (opcion) {
        case 1:{
            int V;
            datos1 = FR.load();
            show_all(datos1);
            do{
                cout << endl << "Ingrese 0 para salir: ";
                cin >> V;
                Clear();
            }while(V!=0);
        }
        break;
        case 2:{
            int V;
            Alumno a1;
            a1 = createAlumno();
            FR.add(a1);
            do{
                cout << endl << "Ingrese 0 para salir: ";
                cin >> V;
                Clear();
            }while(V!=0);
        }
        break;
        case 3:{
            int V;
            Alumno a2;
            int pos1;
            cout << "\nSelect a position (starting from 0): ";
            cin >> pos1;
            a2 = FR.readRecord(pos1);
            show(a2);
            do{
                cout << endl << "Ingrese 0 para salir: ";
                cin >> V;
                Clear();
            }while(V!=0);
        }
        break;
        case 4:{
            int V;
            int pos2;
            cout << "\nSelect a position (starting from 0): ";
            cin >> pos2;
            FR.delete_record(pos2);
            do{
                cout << endl << "Ingrese 0 para salir: ";
                cin >> V;
                Clear();
            }while(V!=0);
        }
        break;
        case 5:{
            int V;
            FR.readDeleted(true);
            do{
                cout << endl << "Ingrese 0 para salir: ";
                cin >> V;
                Clear();
            }while(V!=0);
        }
        break;
        case 6:{
            int V;
            FR.print_header();
            do{
                cout << endl << "Ingrese 0 para salir: ";
                cin >> V;
                Clear();
            }while(V!=0);
        }
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

int main(){
    InteractiveMenu();
}