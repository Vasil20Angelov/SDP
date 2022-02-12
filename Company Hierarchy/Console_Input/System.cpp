#include "System.h"

System::System()
{
}

void System::Run()
{
    cout << "\t\t\t\tWelcome to the system!\nType help to display manual.\n\n";
    string input = "";
    cout << "> ";
    while (std::getline(cin, input))
    {
        if (input == "exit")
            break;

        operation(input);

        cin.clear();
        cout << "\n> ";
    }

    std::forward_list<Data*> modLst = branches.getModified();
    for (Data* element : modLst)
        saveOnExit(element);

    cout << "\nExited.\n";
}

void System::operation(const string& input)
{
    int pos = 0;
    string option = extract(input, ' ', pos); // The first string in each input is the command. The rest are parameters

    if (option == "load") {
        load(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "save") {
        save(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "find") {
        find(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "num_subordinates") {
        num_subordinates(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "manager") {
        manager(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "num_employees") {
        num_employees(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "overloaded") {
        overloaded(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "join") {
        join(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "fire") {
        fire(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

	if (option == "hire") {
		hire(clearWhiteSpace(input.substr(pos, input.size())));
		return;
	}

    if (option == "salary") {
        salary(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "incorporate") {
        incorporate(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "modernize") {
        modernize(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "longest_chain") {
        longest_chain(clearWhiteSpace(input.substr(pos, input.size())));
        return;
    }

    if (option == "help") {
        help();
        return;
    }

    cout << "Invalid command!\n";
}

void System::help()
{
    cout << "\t\t\t\t\tManual\n\n";
    cout << "Command:          Parameters:                         Information:\n";
    cout << "load             'object' [file_name]              -> load a new branch (firm/corporation/object...)\n";
    cout << "save             'object' [file_name]              -> save a branch into a file or display it on the screen\n";
    cout << "find             'object' 'subordinate'            -> check if there is a subordinate working in that object\n";
    cout << "manager          'object' 'subordinate'            -> show the direct manager of a subordinate\n";
    cout << "salary           'object' 'subordinate'            -> show the salary of a subordinate\n";
    cout << "num_subordinates 'object' 'subordinate'            -> show the number of subordinates of that person\n";
    cout << "fire             'object' 'subordinate'            -> fire a person from the object\n";
    cout << "hire             'object' 'subordinate' 'manager'  -> hire a person or changes his current manager\n";
    cout << "num_employees    'object'                          -> show how many people are working for that object\n";
    cout << "overloaded       'object'                          -> show how many people in the object are overloaded\n";
    cout << "longest_chain    'object'                          -> show how many people are in the longest relations chain\n";
    cout << "incorporate      'object'                          -> incorporate an object\n";
    cout << "modernize        'object'                          -> modernize an object\n";
    cout << "join             'object1' 'object2' 'object3'     -> unite 2 objects into another (not already existing)\n";
    cout << "help                                               -> display help menu\n\n";
    cout << "exit                                               -> exit the system\n";
    cout << "********************************************************\n";
    cout << "The first string is the command.\n";
    cout << "The command is executed on the parameters in the quotes.\n";
    cout << "The parameter in square brackets is optional.\n";
    cout << "Only usage of .txt files are allowed.\n";
    cout << "********************************************************\n\n";
}

void System::find(const string& input)
{
    int pos = 0;
    string branchName = extract(input, ' ', pos);
    string subordinate = input.substr(pos, input.size());

    if (branchName.empty() || subordinate.empty()) {
        cout << "Invalid command!\n";
        return;
    }

    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    if (branches.find(branchName)->fValue.find(subordinate))
        cout << subordinate << " is employed in " << branchName << ".\n";
    else
        cout<< subordinate << " is not found in " << branchName << ".\n";
}

void System::num_subordinates(const string& input)
{
    int pos = 0;
    string branchName = extract(input, ' ', pos);
    string subordinate = input.substr(pos, input.size());

    if (branchName.empty() || subordinate.empty()) {
        cout << "Invalid command!\n";
        return;
    }

    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    int subCount = branches.find(branchName)->fValue.num_subordinates(subordinate);
    if (subCount == 0)
        cout << subordinate << " is not found in " << branchName << " or has 0 subordinates.\n";
    else
        cout<< subordinate << " has "<< subCount <<" subordinates.\n";

}

void System::manager(const string& input)
{
    int pos = 0;
    string branchName = extract(input, ' ', pos);
    string subordinate = input.substr(pos, input.size());

    if (branchName.empty() || subordinate.empty()) {
        cout << "Invalid command!\n";
        return;
    }

    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    if (subordinate == BOSS) {
        cout << BOSS << " is the boss!\n";
        return;
    }

    string manager = branches.find(branchName)->fValue.manager(subordinate);
    if (manager == "")
        cout << "There is no " << subordinate << " in " << branchName << ".\n";
    else
        cout << "The manager of " << subordinate << " is " << manager << ".\n";
}

void System::num_employees(const string& branchName)
{
    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    cout << "There are " << branches.find(branchName)->fValue.num_employees() << " employees in " << branchName << ".\n";
}

void System::overloaded(const string& branchName)
{
    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    cout << "There are " << branches.find(branchName)->fValue.num_overloaded() << " overloaded employees in " << branchName << ".\n";
}

void System::join(const string& input)
{
    int pos = 0;
    string branch1 = extract(input, ' ', pos);
    string branch2 = extract(input, ' ', pos);
    string newBranch = input.substr(pos, input.size());

    if (branch1.empty() || branch2.empty() || newBranch.empty()) {
        cout << "Invalid command!\n";
        return;
    }

    if (!branches.contains(branch1)) {
        cout << branch1 << " is not found!\n";
        return;
    }

    if (!branches.contains(branch2)) {
        cout << branch2 << " is not found!\n";
        return;
    }

    bool newObj = false;
    if (!branches.contains(newBranch)) {
        newObj = true;
        if (!valdidName(newBranch)) {
            cout << newBranch << " is invalid branch name!\n";
            return;
        }
    }

    try {
        Hierarchy newHierarchy = branches.find(branch1)->fValue.join(branches.find(branch2)->fValue);
        if (!newHierarchy.find(BOSS)) {
            cout << "Error! The 2 hierarchies have not been united!\n";
            return;
        }

        if (!newObj)
            branches.erase(newBranch);

        branches.insert(Data(newBranch, newHierarchy, true));
        cout << newBranch << " has been created!\n";
    }
    catch (std::exception& e) {
        cout << "Error! " << e.what() << "\n"<<newBranch << " has not been created!\n";
    }
}

void System::fire(const string& input)
{
    int pos = 0;
    string branchName = extract(input, ' ', pos);
    string subordinate = input.substr(pos, input.size());

    if (branchName.empty() || subordinate.empty()) {
        cout << "Invalid command!\n";
        return;
    }

    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    if (subordinate == BOSS) {
        cout << BOSS << " cannot be fired!\n";
        return;
    }

    Data* toModify = branches.find(branchName);
    if (toModify->fValue.fire(subordinate)) {
        toModify->modified = true;
        cout << subordinate << " was fired.\n";
    }
    else
        cout << subordinate << " is not found in " << branchName << ".\n";
}

void System::hire(const string& input)
{
    int pos = 0;
    string branchName = extract(input, ' ', pos);
    string newWorker = extract(input, ' ', pos);
    string manager = input.substr(pos, input.size());

    if (branchName.empty() || manager.empty() || newWorker.empty()) {
        cout << "Invalid command!\n";
        return;
    }

    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    Data* toModify = branches.find(branchName);
    if (toModify->fValue.hire(newWorker, manager)) {
        toModify->modified = true;
        cout << newWorker << " has been hired as subordinate of " << manager << "!\n";
    }
    else
        cout<< newWorker << " has not been hired!\n";
}

void System::salary(const string& input)
{
    int pos = 0;
    string branchName = extract(input, ' ', pos);
    string subordinate = input.substr(pos, input.size());

    if (branchName.empty() || subordinate.empty()) {
        cout << "Invalid command!\n";
        return;
    }

    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    long salary = branches.find(branchName)->fValue.getSalary(subordinate);
    if (salary >= 0)
        cout << "The salary of " << subordinate << " is " << salary << ".\n";
    else
        cout << "There is no subordinate " << subordinate << " in " << branchName << ".\n";
}

void System::incorporate(const string& branchName)
{
    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    Data* data = branches.find(branchName);
    data->fValue.incorporate();
    data->modified = true;

    cout << branchName << " was incorporated!\n";
}

void System::modernize(const string& branchName)
{
    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    Data* data = branches.find(branchName);
    data->fValue.modernize();
    data->modified = true;

    cout << branchName << " was modernized!\n";
}

void System::longest_chain(const string& branchName)
{
    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return;
    }

    cout << "The longest chain of relations has " << branches.find(branchName)->fValue.longest_chain() << " employees.\n";
}

void System::load(const string& input)
{
    int pos = 0;
    string branchName = extract(input, ' ', pos);
    string fileName = input.substr(pos, input.size());

    if (branchName.empty()) {
        cout << "Invalid command!\n";
        return;
    }

    bool newBranch = false;
    if (!branches.contains(branchName)) {
        newBranch = true;
        if (!valdidName(branchName)) {
            cout << branchName << " is invalid branch name!\n";
            return;
        }
    }

    if (!fileName.empty()) {
        loadFromFile(branchName, fileName, newBranch);
        return;
    }

    try {
        string rel;
        std::getline(cin, rel);
        Hierarchy branch(rel);
        Hierarchy::Relation relation;
        while (std::getline(cin, rel))
        {
            int pos = 0;
            relation = branch.extractPair(rel + '\n', pos);
            if (!relation.valid() || !branch.insert(relation.subordinate, relation.manager)) {
                cout << "Invalid input! Data has not been loaded!\n";
                return;
            }
        }

        if (!newBranch)
            branches.erase(branchName);
      
        branches.insert(Data(branchName, branch, true));
        cout << "Succesfully loaded " << branchName << "!\n";
    }
    catch (std::exception& e) {
        cout << e.what() << "\nData has not been loaded!\n";
    }
}

void System::loadFromFile(const string& branchName, const string& fileName, bool newBranch)
{
    std::ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Couldn't open " << fileName << " for reading!\n";
        return;
    }

    string fileData;
    Hierarchy::Relation relation;

    try {
        std::getline(file, fileData);
        Hierarchy branch(fileData + '\n');

        while (!file.eof()) {
            std::getline(file, fileData);
            if (fileData == "")
                continue;
            int index = 0;
            relation = branch.extractPair(fileData + '\n', index);
            if (!relation.valid() || !branch.insert(relation.subordinate, relation.manager)) {
                cout << "Invalid input! Data has not been loaded!\n";
                file.close();
                return;
            }
        }

        if (!newBranch)
            branches.erase(branchName);

        branches.insert(Data(branchName, branch));
        cout << "Succesfully loaded " << branchName << "!\n";
    }
    catch (std::exception& e) {
        cout << e.what() << "\nData has not been loaded!\n";
    }

    file.close();
}

bool System::save(const string& input)
{
    int pos = 0;
    string branchName = extract(input, ' ', pos);
    string fileName = input.substr(pos, input.size());

    if (branchName.empty()) {
        cout << "Invalid command!\n";
        return false;
    }

    if (!branches.contains(branchName)) {
        cout << "The branch " << branchName << " has not been found!\n";
        return false;
    }

    Data* toSave = branches.find(branchName);
    if (fileName.empty())
    {
        cout << toSave->fValue.print();
        return true;
    }

    if (saveToFile(toSave, fileName))
        return true;
 
    return false;
}

bool System::saveToFile(Data*& toSave, const string& fileName)
{
    if (!vallidFileName(fileName)) {
        cout << "Invalid file name!\n";
        return false;
    }

    std::ofstream file(fileName.size() > 4 && fileName[fileName.size() - 4] == '.' ? fileName : fileName + ".txt"); // Append .txt if not added
    if (!file.is_open()) {
        cout << "Couldn't open the file for writing\n";
        return false;
    }

    string output = toSave->fValue.print();
    int pos = 0;
    int size = output.size();
    while (pos < size) {
        string line = extract(output, '\n', pos);
        file << line + '\n';
    }

    file.close();
    toSave->modified = false;
    cout << toSave->fKey << " was successfully saved!\n";

    return true;
}

void System::saveOnExit(Data*& data)
{
    if (!data->modified)
        return;

    cout << '\n' << data->fKey << " is not saved! Type 'y' or 'Y' to save it\n*Note that any other input will not save the object!\n> ";
    string input;
    getline(cin, input); // using getline instead of cin to not mess the other objects with wrong input

    while (input == "y" || input == "Y") {
        cout << "Enter file name > ";
        getline(cin, input);

        if (saveToFile(data, input))
            return;
        
        cout << "Do you want to try again?\n> ";
        getline(cin, input);
    }

    cout << data->fKey << " was not saved!\n\n";
}

bool System::valdidName(const string& objectName)
{
    for (char c : objectName)
    {
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
            continue;

        return false;
    }

    return true;
}

bool System::vallidFileName(const string& fileName)
{
    // Only .txt extension is allowed. On file names without extension will be appeneded .txt
    if (fileName.empty())
        return false;

    int pos = 0;
    while (pos < fileName.size() && fileName[pos] != '.') {
        ++pos;
    }

    if (pos == 0)
        return false;

    if (pos == fileName.size())
        return true;

    if (pos + 4 != fileName.size())
        return false;

    if (fileName[pos + 1] != 't' || fileName[pos + 2] != 'x' || fileName[pos + 3] != 't')
        return false;

    return true;
}

string System::clearWhiteSpace(const string& input)
{
    int pos = input.size();
    if (input[pos - 1] != ' ')
        return input;
   
    pos -= 2; // skip '\0' and the last white space
    while (input[pos] == ' ') {
        --pos;
    }
    
    return string(input.c_str(), input.c_str() + pos + 1);
}

string System::extract(const string& input, char delimiter, int& pos)
{
    // Skip the white space before each word
    while (pos < input.size() && input[pos] == delimiter) {
        ++pos;
    }

    // Take characters until the delimiter is found or till the end of the given input
    string firstWord = "";
    while (pos < input.size() && input[pos] != delimiter) {
        firstWord += input[pos++];
    }

    // Skip all delimiters after the word
    while (pos < input.size() && input[pos] == delimiter) {
        ++pos;
    }

    return firstWord;
}
