#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

// Class representing a person
class Person
{
public:
    string firstName;
    string lastName;
    string email;

    Person() : firstName(""), lastName(""), email("") {}
    Person(const string &first, const string &last, const string &mail)
        : firstName(first), lastName(last), email(mail) {}
};

// Class representing a library member
class Member
{
public:
    static int nextMemberID;
    int memberID;
    Person person;
    string address;
    vector<int> borrowedBooks;
    vector<time_t> dueDates;

    Member() : memberID(0), address("") {}
    Member(const Person &p, const string &addr)
        : memberID(generateRandomID()), person(p), address(addr) {}

    int generateRandomID()
    {
        return rand() % 900 + 100; // Generates a random 3-digit number
    }
};

// Initialize the static member variable
int Member::nextMemberID = 100;

// Class representing a book
class Book
{
public:
    int BookID;
    string BookName, AuthorFirstName, AuthorLastName, BookType;
    int PageCount;
};

// Function declarations
void loadBooksFromFile(vector<Book> &books);
void displayAvailableBooks(const vector<Book> &books);
void saveMembersToFile(const vector<Member> &members);
void loadMembersFromFile(vector<Member> &members);
void displayMemberInformation(const vector<Member> &members);
void issueBookToMember(vector<Member> &members, const vector<Book> &books);
void returnBookFromMember(vector<Member> &members);
void displayBooksBorrowedByMember(const vector<Member> &members, const vector<Book> &books);
float calculateFine(time_t dueDate, time_t returnDate);

// Function to load book details from a file
void loadBooksFromFile(vector<Book> &books)
{
    ifstream file("library_books.csv");

    if (file.is_open())
    {
        books.clear(); // Clear existing data

        Book book;
        string line;
        getline(file, line);
        while (getline(file, line))
        {
            stringstream ss(line);
            ss >> book.BookID;
            ss.ignore();
            getline(ss, book.BookName, ',');
            ss >> book.PageCount;
            ss.ignore();
            getline(ss, book.AuthorFirstName, ',');
            getline(ss, book.AuthorLastName, ',');
            getline(ss, book.BookType);

            books.push_back(book);
        }

        file.close();
        cout << "Book details loaded from file.\n";
    }
    else
    {
        cout << "Error: Unable to open the file for reading.\n";
    }
}

// Function to display available books
void displayAvailableBooks(const vector<Book> &books)
{
    cout << "Available Books:\n";
    for (const auto &book : books)
    {
        cout << "ID: " << book.BookID << ", Title: " << book.BookName
             << ", Page count: " << book.PageCount
             << ", Author: " << book.AuthorFirstName << " " << book.AuthorLastName
             << ", Type: " << book.BookType << "\n";
    }
}

// Function to save member details to a file
void saveMembersToFile(const vector<Member> &members)
{
    ofstream file("member_details.txt");

    if (file.is_open())
    {
        for (const auto &member : members)
        {
            file << "Member," << member.memberID << "," << member.person.firstName << ","
                 << member.person.lastName << "," << member.person.email << "," << member.address << "\n";
        }

        // Save nextMemberID at the end of the file
        file << "NextMemberID," << Member::nextMemberID << "\n";

        file.close();
        cout << "Member details saved to file.\n";
    }
    else
    {
        cout << "Error: Unable to open the file for writing.\n";
    }
}

// Function to load member details from a file
void loadMembersFromFile(vector<Member> &members)
{
    ifstream file("member_details.txt");

    if (file.is_open())
    {
        members.clear();

        Member member;
        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string type;
            ss >> type;

            if (type == "Member")
            {
                ss >> member.memberID >> member.person.firstName >> member.person.lastName >> member.person.email >> member.address;
                members.push_back(member);
            }
            else if (type == "NextMemberID")
            {
                ss >> Member::nextMemberID;
            }
        }

        file.close();
        cout << "Member details loaded from file.\n";
    }
    else
    {
        cout << "No saved member data found.\n";
    }
}

// Function to display member information
void displayMemberInformation(const vector<Member> &members)
{
    if (members.empty())
    {
        cout << "No members found.\n";
    }
    else
    {
        cout << "\n---------------------------\n";
        cout << "Member Information:\n";
        for (const auto &member : members)
        {
            cout << "ID: " << member.memberID << ", Name: " << member.person.firstName
                 << " " << member.person.lastName << ", Address: " << member.address << ", Email: " << member.person.email << "\n";
        }
    }
}

// Function to issue a book to a member
void issueBookToMember(vector<Member> &members, const vector<Book> &books)
{
    int memberID, bookID;
    cout << "Enter Member ID: ";
    cin >> memberID;

    cout << "Enter Book ID: ";
    cin >> bookID;

    for (auto &member : members)
    {
        if (member.memberID == memberID)
        {
            auto it = find_if(books.begin(), books.end(), [bookID](const Book &b) { return b.BookID == bookID; });

            if (it != books.end())
            {
                member.borrowedBooks.push_back(bookID);

                time_t now = time(nullptr);
                time_t dueDate = now + (3 * 24 * 60 * 60);
                member.dueDates.push_back(dueDate);

                cout << "\n---------------------------\n";
                cout << "Book issued successfully to member " << memberID << ".\n";
                saveMembersToFile(members);
                return;
            }
            else
            {
                cout << "Book not found.\n";
                return;
            }
        }
    }

    cout << "Member not found.\n";
}

// Function to return a book from a member
void returnBookFromMember(vector<Member> &members)
{
    int memberID, bookID;
    cout << "Enter Member ID: ";
    cin >> memberID;

    cout << "Enter Book ID: ";
    cin >> bookID;

    for (auto &member : members)
    {
        if (member.memberID == memberID)
        {
            auto it = find(member.borrowedBooks.begin(), member.borrowedBooks.end(), bookID);

            if (it != member.borrowedBooks.end())
            {
                int index = static_cast<int>(distance(member.borrowedBooks.begin(), it));

                string returnDateStr;
                cout << "Enter return date (DD-MM-YYYY): ";
                cin.ignore();
                getline(cin, returnDateStr);

                tm tm = {};
                istringstream ss(returnDateStr);
                ss >> get_time(&tm, "%d-%m-%Y");

                if (ss.fail())
                {
                    cout << "Invalid date format. Please enter date in DD-MM-YYYY format.\n";
                    return;
                }

                time_t returnDate = mktime(&tm);
                time_t now = time(nullptr);

                float fine = calculateFine(member.dueDates[index], returnDate);

                if (fine > 0.0)
                {
                    cout << "\n---------------------------\n";
                    cout << "Fine of £" << fine << " charged for overdue book.\n";
                }
                else
                {
                    cout << "\n---------------------------\n";
                    cout << "Book returned on time.\n";
                }

                member.borrowedBooks.erase(it);
                member.dueDates.erase(member.dueDates.begin() + index);
                saveMembersToFile(members);
                return;
            }
            else
            {
                cout << "Member did not borrow this book.\n";
                return;
            }
        }
    }

    cout << "Member not found.\n";
}

// Function to display books borrowed by a member
void displayBooksBorrowedByMember(const vector<Member> &members, const vector<Book> &books)
{
    int memberID;
    cout << "Enter Member ID: ";
    cin >> memberID;

    for (const auto &member : members)
    {
        if (member.memberID == memberID)
        {
            cout << "\n---------------------------\n";
            cout << "Books borrowed by member " << memberID << ":\n";
            for (int i = 0; i < member.borrowedBooks.size(); ++i)
            {
                int bookID = member.borrowedBooks[i];
                auto it = find_if(books.begin(), books.end(), [bookID](const Book &b) { return b.BookID == bookID; });

                if (it != books.end())
                {
                    cout << "ID: " << it->BookID << ", Title: " << it->BookName
                         << ", Author: " << it->AuthorFirstName << ", Type: " << it->BookType << "\n";
                }
            }
            return;
        }
    }

    cout << "Member not found.\n";
}

// Function to calculate fine for an overdue book
float calculateFine(time_t dueDate, time_t returnDate)
{
    const float fineRate = 1.0;
    int daysOverdue = max(0, static_cast<int>((returnDate - dueDate + 86399) / (24 * 60 * 60)));
    return fineRate * daysOverdue;
}

// Main program
int main()
{
    vector<Book> availableBooks;
    vector<Member> members;

    loadBooksFromFile(availableBooks);
    loadMembersFromFile(members);

    int choice;

    do
    {
        cout << "\n---------------------------\n";
        cout << "\nLibrary System Menu:\n";
        cout << "1. Display Available Books\n";
        cout << "2. Display Member Information\n";
        cout << "3. Add Member\n";
        cout << "4. Issue a Book to a Member\n";
        cout << "5. Return a Book\n";
        cout << "6. Display Books Borrowed by a Member\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            displayAvailableBooks(availableBooks);
            break;
        case 2:
            displayMemberInformation(members);
            break;
        case 3:
        {
            string name, address, email;
            cout << "Enter Name: ";
            cin.ignore();
            getline(cin, name);

            cout << "Enter Address: ";
            getline(cin, address);

            cout << "Enter Email: ";
            getline(cin, email);

            Person person(name, "", email);
            Member newMember(person, address);

            members.push_back(newMember);
            cout << "\n---------------------------\n";
            cout << "Member successfully added. Member ID: " << newMember.memberID << "\n";
            saveMembersToFile(members);
            break;
        }
        case 4:
            issueBookToMember(members, availableBooks);
            break;
        case 5:
            returnBookFromMember(members);
            break;
        case 6:
            displayBooksBorrowedByMember(members, availableBooks);
            break;
        case 0:
            cout << "Exiting the program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please enter a valid option.\n";
        }

    } while (choice != 0);

    return 0;
}
