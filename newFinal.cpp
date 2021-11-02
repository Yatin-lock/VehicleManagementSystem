#include<bits/stdc++.h>
using namespace std;

//    Delimeter for parsing dates Dates are always given in d/m/yyyy format
const char DATEDELIMETER = '/';

//    Delimeter for parsing The Entites data the data in the file
//    is written seperated by this Delimeter
const char DELIMETER = ';';

// Abstract class that is the parent of entity class
// it provides a toString method which have different implementation for every junior class
class Issuable 
{
    public:
    virtual string toString() const = 0 ;
};

//Entity class signifies all the storable entities in the system namely vehicle, trips, users.
class Entity: public Issuable{
protected:
    //recordID of that particular Entity
    long recordId;
public:
    // parameterized constructor for setting the recordId
    Entity(long recordId){
        this->recordId = recordId;
    }

    //accessor function for getting the recordId
    long getRecord() const{
        return this->recordId;
    }

    // virtual method that helps setting data from a desired entity to the current entity.
    // Each entity have its own implementation of setDataFrom method
    virtual void setDataFrom(Entity *s) = 0;

    // setting the templated class Table to be the friend of Entity class
    // Table can now access all methods of Entity class.
    template<typename T> friend class Table;

    // setting the Database class to be the friend of Entity class
    // Table can now access all methods of Entity class.
    friend class Database;
};

//Error class for defining custom Exceptions
class Error
{
private:
    // message to be displayed.
    string message;

public:
    //parametrized constructor for setting message
    Error(string message)
    {
        this->message = message;
    }

    //accessor function to access the message
    string getMessage() const
    {
        return this->message;
    }
};

//Child class of Error 
//It displays Errors pertaining which no Record is found in txt bases Database
class RecordNotFoundError : public Error
{
    public:
    //Default constructor which sets the message by calling parent constructor
    RecordNotFoundError() : Error ("No such record found") {} ;
};

//Signifies Input Output Error.
class IOError : public Error
{
    public:
    //Default constructor which sets the message by calling parent constructor
    IOError() : Error ("I/O Error could not open or process file make sure 'vehicle.txt', 'trips.txt, and 'users.txt' files exist in this directory") {};
};

//Signifies Memory Errors in programm
class MemoryError : public Error 
{
    public:
    //Default constructor which sets the message by calling parent constructor
    MemoryError() : Error ("System out of memory") {};
};

//A helper method which helps spliting the string given a delimeter
//Splits the string based of a given delimeter and returns the splited string as a vector of strings.
vector <string> split (const string &s, char delimiter)
{
    vector <string> tokens ;
    string token ;
    istringstream tokenStream (s) ;

    while (getline (tokenStream , token , delimiter) )
    {
        tokens.push_back (token);
    }

    return tokens ;
}

//Child class of class Issuable
// it contains information for the date of trips and expiration
class Date: public Issuable{
    private:
        tm date;
        bool empty;
    public: 
        Date(string date);
        Date();
        bool operator>(Date date) const;
        bool operator<(Date date) const;
        bool operator<=(Date date) const;
        bool operator>=(Date date) const;
        bool isEmpty() const;
        string toString() const;
};

typedef enum { bike = 1, car = 2, bus = 3 } VehicleType;

class Vehicle : public Entity {
    string registrationNumber;
    VehicleType type;
    int seats;
    string companyName;
    double pricePerKm;
    Date PUCExpirationDate;
public:
    Vehicle(
            string registrationNumber, 
            VehicleType type,
            int seats,
            string companyName,
            double pricePerKm,
            Date PUCExpirationDate,
            long recordId=0
            );
    string getRegistrationNumber() const;
    VehicleType getVehicleType() const;
    string getVehicleTypeName() const;
    int getSeats() const;
    string getCompanyName() const;
    double getPricePerKm() const;
    Date getPUCExpirationDate() const;
    void setPricePerKm(double newPrice);
    void display() const;
    string toString() const;
    void setDataFrom(Entity *s);
};

class User: public Entity{
    string name;
    string contact;
    string email;
public:
    User(string name, string contact,string email, long recordId=0);
    string getName() const;
    string getContact() const;
    string getEmail() const;
    void setName(string);
    void setContact(string);
    void setEmail(string);
    void display() const;
    string toString() const;
    void setDataFrom(Entity *s);
};

class Trip : public Entity 
{
    private:
    const Vehicle *vehicle ;
    const User *user ;
    Date startDate;
    Date endDate ;
    long startReading ;
    long endReading ;
    double fare ;
    bool completed ;

    public:
    Trip(const Vehicle*vehicle, const User *user, Date startDate, Date endDate, long recordId=0, long startReading = 0, long endReading =0 , double fare = 0.0 , bool isCompleted = false );
    const User & getUser () const ;
    const Vehicle & getVehicle () const ;
    Date getStartDate () const ;
    Date getEndDate () const ;
    long getStartReading () const ;
    long getEndReading() const;
    long getFare () const ;
    void startTrip (long startReading) ;
    double completeTrip (long endReading);
    void display () const ;
    string toString() const ;
    bool isCompleted() const;
    void setDataFrom (Entity *s);
};

template<typename T>
class Table{

    string fileName;
    fstream fileStream;
    vector<T*> records;

    T *getReferenceOfRecordForId(long recordId) const throw (RecordNotFoundError);
    void writeToFile() throw (IOError);
    const T* const addNewRecord(T data) throw (MemoryError, IOError);
    void updateRecord(T updatedRecord) throw (IOError, RecordNotFoundError);
public:
    Table(string filename) throw (MemoryError);
    long getNextRecordId() const;
    const T *const  getRecordForId(long recordId) const throw (RecordNotFoundError);
    friend class Database;
};

class Database
{
private:
    Table<Vehicle> *vehicleTable;
    Table<User> *userTable;
    Table<Trip> *tripTable;

    void fetchAllVehicles() throw(IOError, MemoryError);
    void fetchAllUsers() throw(IOError, MemoryError);
    void fetchAllTrips() throw(IOError, MemoryError);

    void cleanUp();

public:
    Database() throw(MemoryError, IOError);

    ~Database();

    const Table<Vehicle> *const getVehicleRef() const;
    const Table<User> *const getUserRef() const;
    const Table<Trip> *const getTripRef() const;

    const Vehicle *const getVehicle(string registrationNo) const throw(RecordNotFoundError);
    const User *const getUser(string contactNo) const throw(RecordNotFoundError);
    const vector<const Vehicle *> getVehicle(Date startDate, Date endDate, VehicleType type) const;

    template <class T>
    void addNewRecord(T *record) throw(IOError, MemoryError);
    template <class T>
    void updateRecord(T *record) throw(IOError, RecordNotFoundError);
};

class Application{

    Database *db;
    void renderMenu();
    void welcome();
    void gotoXY(int x, int y) const;
    void renderAddNewVehicleMenu() const;
    void renderViewVehicleMenu() const;
    void renderEditVehicleMenu() const;
    void renderAddNewUserMenu() const;
    void renderAddNewTripMenu() const;
    void renderViewTripMenu() const;
    void renderStartTripMenu() const;
    void renderCompleteTripMenu() const;
    void showDialog(string message, string id="") const;
    void cleanMemory();
    
public:
    Application();
    void start();
};


int main(){
    Application *app = new  Application();
    app->start();
    return 0;
}

Date::Date(){
    time_t now = time(nullptr);
    this->empty = false;
    this->date = *localtime(&now);
}

Date::Date(string date){
    if(date.length()<0){
        this->empty=true;
        return;
    }
    this->empty=false;
    vector<string> dateComponents = split(date,DATEDELIMETER);
    int day = stoi(dateComponents[0]);
    int month = stoi(dateComponents[1]);
    int year = stoi(dateComponents[2]);
    time_t now =  time(nullptr);
    this->date = *localtime(&now);
    this->date.tm_year = year-1900;
    this->date.tm_mon = month-1;
    this->date.tm_mday = day;
    mktime(&this->date);
}

bool Date::isEmpty() const{
    return this->empty;
}

string Date:: toString() const{
    if(this->empty)
        return "";
    stringstream ss;
    ss<<this->date.tm_mday<<"/"<<this->date.tm_mon+1<<"/"<<this->date.tm_year+1900;
    return ss.str();
}

bool Date::operator>(Date date) const{
    if(this->isEmpty()|| date.isEmpty()){
        return false;
    }
    int dyear = this->date.tm_year - date.date.tm_year;
    int dmonth = this->date.tm_mon - date.date.tm_mon;
    int dday = this->date.tm_mday - this->date.tm_mday;

    if(dyear!=0){
        return dyear>0;
    }
    else if(dmonth!=0){
        return dmonth>0;
    }
    else{
        return dday>0;
    }
}

bool Date::operator<(Date date) const{
    if(this->isEmpty()|| date.isEmpty()){
        return false;
    }
    int dyear = this->date.tm_year - date.date.tm_year;
    int dmonth = this->date.tm_mon - date.date.tm_mon;
    int dday = this->date.tm_mday - this->date.tm_mday;

    if(dyear!=0){
        return dyear<0;
    }
    else if(dmonth!=0){
        return dmonth<0;
    }
    else{
        return dday<0;
    }
}

bool Date:: operator>=(Date date) const{
    return !(*this<date);
}

bool Date:: operator<=(Date date) const{
    return !(*this>date);
}

Vehicle::Vehicle(
            string registrationNumber, 
            VehicleType type,
            int seats,
            string companyName,
            double pricePerKm,
            Date PUCExpirationDate,
            long recordId
        ): Entity(recordId){
    this->registrationNumber = registrationNumber;
    this->type = type;
    this->seats = seats;
    this->companyName = companyName;
    this->pricePerKm = pricePerKm;
    this->PUCExpirationDate = PUCExpirationDate;
}

string Vehicle::getRegistrationNumber() const{
    return this->registrationNumber;
}

VehicleType Vehicle::getVehicleType() const{
    return this->type;
}

int Vehicle::getSeats() const{
    return this->seats;
}

string Vehicle::getCompanyName() const{
    return this->companyName;
}

double Vehicle::getPricePerKm() const{
    return this->pricePerKm;
}

Date Vehicle::getPUCExpirationDate() const{
    return this->PUCExpirationDate;
}

void Vehicle::setPricePerKm(double newPrice){
    this->pricePerKm = newPrice;
}

string Vehicle::getVehicleTypeName() const{
    switch (this->type)
    {
        case VehicleType::bike:
            return "bike" ;
            break;
        case VehicleType::car:
            return "Car";
            break;
        case VehicleType::bus: 
            return "Bus";
            break;
        default:
            return "";
            break;
    }
}

void Vehicle::display() const{
    cout<<"Vehicle Details:\n";
    cout<<"Registration number: "<<this->registrationNumber<<endl;
    cout<<"Vehicle type: "<<this->getVehicleTypeName()<<endl;
    cout<<"Number of seats: "<<this->seats<<endl;
    cout<<"Company name: "<<this->companyName<<endl;
    cout<<"Price per km: "<<this->pricePerKm<<endl;
    cout<<"PUC Expiration date: "<<this->PUCExpirationDate.toString()<<endl;
}

string Vehicle::toString() const{
    stringstream ss;
    ss<<recordId<<DELIMETER
      <<registrationNumber<<DELIMETER
      <<type<<DELIMETER
      <<seats<<DELIMETER
      <<companyName<<DELIMETER
      <<to_string(pricePerKm)<<DELIMETER
      <<PUCExpirationDate.toString();
      return ss.str();
}

void Vehicle::setDataFrom(Entity *s){
    Vehicle *v = dynamic_cast<Vehicle*>(s);
    if(v){
        this->registrationNumber = v->registrationNumber;
        this->type = v->type;
        this->seats = v->seats;
        this->companyName = v->companyName;
        this->pricePerKm = v->pricePerKm;
        this->PUCExpirationDate = v->PUCExpirationDate;
    }
}

User::User(string name, string contact, string email, long recordId) : Entity(recordId)
{
    this->name = name;
    this->contact = contact;
    this->email = email;
}

string User ::getName() const { return this->name; }
string User ::getContact() const { return this->contact; }
string User ::getEmail() const { return this->email; }
void User ::setName(string newName) { this->name = newName; }
void User ::setContact(string newContact) { this->contact = newContact; }
void User ::setEmail(string newEmail) { this->email = newEmail; }

void User ::display() const
{
    cout << "User Details : " << endl;
    cout << "Name : " << this->name << endl;
    cout << "Contact : " << this->contact << endl;
    cout << "Email id : " << this->email << endl;
}

string User ::toString() const
{
    stringstream ss;
    ss << recordId << DELIMETER
    << name << DELIMETER
    << contact << DELIMETER
    << email;
    return ss.str();
}

void User ::setDataFrom(Entity *s)
{
    User *user = dynamic_cast<User *>(s);

    if (user)
    {
        this->contact = user->contact;
        this->name = user->name;
        this->email = user->email;
    }
}

Trip ::Trip(const Vehicle*vehicle, const User * user, Date startDate, Date endDate, long recordId, long startReading, long endReading, double fare, bool isCompleted) : Entity(recordId)
{
    this->vehicle = vehicle;
    this->user = user;
    this->startDate = startDate;
    this->endDate = endDate;
    this->startReading = startReading;
    this->endReading = endReading;
    this->fare = fare;
    this->completed = isCompleted;
}

const User &Trip ::getUser() const { return *this->user; }
const Vehicle &Trip ::getVehicle() const { return *this->vehicle; }
Date Trip ::getStartDate() const { return this->startDate; }
Date Trip ::getEndDate() const { return this->endDate; }
long Trip ::getStartReading() const { return this->startReading; }
long Trip ::getEndReading() const { return this->endReading; }
long Trip ::getFare() const { return this->fare; }
bool Trip ::isCompleted() const { return this->completed; }

void Trip ::startTrip(long startReading)
{
    this->startReading = startReading;
}

double Trip ::completeTrip(long endReading)
{
    if (this->completed)
    {
        return this->fare;
    }

    this->endReading = endReading;
    this->fare = (this->endReading - this->startReading) * this->vehicle->getPricePerKm(); //??
    this->completed = true;
    return this->fare;
}

void Trip ::display() const
{
    cout << "Vehicle Details : " << endl;
    cout << "Registration no : " << this->vehicle->getRegistrationNumber() << endl;
    cout << "Vehicle Type : " << this->vehicle->getVehicleTypeName() << endl;
    cout << "Company name : " << this->vehicle->getCompanyName() << endl;
    cout << "Price per km : " << this->vehicle->getPricePerKm() << endl;
    cout << endl;
    cout << "User Details : " << endl;
    cout << "Name : " << this->user->getName() << endl;
    cout << "Contact : " << this->user->getContact() << endl;
    cout << endl;

    cout << "Trip Details : " << endl;
    cout << "Start date : " << this->startDate.toString() << endl;
    cout << "End date : " << this->endDate.toString() << endl;

    string tripStatus = "Not started";

    if (this->startReading != 0)
    {
        tripStatus = "On going";
        cout << "Start Reading : " << this->startReading << endl;
    }
    if (this->completed)
    {
        tripStatus = "Completed";
        cout << "End reading : " << this->endReading << endl;
        cout << "Total run : " << this->endReading - this->startReading << endl;
        cout << "Total fare : " << this->fare << endl;
    }

    cout << "Trip status : " << tripStatus << endl;
}

string Trip ::toString() const
{
    stringstream ss;
    ss << recordId << DELIMETER
       << vehicle->getRecord() << DELIMETER
       << user->getRecord() << DELIMETER
       << startDate.toString() << DELIMETER
       << endDate.toString() << DELIMETER
       << startReading << DELIMETER
       << endReading << DELIMETER
       << fare << DELIMETER
       << completed;
    return ss.str();
}

void Trip ::setDataFrom(Entity *s)
{
    Trip *t = static_cast<Trip *>(s); // keyword
    if (t)
    {
        this->vehicle = t->vehicle;
        this->user = t->user;
        this->startDate = t->startDate;
        this->endDate = t->endDate;
        this->startReading = t->startReading;
        this->endReading = t->endReading;
        this->fare = t->fare;
        this->completed = t->completed;
    }
}

template<typename T>
Table<T> ::Table(string filename) throw (MemoryError){
    this->fileName = filename;
}
template<typename T>
long Table<T>::getNextRecordId() const{
    return this->records.size()+1;
}

template<typename T>
const T* const Table<T>::addNewRecord(T record) throw (MemoryError, IOError){
    T *newRecord = new T(record);
    if(!newRecord){
        throw new MemoryError();
    }
    newRecord->recordId = this->getNextRecordId();
    this->records.push_back(newRecord);
    try{
        this->writeToFile();
    } catch(IOError error){
        this->records.pop_back();
        delete newRecord;
        throw;
    }
    return newRecord;
}

template<typename T>
void Table<T> :: updateRecord(T updatedRecord) throw (IOError,RecordNotFoundError){
    for(int i=0;i<records.size();i++){
        if(records[i]->getRecord()==updatedRecord.getRecord()){
            T *pointerToRecord = dynamic_cast<T*>(records[i]);
            if(pointerToRecord){
                T oldRecord = T(*pointerToRecord);
                records[i]->setDataFrom(&updatedRecord);
                try{
                    this->writeToFile();
                    return;
                }
                catch(IOError error){
                    records[i]->setDataFrom(&oldRecord);
                    throw;
                }
            }
        }
    }
    throw RecordNotFoundError();
}

template<typename T>
void Table<T>:: writeToFile() throw(IOError){
    this->fileStream.open(fileName,ios::out|ios::trunc);
    if(!this->fileStream){
        throw IOError();
    }
    for(auto record: records){
        fileStream<<record->toString()<<endl;
    }
    this->fileStream.close();
}

template<typename T>
const T* const Table<T>::getRecordForId(long recordID) const throw(RecordNotFoundError){
    try{
        return this->getReferenceOfRecordForId(recordID);
    }
    catch(RecordNotFoundError err){
        throw;
    }
}

template<typename T>
T* Table<T>::getReferenceOfRecordForId(long recordId) const throw (RecordNotFoundError){
    for(auto record: records){
        if(record->getRecord() == recordId){
            return dynamic_cast<T*>(record);
        }
    }
    throw RecordNotFoundError();
}


Database ::Database() throw(IOError, MemoryError)
{
    try
    {
        this->vehicleTable = new Table<Vehicle>("vehicle.txt");
        this->userTable = new Table<User>("users.txt");
        this->tripTable = new Table<Trip>("trips.txt");

        this->fetchAllVehicles();
        this->fetchAllUsers();
        this->fetchAllTrips();
    }
    catch (...)
    {
        throw;
    }
}

void Database ::fetchAllVehicles() throw(IOError, MemoryError)
{
    this->vehicleTable->fileStream.open(this->vehicleTable->fileName);

    if (!this->vehicleTable->fileStream)
    {
        throw IOError();
    }

    for (string line; getline(this->vehicleTable->fileStream, line);)
    {
        vector<string> components = split(line, DELIMETER);

        auto recordId = stol(components[0]);
        auto registrationNumber = components[1];
        auto type = VehicleType(stoi(components[2]));
        auto seats = stoi(components[3]);
        auto companyName = components[4];
        auto pricePerKm = stod(components[6]);
        auto PUCExpirationDate = Date(components[6]);

        Vehicle *record = new Vehicle(registrationNumber, type, seats, companyName, pricePerKm, PUCExpirationDate, recordId);
        if (!record)
        {
            throw MemoryError();
        }
        this->vehicleTable->records.push_back(record);
    }

    this->vehicleTable->fileStream.close();
}

void Database ::fetchAllUsers() throw(IOError, MemoryError)
{
    this->userTable->fileStream.open(this->userTable->fileName);

    if (!this->userTable->fileStream)
    {
        throw IOError();
    }

    for (string line; getline(this->userTable->fileStream, line);)
    {
        vector<string> components = split(line, DELIMETER);
        auto recordId = stoi(components[0]);
        auto name = components[1];
        auto contact = components[2];
        auto email = components[3];

        User *record = new User(name, contact, email, recordId);
        if (!record)
        {
            throw MemoryError();
        }
        this->userTable->records.push_back(record);
    }

    this->userTable->fileStream.close();
}

void Database ::fetchAllTrips() throw(IOError, MemoryError)
{
    this->tripTable->fileStream.open(this->tripTable->fileName);
    if (!this->tripTable->fileStream)
    {
        throw IOError();
    }

    for (string line; getline(this->tripTable->fileStream, line);)
    {
        vector<string> components = split(line, DELIMETER);

        try
        {
            auto recordID = stoi(components[0]);
            auto vehiclePtr = this->vehicleTable->getReferenceOfRecordForId(stoi(components[1]));
            auto userPtr = this->userTable->getReferenceOfRecordForId(stoi(components[2]));
            auto startDate = Date(components[3]);
            auto endDate = Date(components[4]);
            auto startReading = stol(components[5]);
            auto endReading = stol(components[6]);
            auto fare = stod(components[7]);
            auto isCompleted = components[8] == "0" ? false : true;

            Trip *record = new Trip(vehiclePtr, userPtr, startDate, endDate, recordID, startReading, endReading, fare, isCompleted);
            if (!record)
            {
                throw MemoryError();
            }
            this->tripTable->records.push_back(record);
        }
        catch (...)
        {
        }
    }

    this->tripTable->fileStream.close();
}

const Vehicle *const Database ::getVehicle(string RegistrationNo)
    const throw(RecordNotFoundError)
{
    for (auto record : this->vehicleTable->records)
    {
        Vehicle *vehicle = dynamic_cast<Vehicle *>(record);
        if (vehicle)
        {
            if (vehicle->getRegistrationNumber() == RegistrationNo)
            {
                return vehicle;
            }
        }
    }
    throw RecordNotFoundError();
}

const User *const Database ::getUser(string contactNo) const throw(RecordNotFoundError)
{
    for (auto record : this->userTable->records)
    {
        User *user = dynamic_cast<User *>(record);
        if (user)
        {
            if (user->getContact() == contactNo)
            {
                return user;
            }
        }
    }
    throw RecordNotFoundError();
}

const vector<const Vehicle *> Database ::getVehicle(Date startDate, Date endDate, VehicleType type) const
{
    vector<const Vehicle *> vehicles = vector<const Vehicle *>();

    for (auto vrecord : this->vehicleTable->records)
    {
        Vehicle *vehicle = dynamic_cast<Vehicle *>(vrecord);
        if (vehicle && vehicle->getVehicleType() == type)
        {
            bool tripFound = false;
            for (auto trecord : (this->tripTable->records))
            {
                Trip *trip = dynamic_cast<Trip *>(trecord);
                if (trip && !trip->isCompleted() && trip->getVehicle().getRecord() == vehicle->getRecord() && !(trip->getStartDate() >= endDate && trip->getEndDate() >= endDate) && !(trip->getStartDate() <= startDate && trip->getEndDate() <= startDate))
                {
                    tripFound = true;
                    break;
                }
            }
            if (!tripFound)
            {
                vehicles.push_back(vehicle);
            }
        }
    }
    return vehicles;
}

void Database ::cleanUp()
{
    delete this->vehicleTable;
    delete this->userTable;
    delete this->tripTable;
}

Database ::~Database()
{
    this->cleanUp();
}

const Table<Vehicle> *const Database ::getVehicleRef() const
{
    return this->vehicleTable;
}

const Table<User> *const Database ::getUserRef() const
{
    return this->userTable;
}

const Table<Trip> *const Database ::getTripRef() const
{
    return this->tripTable;
}

template <class T>
void Database ::addNewRecord(T *record) throw(IOError, MemoryError)
{
    try
    {
        Vehicle *v = dynamic_cast<Vehicle *>(record);
        if (v)
        {
            auto savedRecord = this->vehicleTable->addNewRecord(*v);
            record->recordId = savedRecord->recordId;
            return;
        }

        User *u = dynamic_cast<User *>(record);
        if (u)
        {
            auto savedRecord = this->userTable->addNewRecord(*u);
            record->recordId = savedRecord->recordId;
            return;
        }
        Trip *t = dynamic_cast<Trip *>(record);
        if (t)
        {
            auto savedRecord = this->tripTable->addNewRecord(*t);
            record->recordId = savedRecord->recordId;
            return;
        }
    }
    catch (...)
    {
        throw;
    }
}

template <class T>
void Database ::updateRecord(T *record) throw(IOError, RecordNotFoundError)
{
    try
    {
        Vehicle *v = dynamic_cast<Vehicle *>(record);
        if (v)
        {
            this->vehicleTable->updateRecord(*v);
            return;
        }

        User *u = dynamic_cast<User *>(record);
        if (u)
        {
            this->userTable->updateRecord(*u);
            return;
        }

        Trip *t = dynamic_cast<Trip *>(record);
        if (t)
        {
            this->tripTable->updateRecord(*t);
            return;
        }
    }
    catch (...)
    {
        throw;
    }
}

Application::Application(){
    try{
        this->db = new Database();
    }
    catch(Error e){
        cout<<e.getMessage();
        exit(EXIT_FAILURE);
    }
}

void Application::gotoXY(int x, int y) const{
    printf("%c[%d;%df",0x1B,y,x);
}

void Application::renderMenu(){
    char choice = 1;
    while(true){
        system("cls");
        gotoXY(25,4);
        cout<<"Select Any option from below";
        gotoXY(25,5);
        cout<<"1. Add New Vehicle";
        gotoXY(25,6);
        cout<<"2. View Vehicle Details";
        gotoXY(25,7);
        cout<<"3. Edit Vehicle Details";
        gotoXY(25,8);
        cout<<"4. Add New User";
        gotoXY(25,9);
        cout<<"5. Add New Trip";
        gotoXY(25,10);
        cout<<"6. View Trip";
        gotoXY(25,11);
        cout<<"7. Start Trip";
        gotoXY(25,12);
        cout<<"8. Complete Trip";
        gotoXY(25,13);
        cout<<"9. Exit";

        gotoXY(30,15);
        cout<<"Enter your Choice: ";
        gotoXY(50,15);
        cin>>choice;
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        switch(choice){
            case '1': 
                this->renderAddNewVehicleMenu();
                break;
            case '2': 
                this->renderViewVehicleMenu();
                break;
            case '3': 
                this->renderEditVehicleMenu();
                break;
            case '4':
                this->renderAddNewUserMenu();
                break;
            case '5':
                this->renderAddNewTripMenu();
                break;
            case '6':
                this->renderViewTripMenu();
                break;
            case '7':
                this->renderStartTripMenu();
                break;
            case '8':
                this->renderCompleteTripMenu();
            case '9':
                this->cleanMemory();
                system("cls");
                exit(EXIT_SUCCESS);
                break;
            default: 
                this->cleanMemory();
                system("cls");
                exit(EXIT_SUCCESS);
                break;
        }
    }
}

void Application::renderAddNewVehicleMenu() const{
    string regNo,companyName;
    int vehicleType,seat;
    double price;
    string pucExpirationDate;
    system("cls");
    gotoXY(0,1);
    cout<<"Enter Details of Vehicle:";
    gotoXY(0,3);
    cout<<"Enter Vehicle Registration Number ";
    getline(cin,regNo);
    cout<<"Enter Vehicle type:";
    gotoXY(0,5);
    cout<<"1. Bike 2. Car 3. Bus";
    gotoXY(0,6);
    cin>>vehicleType;
    cout<<"Enter number of seats: ";
    cin>>seat;
    fflush(stdin);
    cout<<"Enter name of company: ";
    getline(cin,companyName);
    cout<<"Enter Price per KM: ";
    cin>>price;
    cout<<"Enter PUC Expiration Date (dd/mm/yyyy): ";
    cin>>pucExpirationDate;

    Vehicle *vehicle;
    try{
        vehicle = 
        new Vehicle(regNo,VehicleType(vehicleType),
                    seat,companyName,price,Date(pucExpirationDate));
        this->db->addNewRecord(vehicle);
        stringstream ss;
        ss<<"Vehicle id: "<<vehicle->getRecord();
        showDialog("Vehicle Added Successfully",ss.str());
        system("pause");
    }
    catch(Error e){
        showDialog(e.getMessage());
        delete vehicle; 
    }
}

void Application::renderViewVehicleMenu() const{
    string regNo;
    system("cls");
    gotoXY(0,1);
    cout<<"Enter registration number of Vehicle: ";
    getline(cin,regNo);
    try{
        auto vehicle = this->db->getVehicle(regNo);
        vehicle->display();
    }
    catch(Error e){
        this->showDialog(e.getMessage());
    }
    system("pause");
}

void Application::renderEditVehicleMenu() const{
    string regNo,price;
    system("cls");
    gotoXY(0,1);
    cout<<"Enter registration number of vehicle you want to edit:";
    getline(cin,regNo);
    try{
        auto vehicle = this->db->getVehicle(regNo);
        Vehicle *newVehicle = new Vehicle(*vehicle);
        vehicle->display();
        cout<<"Enter new price per KM";
        getline(cin,price);
        if(price!=""){
            newVehicle->setPricePerKm(stod(price));    
            this->db->updateRecord(newVehicle);
            this->showDialog("Vehicle updated Successfully"); 
        }
        delete newVehicle;
    }
    catch(Error e){
        this->showDialog(e.getMessage());
    }
}

void Application::renderAddNewUserMenu() const{
    string name,contact,email;
    system("cls");
    gotoXY(0,1);
    cout<<"Enter details of User: ";
    gotoXY(0,2);
    cout<<"Enter Name of User: ";
    getline(cin,name);
    cout<<"Enter Contact number of the user: ";
    getline(cin,contact);
    cout<<"Enter Email of the user: ";
    getline(cin,email);
    try{
        User *user = new User(name,contact,email);
        this->db->addNewRecord(user);
        stringstream ss;
        ss<<"User id: "<<user->getRecord();
        showDialog("User Added succesfully: ",ss.str());
        delete user;
    }
    catch(Error e){
        showDialog(e.getMessage());
    }   
}

void Application::renderAddNewTripMenu() const{
    const User *user;
    const Vehicle *vehicle;
    system("cls");
    gotoXY(0,1);
    string contact,startDate,endDate;
    int vehicleType;
    cout<<"Enter details of Trip: ";
    gotoXY(0,2);
    cout<<"Enter contact number of user: ";
    getline(cin,contact);
    try{
        user = this->db->getUser(contact);
        gotoXY(0,3);
        cout<<"User Found: "<<user->getName()<<endl;
    }
    catch(Error e){
        showDialog(e.getMessage());
        return;
    }
    cout<<"When should your trip start?(d/m/yyyy): ";
    getline(cin,startDate);
    cout<<"When should your trip end?(d/m/yyyy): ";
    getline(cin,endDate);
    cout
        <<"Enter Vehicle Type:\n"
        <<"1.Bike 2.Car 3.Bus\n";
    cin>>vehicleType;
        auto availableVehicles = 
            this->db->getVehicle(Date(startDate),
                                 Date(endDate),
                                 VehicleType(vehicleType));

        if(availableVehicles.size()==0){
            this->showDialog("No vehicles are free in given Date Range");
            return;
        }
        gotoXY(0,12);
        cout<<"Registration no.    |"<<"Seats    |"<<"Price per KM     |"<<endl;
        for(auto vehicle: availableVehicles){
            cout<<setw(13)<<vehicle->getRegistrationNumber()
                <<setw(10)<<vehicle->getSeats()
                <<setw(10)<<vehicle->getPricePerKm()<<"Rs. per KM"<<endl;
        }
        fflush(stdin);
        string regNo;
        cout<<"Enter Registration no of desired Vehicle"<<endl;
        getline(cin,regNo);
        try{
            vehicle = this->db->getVehicle(regNo); 
        }
        catch(Error e){
            showDialog(e.getMessage());
            return;
        }
        long userId = user->getRecord();
        long vehicleID = vehicle->getRecord();
        Trip *trip;
        try{
            trip = 
            new Trip(this->db->getVehicleRef()->getRecordForId(vehicleID)
                    ,this->db->getUserRef()->getRecordForId(userId)
                    ,Date(startDate),Date(endDate));
            this->db->addNewRecord(trip);
            stringstream ss;    
            ss<<"Trip id: "<<trip->getRecord();
            showDialog("Trip added succesfully",ss.str());
        }
        catch(Error e){
            showDialog(e.getMessage());
            return;
        }    
    delete trip;
}

void Application::renderViewTripMenu() const{
    long tripId;
    system("cls");
    cout<<"Enter Trip id: ";
    cin>>tripId;
    gotoXY(0,3);
    try{
        auto trip = this->db->getTripRef()->getRecordForId(tripId);
        trip->display();
        cout<<endl;
    }
    catch(Error e){
        this->showDialog(e.getMessage());
    }
}

void Application::renderStartTripMenu() const{
    long tripId;
    long initialReading;
    system("cls");
    cout<<"Enter Trip id: ";
    cin>>tripId;
    cout<<"Enter Odometer(distance) reading: ";
    cin>>initialReading;

    Trip *newTrip;
    try{
        auto trip = this->db->getTripRef()->getRecordForId(tripId);
        newTrip = new Trip(*trip);
        newTrip->startTrip(initialReading);
        this->db->updateRecord(newTrip);
        showDialog("Trip started successfully");
    }
    catch(Error e){
        this->showDialog(e.getMessage());
    }
}

void Application::renderCompleteTripMenu() const{
    long tripId;
    long finalReading;
    system("cls");
    cout<<"Enter Trip id: ";
    cin>>tripId;
    cout<<"Enter Odometer(distance) reading: ";
    cin>>finalReading;

    Trip *newTrip;
    try{
        auto trip = this->db->getTripRef()->getRecordForId(tripId);
        newTrip = new Trip(*trip);
        auto fare = newTrip->completeTrip(finalReading);
        this->db->updateRecord(newTrip);
        stringstream ss;
        ss<<"Total Fare: "<<fare;
        showDialog("Trip completed successfully");
    }
    catch(Error e){
        this->showDialog(e.getMessage());
    }
}

void Application::showDialog(string message, string id) const {
    cout<<"\n\n";
    cout<<message<<"\n";
    if(id!=""){
        cout<<id<<"\n";
    }
    system("pause");
}


void Application::welcome(){
    system("cls");
    gotoXY(25,5);
    cout<<"Welcome to Vehicle Rental System\n";
    this->renderMenu();
}

void Application::start(){
    welcome();
}

void Application::cleanMemory(){
    delete db;
}