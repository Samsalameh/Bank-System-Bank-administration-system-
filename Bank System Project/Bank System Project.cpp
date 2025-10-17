//---------------------------------Bank System (administration system)--------------------------------------------------------

#include<iostream>
#include <stdio.h>
#include<iomanip>
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;
enum enMainMenueOptions {
    eListClients = 1, eAddNewClient = 2, eDeleteClient = 3,
    eUpdateClient = 4, eFindClient = 5, eTransaction = 6, eManageUsers = 7, eLogOut = 8
};
enum enPermissionsMenuOptions {
    eListClietsPer = 1,
    eAddNewClietPer = 2,
    eDeleteClietPer = 4,
    eUpdateClietPer = 8,
    eFindClietPer = 16,
    eTransactionPer = 32,
    eManageUsersPer = 64,
    eAll = -1
};
enum enManageUsersOptions {
    eListUsers = 1, eAddNewUser = 2, eDeleteUser = 3,
    eUpdateUser = 4, eFindUser = 5, eHomeMenue = 6
};
enum enTransactionMenueOptions {
    eDeposit = 1, eWithdraw = 2, eTotalBalances = 3, eMainMenue = 4
};
const string ClientsFileName = "Clients.txt";
const string UsersFileName = "Users.txt";
void ShowAccessDeniedMassage()
{
    cout << "\n-------------------------------------\n";
    cout << "Access Denied, \n You don't have Permission to do this,\n Please contact with your User.\n";
    cout << "\n-------------------------------------\n";
}
struct sClient
{
    string AccountNumber;
    string PinCode;
    string Name;
    string Phone;
    double AccountBalance;
    bool MarkForDelete = false;
};
struct sUser
{
    string UserName;
    string PassWord;
    int Permissions = 0;
    bool MarkForDelete = false;
};
sUser CurrentUser;
void ShowTransactionMenue();
void ShowMainMenue();
void ShowManageUsersMenue();
void GoBackToMainMenue();
vector<string> SplitString(string S1, string Delim)
{
    vector<string> vString;
    short pos = 0;
    string sWord; // define a string variable  

    // use find() function to get the position of the delimiters  
    while ((pos = S1.find(Delim)) != std::string::npos)
    {
        sWord = S1.substr(0, pos); // store the word   
        if (sWord != "")
        {
            vString.push_back(sWord);
        }

        S1.erase(0, pos + Delim.length());  /* erase() until positon and move to next word. */
    }

    if (S1 != "")
    {
        vString.push_back(S1); // it adds last word of the string.
    }

    return vString;

}
sClient ConvertLinetoRecord(string Line, string Seperator = "//")
{
    sClient Client;
    vector<string> vClientData;
    vClientData = SplitString(Line, Seperator);

    Client.AccountNumber = vClientData[0];
    Client.PinCode = vClientData[1];
    Client.Name = vClientData[2];
    Client.Phone = vClientData[3];
    Client.AccountBalance = stod(vClientData[4]);//cast string to double
    return Client;
}
bool CheckAccessPermission(enPermissionsMenuOptions Permission)
{
    if (CurrentUser.Permissions == enPermissionsMenuOptions::eAll)
        return true;
    if ((Permission & CurrentUser.Permissions) == Permission)
        return true;
    else
        return false;
}
sUser ConvertUsersLinetoRecord(string Line, string Seperator = "//")
{
    sUser User;
    vector<string> vUsersData;
    vUsersData = SplitString(Line, Seperator);

    User.UserName = vUsersData[0];
    User.PassWord = vUsersData[1];
    User.Permissions = stoi(vUsersData[2]);

    return User;
}
int ReadPermissionsToSet()
{
    int Permissions = 0;
    char Answer = 'n';
    cout << "\nDo you want to give Full Permissions ?\n ";
    cin >> Answer;
    if (toupper(Answer)== 'Y')
    {
        Permissions = -1;
    }
    cout << "\nDo you want to give access to  ?\n ";
    cout << "\nShow Client List ? y/n? \n";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
    {
        Permissions += enPermissionsMenuOptions ::eListClietsPer;
    }
    cout << "\nAdd New Client ? y/n? \n";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
    {
        Permissions += enPermissionsMenuOptions::eAddNewClietPer;
    }
    cout << "\nDelete Client ? y/n? \n";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
    {
        Permissions += enPermissionsMenuOptions::eDeleteClietPer;
    }
    cout << "\nUpdate Client ? y/n? \n";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
    {
        Permissions += enPermissionsMenuOptions::eUpdateClietPer;
    }
    cout << "\nFind Client ? y/n? \n";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
    {
        Permissions += enPermissionsMenuOptions::eFindClietPer;
    }
    cout << "\nTransctions ? y/n? \n";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
    {
        Permissions += enPermissionsMenuOptions::eTransactionPer;
    }
    cout << "\nManage Users ? y/n? \n";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
    {
        Permissions += enPermissionsMenuOptions::eManageUsersPer;
    }
    return Permissions;
}
string ConvertRecordToLine(sClient Client, string Seperator = "//")
{

    string stClientRecord = "";
    stClientRecord += Client.AccountNumber + Seperator;
    stClientRecord += Client.PinCode + Seperator;
    stClientRecord += Client.Name + Seperator;
    stClientRecord += Client.Phone + Seperator;
    stClientRecord += to_string(Client.AccountBalance);
    return stClientRecord;
}
string ConvertUserRecordToLine(sUser User, string Seperator = "//")
{

    string stUserRecord = "";
    stUserRecord += User.UserName + Seperator;
    stUserRecord += User.PassWord + Seperator;
    stUserRecord += to_string(User.Permissions);

    return stUserRecord;
}
vector <sUser> LoadUsersDataFromFile(string FileName)
{
    vector <sUser> vUser;
    fstream UsersFile;
    UsersFile.open(FileName, ios::in); //read Mode

    if (UsersFile.is_open())
    {
        string Line;
        sUser User;

        while (getline(UsersFile, Line))
        {
            User = ConvertUsersLinetoRecord(Line);
            vUser.push_back(User);
        }
        UsersFile.close();
    }
    return vUser;
}
bool FindUserByUsernameAndPassword(string UserName,string PassWord, sUser& User)
{
    vector <sUser> vUser = LoadUsersDataFromFile(UsersFileName);
    for (sUser U : vUser)
    {
        if (U.UserName == UserName && U.PassWord==PassWord)
        {
            User = U;
            return true;
        }
    }
    return false;
}
bool LoadUserInfo(string UserName,string PassWord)
{
    if (FindUserByUsernameAndPassword(UserName, PassWord, CurrentUser))
        return true;
    else
        return false;
}
bool ClientExistsByAccountNumber(string AccountNumber, string FileName)
{

    vector <sClient> vClients;
    fstream MyFile;
    MyFile.open(FileName, ios::in);//read Mode

    if (MyFile.is_open())
    {
        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {
            Client = ConvertLinetoRecord(Line);
            if (Client.AccountNumber == AccountNumber)
            {
                MyFile.close();
                return true;
            }
            vClients.push_back(Client);
        }

        MyFile.close();

    }
    return false;
}
bool UsersExistsByUserName(string UserName, string FileName)
{

    vector <sUser> vUsers;
    fstream UsersFile;
    UsersFile.open(FileName, ios::in);//read Mode

    if (UsersFile.is_open())
    {
        string Line;
        sUser User;

        while (getline(UsersFile, Line))
        {
            User = ConvertUsersLinetoRecord(Line);
            if (User.UserName == UserName)
            {
                UsersFile.close();
                return true;
            }
            vUsers.push_back(User);
        }

        UsersFile.close();

    }
    return false;
}

sClient ReadNewClient()
{
    sClient Client;
    cout << "Enter Account Number? ";

    getline(cin >> ws, Client.AccountNumber);

    while (ClientExistsByAccountNumber(Client.AccountNumber, ClientsFileName))
    {
        cout << "\nClient with [" << Client.AccountNumber << "] already exists, Enter another Account Number? ";
        getline(cin >> ws, Client.AccountNumber);
    }

    cout << "Enter PinCode? ";
    getline(cin, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> Client.AccountBalance;

    return Client;
}
sUser ReadNewUser()
{
    sUser User;
    cout << "Enter UserName? ";

    getline(cin >> ws, User.UserName);

    while (UsersExistsByUserName(User.UserName, UsersFileName))
    {
        cout << "\nUser with [" << User.UserName << "] already exists, Enter another UserName? ";
        getline(cin >> ws, User.UserName);
    }

    cout << "Enter PassWord? ";
    getline(cin, User.PassWord);
    User.Permissions = ReadPermissionsToSet();
    //MakingPermmissions(User);


    return User;
}
vector <sClient> LoadCleintsDataFromFile(string FileName)
{
    vector <sClient> vClients;
    fstream MyFile;
    MyFile.open(FileName, ios::in); //read Mode

    if (MyFile.is_open())
    {
        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {
            Client = ConvertLinetoRecord(Line);
            vClients.push_back(Client);
        }
        MyFile.close();
    }
    return vClients;
}
void PrintClientRecordLine(sClient Client)
{
    cout << "| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(10) << left << Client.PinCode;
    cout << "| " << setw(40) << left << Client.Name;
    cout << "| " << setw(12) << left << Client.Phone;
    cout << "| " << setw(12) << left << Client.AccountBalance;
}
void PrintUserRecordLine(sUser User)
{
    cout << "| " << setw(15) << left << User.UserName;
    cout << "| " << setw(10) << left << User.PassWord;
    cout << "| " << setw(10) << left << User.Permissions;
}
void PrintBalanceClientRecordLine(sClient Client)
{
    cout << "\n| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(10) << left << Client.Name;
    cout << "\t\t\t\t\t\t\t| " << setw(12) << left << Client.AccountBalance << endl;
}
string ReadUserName()
{
    string Username = "";
    cout << "Enter User Name? ";
    cin >> Username;
    return Username;
}
string ReadUserPssWd()
{
    string Password = "";
    cout << "Enter Password ? ";
    cin >> Password;
    return Password;
}
bool FindUserByUserName(string UserName, vector <sUser> vUsers, sUser& User)
{
    for (sUser M : vUsers)
    {

        if (M.UserName == UserName)
        {
            User = M;
            return true;
        }

    }
    return false;
}

void LogIn()
{
    bool LoginFailed = false;
    string UserName, PassWord;
    do
    {
        system("cls");
        cout << "\n-----------------------------------\n";
        cout << "\tLogin Screen";
        cout << "\n-----------------------------------\n";

        if (LoginFailed)
        {
            cout << "Invailed UserName/Password!\n";
        }
        cout << "Enter UserName ? ";
            cin >> UserName;
            cout << "Enter PassWord ?";
            cin >> PassWord;
        LoginFailed = !LoadUserInfo(UserName, PassWord);
    } while (LoginFailed);
    ShowMainMenue();
    
}

void ShowAllClientsScreen()
{
    if (!CheckAccessPermission(enPermissionsMenuOptions::eListClietsPer))
    {
        ShowAccessDeniedMassage();
        GoBackToMainMenue();
        return;
    }
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);

    cout << "\n\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "Accout Number";
    cout << "| " << left << setw(10) << "Pin Code";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(12) << "Phone";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    if (vClients.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else
        for (sClient Client : vClients)
        {
            PrintClientRecordLine(Client);
            cout << endl;
        }
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
}
void ShowAllUsersScreen()
{

    vector <sUser> vUsers = LoadUsersDataFromFile(UsersFileName);

    cout << "\n\t\t\t\t\tUsers List (" << vUsers.size() << ") User(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "UserName";
    cout << "| " << left << setw(10) << "PassWord";
    cout << "| " << left << setw(10) << "Permissions";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    if (vUsers.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else
        for (sUser User : vUsers)
        {
            PrintUserRecordLine(User);
            cout << endl;
        }
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
}

void PrintClientCard(sClient Client)
{
    cout << "\nThe following are the client details:\n";
    cout << "-----------------------------------";
    cout << "\nAccout Number: " << Client.AccountNumber;
    cout << "\nPin Code     : " << Client.PinCode;
    cout << "\nName         : " << Client.Name;
    cout << "\nPhone        : " << Client.Phone;
    cout << "\nAccount Balance: " << Client.AccountBalance;
    cout << "\n-----------------------------------\n";
}
void PrintUserCard(sUser User)
{
    cout << "\nThe following are the User details:\n";
    cout << "-----------------------------------";
    cout << "\nUserName: " << User.UserName;
    cout << "\nPassWord     : " << User.PassWord;
    cout << "\nPermissions     : " << User.Permissions;
    cout << "\n-----------------------------------\n";
}

bool FindClientByAccountNumber(string AccountNumber, vector <sClient> vClients, sClient& Client)
{
    for (sClient C : vClients)
    {

        if (C.AccountNumber == AccountNumber)
        {
            Client = C;
            return true;
        }

    }
    return false;
}

sClient ChangeClientRecord(string AccountNumber)
{
    sClient Client;

    Client.AccountNumber = AccountNumber;

    cout << "\n\nEnter PinCode? ";
    getline(cin >> ws, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> Client.AccountBalance;
    return Client;
}
sUser ChangeUserRecord(string UserName)
{
    sUser User;

    User.UserName = UserName;

    cout << "\n\nEnter PassWord? ";
    getline(cin >> ws, User.PassWord);
    User.Permissions = ReadPermissionsToSet();

    return User;
}

sClient ChangeClientBalance(sClient& Client, int Amount, string AccountNumber, enTransactionMenueOptions OperationKind)
{

    switch (OperationKind)
    {
    case enTransactionMenueOptions::eDeposit:
    {
        Client.AccountBalance += Amount;
        return Client;
        break;
    }
    case enTransactionMenueOptions::eWithdraw:
    {
        if (Client.AccountBalance < Amount * -1)
        {
            cout << "\n !! You do not have enough money !!  your balance is " << Client.AccountBalance << endl;
            return Client;
        }
        else
        {
            Client.AccountBalance += Amount;
            return Client;
        }
        break;
    }
    }

    return Client;
}

bool MarkClientForDeleteByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{

    for (sClient& C : vClients)
    {

        if (C.AccountNumber == AccountNumber)
        {
            C.MarkForDelete = true;
            return true;
        }

    }

    return false;
}
bool MarkUserForDeleteByUserName(string UserName, vector <sUser>& vUsers)
{

    for (sUser& C : vUsers)
    {

        if (C.UserName == UserName)
        {
            C.MarkForDelete = true;
            return true;
        }

    }

    return false;
}

vector <sClient> SaveCleintsDataToFile(string FileName, vector <sClient> vClients)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out);//overwrite

    string DataLine;

    if (MyFile.is_open())
    {
        for (sClient& C : vClients)
        {

            if (C.MarkForDelete == false)
            {
                //we only write records that are not marked for delete.  
                DataLine = ConvertRecordToLine(C);
                MyFile << DataLine << endl;
            }

        }

        MyFile.close();
    }

    return vClients;
}
vector <sUser> SaveUsersDataToFile(string UsersFileName, vector <sUser> vUsers)
{
    fstream UsersFile;
    UsersFile.open(UsersFileName, ios::out);//overwrite

    string DataLine;

    if (UsersFile.is_open())
    {
        for (sUser& C : vUsers)
        {

            if (C.MarkForDelete == false)
            {
                //we only write records that are not marked for delete.  
                DataLine = ConvertUserRecordToLine(C);
                UsersFile << DataLine << endl;
            }

        }

        UsersFile.close();
    }

    return vUsers;
}

void AddDataLineToFile(string FileName, string  stDataLine)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out | ios::app);

    if (MyFile.is_open())
    {

        MyFile << stDataLine << endl;

        MyFile.close();
    }
}

void AddUserClientToFile(string FileName, string  stDataLine)
{
    fstream ClientFile;
    ClientFile.open(FileName, ios::out | ios::app);

    if (ClientFile.is_open())
    {

        ClientFile << stDataLine << endl;

        ClientFile.close();
    }
}

void AddNewClient()
{
    sClient Client;
    Client = ReadNewClient();
    AddDataLineToFile(ClientsFileName, ConvertRecordToLine(Client));
}
void AddNewUser()
{
    sUser User;
    User = ReadNewUser();
    AddDataLineToFile(UsersFileName, ConvertUserRecordToLine(User));
}

void AddNewClients()
{
    char AddMore = 'Y';
    do
    {
        //system("cls");
        cout << "Adding New Client:\n\n";

        AddNewClient();
        cout << "\nClient Added Successfully, do you want to add more clients? Y/N? ";
        cin >> AddMore;

    } while (toupper(AddMore) == 'Y');

}
void AddNewUsers()
{
    char AddMore = 'Y';
    do
    {
        //system("cls");
        cout << "Adding New User:\n\n";

        AddNewUser();
        cout << "\nUser Added Successfully, do you want to add more users? Y/N? ";
        cin >> AddMore;

    } while (toupper(AddMore) == 'Y');

}

bool DeleteClientByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{
    sClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {

        PrintClientCard(Client);

        cout << "\n\nAre you sure you want delete this client? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            MarkClientForDeleteByAccountNumber(AccountNumber, vClients);
            SaveCleintsDataToFile(ClientsFileName, vClients);

            //Refresh Clients 
            vClients = LoadCleintsDataFromFile(ClientsFileName);

            cout << "\n\nClient Deleted Successfully.";
            return true;
        }

    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }
}
bool DeleteUserByUserName(string UserName, vector <sUser>& vUsers)               //Note : continue with the condition that Admin could`nt be deleted
{
    sUser User;
    char Answer = 'n';

    if (FindUserByUserName(UserName, vUsers, User))
    {

        PrintUserCard(User);

        cout << "\n\nAre you sure you want delete this user? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            MarkUserForDeleteByUserName(UserName, vUsers);
            SaveUsersDataToFile(UsersFileName, vUsers);

            //Refresh Clients 
            vUsers = LoadUsersDataFromFile(UsersFileName);

            cout << "\n\nUser Deleted Successfully.";
            return true;
        }

    }
    else
    {
        cout << "\nUser with UserName (" << UserName << ") is Not Found!";
        return false;
    }
}

bool UpdateClientByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{

    sClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {

        PrintClientCard(Client);
        cout << "\n\nAre you sure you want update this client? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            for (sClient& C : vClients)
            {
                if (C.AccountNumber == AccountNumber)
                {
                    C = ChangeClientRecord(AccountNumber);
                    break;
                }
            }

            SaveCleintsDataToFile(ClientsFileName, vClients);

            cout << "\n\nClient Updated Successfully.";
            return true;
        }

    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }
}
bool UpdateUserByUserName(string UserName, vector <sUser>& vUsers)
{

    sUser User;
    char Answer = 'n';

    if (FindUserByUserName(UserName, vUsers, User))
    {

        PrintUserCard(User);
        cout << "\n\nAre you sure you want update this User? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            for (sUser& C : vUsers)
            {
                if (C.UserName == UserName)
                {
                    C = ChangeUserRecord(UserName);
                    break;
                }
            }

            SaveUsersDataToFile(UsersFileName, vUsers);

            cout << "\n\nUser Updated Successfully.";
            return true;
        }

    }
    else
    {
        cout << "\nUser with UserName (" << UserName << ") is Not Found!";
        return false;
    }
}

bool UpdateBalanceClientByAccountNumber(string AccountNumber, vector <sClient>& vClients, enTransactionMenueOptions OperationKind)
{
    sClient Client;
    char Answer = 'n';
    int Amount = 0;
    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientCard(Client);
        if (OperationKind == enTransactionMenueOptions::eDeposit)
        {
            cout << "\n\nAre you sure you want to add money? y/n ? ";
        }
        else
        {
            cout << "\n\nAre you sure you want to withdraw money? y/n ? ";
        }
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            for (sClient& C : vClients)
            {
                if (C.AccountNumber == AccountNumber)
                {
                    cout << "\n\nEnter The amount ";
                    cin >> Amount;
                    if (OperationKind == enTransactionMenueOptions::eDeposit)
                    {

                        C = ChangeClientBalance(Client, Amount, AccountNumber, OperationKind);
                        cout << "\n\n Done Successfully. New Balance is : " << Client.AccountBalance;

                        break;
                    }
                    else
                    {
                        C = ChangeClientBalance(Client, Amount * (-1), AccountNumber, OperationKind);
                        break;
                    }
                }
            }
            SaveCleintsDataToFile(ClientsFileName, vClients);
            return true;
        }
    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }
}

string ReadClientAccountNumber()
{
    string AccountNumber = "";

    cout << "\nPlease enter AccountNumber? ";
    cin >> AccountNumber;
    return AccountNumber;

}
string ReadUserUserName()
{
    string UserName = "";

    cout << "\nPlease enter UserName? ";
    cin >> UserName;
    return UserName;

}

void ShowDeleteClientScreen()
{
    if (!CheckAccessPermission(enPermissionsMenuOptions::eDeleteClietPer))
    {
        ShowAccessDeniedMassage();
        GoBackToMainMenue();
        return;
    }
    cout << "\n-----------------------------------\n";
    cout << "\tDelete Client Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    DeleteClientByAccountNumber(AccountNumber, vClients);
}
void ShowDeleteUserScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tDelete User Screen";
    cout << "\n-----------------------------------\n";

    vector <sUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    string UserName = ReadUserUserName();
    if (UserName != "Admin")
        DeleteUserByUserName(UserName, vUsers);
    else
    {
        cout << "\n You could not delete the Admin \n";
        system("pause>0");
    }
}

void ShowUpdateClientScreen()
{
    if (!CheckAccessPermission(enPermissionsMenuOptions::eUpdateClietPer))
    {
        ShowAccessDeniedMassage();
        GoBackToMainMenue();
        return;
    }
    cout << "\n-----------------------------------\n";
    cout << "\tUpdate Client Info Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    UpdateClientByAccountNumber(AccountNumber, vClients);

}
void ShowUpdateUserScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tUpdate User Info Screen";
    cout << "\n-----------------------------------\n";

    vector <sUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    string UserName = ReadUserUserName();
    UpdateUserByUserName(UserName, vUsers);

}

void ShowAddNewClientsScreen()
{
    if (!CheckAccessPermission(enPermissionsMenuOptions::eAddNewClietPer))
    {
        ShowAccessDeniedMassage();
        GoBackToMainMenue();
        return;
    }
    cout << "\n-----------------------------------\n";
    cout << "\tAdd New Clients Screen";
    cout << "\n-----------------------------------\n";

    AddNewClients();
}
void ShowAddNewUsersScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tAdd New Users Screen";
    cout << "\n-----------------------------------\n";

    AddNewUsers();
}

void ShowFindClientScreen()
{
    if (!CheckAccessPermission(enPermissionsMenuOptions::eFindClietPer))
    {
        ShowAccessDeniedMassage();
        GoBackToMainMenue();
        return;
    }
    cout << "\n-----------------------------------\n";
    cout << "\tFind Client Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    sClient Client;
    string AccountNumber = ReadClientAccountNumber();
    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
        PrintClientCard(Client);
    else
        cout << "\nClient with Account Number[" << AccountNumber << "] is not found!";
}
void ShowFindUserScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tFind User Screen";
    cout << "\n-----------------------------------\n";

    vector <sUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    sUser User;
    string UserName = ReadUserUserName();
    if (FindUserByUserName(UserName, vUsers, User))
        PrintUserCard(User);
    else
        cout << "\nUser with UserName[" << UserName << "] is not found!";
}

void GoBackToMainMenue()
{
    
    cout << "\nPress any Key to return to Main Menue...";
    system("pause>0");
    ShowMainMenue();
}
void GoBackToManageUserMenue()
{
    sUser User;
    cout << "\nPress any Key to return to Manage User Menue...";
    system("pause>0");
    ShowManageUsersMenue();
}

short ReadTransactionMenueOption()
{
    cout << "Choose what do you want to do? [1 to 4]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

void ShowDepositScreen()
{
    system("cls");
    cout << "\n-----------------------------------\n";
    cout << "\tDeposit Client Screen";
    cout << "\n-----------------------------------\n";
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    UpdateBalanceClientByAccountNumber(AccountNumber, vClients, enTransactionMenueOptions::eDeposit);
}
void ShowWithdrawSecreen()
{
    system("cls");
    cout << "\n-----------------------------------\n";
    cout << "\tWithdraw Client Screen";
    cout << "\n-----------------------------------\n";
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    UpdateBalanceClientByAccountNumber(AccountNumber, vClients, enTransactionMenueOptions::eWithdraw);
}
void ShowTotalBalances()
{
    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    double TotalBalance = 0;
    cout << "\n\t\t\t\t\t Balance Client List (" << vClients.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "Accout Number";
    cout << "| " << left << setw(10) << "Client Name";
    cout << "\t\t\t\t\t\t\t" << "| " << "Balance";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n\n" << endl;
    if (vClients.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else
    {
        for (sClient Client : vClients)
        {
            PrintBalanceClientRecordLine(Client);
            TotalBalance += Client.AccountBalance;
        }
        cout << "\n_______________________________________________________";
        cout << "_________________________________________\n" << endl;
        cout << "\t\t\t\t\t\t\t\tTotal Balance = " << setw(16) << left << TotalBalance;
    }
}
void GoBackToTransactionMenue()
{
    cout << "\nPress any Key to return to Tansaction Menue...";
    system("pause>0");
    ShowTransactionMenue();
}
void PerformTransactoinMenueOption(enTransactionMenueOptions TransactionMenueOption)
{
    sUser User;
    switch (TransactionMenueOption)
    {
    case enTransactionMenueOptions::eDeposit:
        ShowDepositScreen();
        GoBackToTransactionMenue();
        break;
    case enTransactionMenueOptions::eWithdraw:
        ShowWithdrawSecreen();
        GoBackToTransactionMenue();
        break;
    case enTransactionMenueOptions::eTotalBalances:
        ShowTotalBalances();
        GoBackToTransactionMenue();
        break;
    case enTransactionMenueOptions::eMainMenue:
        ShowMainMenue();
        break;
    }
}
void ShowTransactionMenue()
{
    system("cls");
    cout << "======================================================" << endl;
    cout << right << setw(16) << "Transaction Manue Screen" << endl;
    cout << "======================================================" << endl;
    cout << setw(6) << left << "[1] Deposit." << endl;
    cout << setw(6) << left << "[2] Withdraw." << endl;
    cout << setw(6) << left << "[3] Totol balances." << endl;
    cout << setw(6) << left << "[4] Main Menue." << endl;
    cout << "======================================================" << endl;

    PerformTransactoinMenueOption((enTransactionMenueOptions)ReadTransactionMenueOption());
}
void  ShowTransactionClientScreen()
{
    system("cls");
    if (!CheckAccessPermission(enPermissionsMenuOptions::eTransactionPer))
    {
        ShowAccessDeniedMassage();
        GoBackToMainMenue();
        return;
    }
    ShowTransactionMenue();
}
void ShowEndScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tProgram Ends :-)";
    cout << "\n-----------------------------------\n";
}

short ReadMainMenueOption()
{
    cout << "Choose what do you want to do? [1 to 8]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}
short ReadManageUsersMenueOption()
{
    cout << "Choose what do you want to do? [1 to 6]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

void PerformMainMenueOption(enMainMenueOptions MainMenueOption)
{

    switch (MainMenueOption)
    {

    case enMainMenueOptions::eListClients:

        system("cls");
        ShowAllClientsScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eAddNewClient:
        system("cls");
        ShowAddNewClientsScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eDeleteClient:
        system("cls");
        ShowDeleteClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eUpdateClient:
        system("cls");
        ShowUpdateClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eFindClient:
        system("cls");
        ShowFindClientScreen();
        GoBackToMainMenue();
        break;
    case enMainMenueOptions::eTransaction:
        system("cls");
        ShowTransactionClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eManageUsers:
        system("cls");
        ShowManageUsersMenue();                //// countinue from here 
        GoBackToMainMenue();
        //ShowEndScreen();
        break;

    case enMainMenueOptions::eLogOut:

        system("cls");
        LogIn();
        break;
    }
}
void PerformUserMenueOption(enManageUsersOptions UserMenueOption)
{

    switch (UserMenueOption)
    {

    case enManageUsersOptions::eListUsers:
    {
        system("cls");
        ShowAllUsersScreen();
        GoBackToManageUserMenue();
        break;
    }
    case enManageUsersOptions::eAddNewUser:
    {
        system("cls");
        ShowAddNewUsersScreen();
        GoBackToManageUserMenue();
        break;
    }


    case enManageUsersOptions::eDeleteUser:
    {
        system("cls");
        ShowDeleteUserScreen();
        GoBackToManageUserMenue();
        break;
    }

    case enManageUsersOptions::eUpdateUser:
    {
        system("cls");
        ShowUpdateUserScreen();
        GoBackToManageUserMenue();
        break;
    }

    case enManageUsersOptions::eFindUser:
    {
        system("cls");
        ShowFindUserScreen();
        GoBackToManageUserMenue();
        break;
    }

    case enManageUsersOptions::eHomeMenue:
    {
        system("cls");
        GoBackToMainMenue();
        break;
    }
    }
}

void ShowMainMenue()
{
    system("cls");
    cout << "======================================================" << endl;
    cout << right << setw(16) << "Main Manue Screen" << endl;
    cout << "======================================================" << endl;
    cout << setw(6) << left << "[1] Show Client List." << endl;
    cout << setw(6) << left << "[2] Add New Client." << endl;
    cout << setw(6) << left << "[3] Delete Client." << endl;
    cout << setw(6) << left << "[4] Update Client Info." << endl;
    cout << setw(6) << left << "[5] Find Client." << endl;
    cout << setw(6) << left << "[6] Transactions." << endl;
    cout << setw(6) << left << "[7] Manage Users." << endl;
    cout << setw(6) << left << "[8] Logout." << endl;
    cout << "======================================================" << endl;

    PerformMainMenueOption((enMainMenueOptions)ReadMainMenueOption());
}
void ShowManageUsersMenue()
{
    system("cls");
    if (!CheckAccessPermission(enPermissionsMenuOptions::eManageUsersPer))
    {
        ShowAccessDeniedMassage();
        GoBackToMainMenue();
        return;
    }
    cout << "======================================================" << endl;
    cout << right << setw(16) << "Manage Users Menue Screen" << endl;
    cout << "======================================================" << endl;
    cout << setw(6) << left << "[1] List Users." << endl;
    cout << setw(6) << left << "[2] Add New Users." << endl;
    cout << setw(6) << left << "[3] Delete User." << endl;
    cout << setw(6) << left << "[4] Update User." << endl;
    cout << setw(6) << left << "[5] Find User." << endl;
    cout << setw(6) << left << "[6] Main Menue." << endl;

    cout << "======================================================" << endl;

    PerformUserMenueOption((enManageUsersOptions)ReadManageUsersMenueOption());
}

int main()
{
    //****Uncomment the following just in the fist run. and then comment them*****
    
    //    fstream MyFile;     MyFile.open("Clients.txt", ios::out);//Write Mode
    //if (MyFile.is_open())    
    //{ 
    //    MyFile << "Q004//664//Obama//044777//3000\n"; 
    //    MyFile << "Q001//4466//LoLo//7799955//4000\n";   
    //    MyFile << "Q009//6600//Vinex//0666644//4000\n";
    //    MyFile << "Q010//211//Spongpop//033054666//9000\n";
    //    MyFile << "G001//46444//Crap//04455888//100000\n";
    //    MyFile.close(); 
    //}
    //fstream UsersFile;    
    //UsersFile.open("Users.txt", ios::out);//Write Mode
    //if (UsersFile.is_open())    
    //{ 
    //    UsersFile << "Admin//1234//-1\n";
    //    UsersFile.close(); 
    //}
    LogIn();
    system("pause>0");
    return 0;
}
