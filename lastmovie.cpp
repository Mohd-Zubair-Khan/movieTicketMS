#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <cstdlib>  

using namespace std;

class Movie {
public:
    string name;
    int totalSeats;
    int availableSeats;
    double ticketPrice;

    Movie(string n, int total, double price) : name(n), totalSeats(total), availableSeats(total), ticketPrice(price) {}
};

class User {
public:
    string username;
    string password;
    string email;
    int age;

    User(string u, string p, string e, int a) : username(u), password(p), email(e), age(a) {}
};

class Booking {
public:
    string movieName;
    int numberOfTickets;
    double totalAmount;

    Booking(string movie, int numTickets, double amount) : movieName(movie), numberOfTickets(numTickets), totalAmount(amount) {}
};

class BookingSystem {
private:
    vector<Movie> movies;
    vector<User> users;
    vector<Booking> bookings;
    User* currentUser;

    // File paths
    const string userFilePath = "users.txt";
    const string movieFilePath = "movies.txt";
    const string bookingFilePath = "bookings.txt";

    void loadUsersFromFile() {
        ifstream file(userFilePath);
        if (file.is_open()) {
            users.clear();
            string username, password, email;
            int age;

            while (file >> username >> password >> email >> age) {
                users.push_back(User(username, password, email, age));
            }

            file.close();
        }
    }

    void saveUsersToFile() {
        ofstream file(userFilePath);
        if (file.is_open()) {
            for (const User& user : users) {
                file << user.username << " " << user.password << " " << user.email << " " << user.age << "\n";
            }

            file.close();
        }
    }

    void loadMoviesFromFile() {
        ifstream file(movieFilePath);
        if (file.is_open()) {
            movies.clear();
            string name;
            int totalSeats;
            double ticketPrice;

            while (file >> name >> totalSeats >> ticketPrice) {
                movies.push_back(Movie(name, totalSeats, ticketPrice));
            }

            file.close();
        }
    }

    void saveMoviesToFile() {
        ofstream file(movieFilePath);
        if (file.is_open()) {
            for (const Movie& movie : movies) {
                file << movie.name << " " << movie.totalSeats << " " << movie.ticketPrice << "\n";
            }

            file.close();
        }
    }

    void loadBookingsFromFile() {
        ifstream file(bookingFilePath);
        if (file.is_open()) {
            bookings.clear();
            string movieName;
            int numTickets;
            double totalAmount;

            while (file >> movieName >> numTickets >> totalAmount) {
                bookings.push_back(Booking(movieName, numTickets, totalAmount));
            }

            file.close();
        }
    }

    void saveBookingsToFile() {
        ofstream file(bookingFilePath);
        if (file.is_open()) {
            for (const Booking& booking : bookings) {
                file << booking.movieName << " " << booking.numberOfTickets << " " << booking.totalAmount << "\n";
            }

            file.close();
        }
    }

public:
    BookingSystem() : currentUser(nullptr) {
        // Load data from files on system startup
        loadUsersFromFile();
        loadMoviesFromFile();
        loadBookingsFromFile();
    }

    ~BookingSystem() {
        // Save data to files before exiting
        saveUsersToFile();
        saveMoviesToFile();
        saveBookingsToFile();
    }

    void displayMovies() {
        cout << "Available Movies:\n";
        for (size_t i = 0; i < movies.size(); ++i) {
            cout << i + 1 << ". " << movies[i].name << " (" << movies[i].availableSeats << " seats available) - $"
                 << fixed << setprecision(2) << movies[i].ticketPrice << " per ticket\n";
        }
    }

    bool isValidEmail(const string& email) {
        size_t atPos = email.find('@');
        size_t dotPos = email.rfind('.');

        return (atPos != string::npos && dotPos != string::npos && atPos < dotPos);
    }

    bool isValidAge(int age) {
        return age > 0 && age <= 100;  // Assuming a reasonable age range
    }

    void signup(string username, string password, string email, int age) {
        if (isValidEmail(email) && isValidAge(age)) {
            users.push_back(User(username, password, email, age));
            cout << "Signup successful! Please log in.\n";
        } else {
            cout << "Invalid email format or age. Please provide a valid email address and age.\n";
        }

        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    User* login(string username, string password) {
        for (User& user : users) {
            if (user.username == username && user.password == password) {
                currentUser = &user;
                return &user;
            }
        }
        return nullptr;
    }

    void bookTickets() {
        displayMovies();
        int movieChoice;
        int numTickets;

        cout << "Enter the number corresponding to the movie you want to book: ";
        cin >> movieChoice;

        if (cin.fail() || movieChoice < 1 || movieChoice > static_cast<int>(movies.size())) {
            cout << "Invalid input. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        Movie& selectedMovie = movies[movieChoice - 1];

        cout << "Enter the number of tickets to book (available seats: " << selectedMovie.availableSeats << "): ";
        cin >> numTickets;

        if (cin.fail() || numTickets < 1 || numTickets > selectedMovie.availableSeats) {
            cout << "Invalid input. Please enter a valid number of tickets.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        double totalAmount = numTickets * selectedMovie.ticketPrice;

        // Update available seats
        selectedMovie.availableSeats -= numTickets;

        // Add booking record
        bookings.push_back(Booking(selectedMovie.name, numTickets, totalAmount));

        cout << "Booking successful!\n";
        cout << "Total Amount: $" << fixed << setprecision(2) << totalAmount << "\n";

        // Save data to files after booking
        saveMoviesToFile();
        saveBookingsToFile();

        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void cancelBooking() {
        if (bookings.empty()) {
            cout << "No bookings to cancel.\n";
            return;
        }

        cout << "Your Bookings:\n";
        for (size_t i = 0; i < bookings.size(); ++i) {
            cout << i + 1 << ". Movie: " << bookings[i].movieName << ", Tickets: " << bookings[i].numberOfTickets
                 << ", Total Amount: $" << fixed << setprecision(2) << bookings[i].totalAmount << "\n";
        }

        int cancelChoice;
        cout << "Enter the number corresponding to the booking you want to cancel: ";
        cin >> cancelChoice;

        if (cin.fail() || cancelChoice < 1 || cancelChoice > static_cast<int>(bookings.size())) {
            cout << "Invalid input. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        Booking& selectedBooking = bookings[cancelChoice - 1];

        // Refund the amount and update available seats
        for (Movie& movie : movies) {
            if (movie.name == selectedBooking.movieName) {
                movie.availableSeats += selectedBooking.numberOfTickets;
                break;
            }
        }

        cout << "Booking canceled. Refund Amount: $" << fixed << setprecision(2) << selectedBooking.totalAmount << "\n";

        // Remove booking record
        bookings.erase(bookings.begin() + cancelChoice - 1);
        // Save data to files after canceling booking
        saveMoviesToFile();
        saveBookingsToFile();
        // Save data to files after canceling booking
        saveMoviesToFile();
        saveBookingsToFile();

        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    // Admin login function to add movies
    void adminLogin() {
        string adminUsername = "admin";
        string adminPassword = "admin";

        cout << "Enter admin username: ";
        string username;
        cin >> username;

        cout << "Enter admin password: ";
        string password;
        cin >> password;

        if (username == adminUsername && password == adminPassword) {
            addMovie();
        } else {
            cout << "Invalid admin credentials. Access denied.\n";
        }

        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

    // Function to add a new movie by the admin
    void addMovie() {
        string name;
        int totalSeats;
        double ticketPrice;

        cout << "Enter movie name: ";
        cin >> name;

        cout << "Enter total seats: ";
        cin >> totalSeats;

        cout << "Enter ticket price: ";
        cin >> ticketPrice;

        movies.push_back(Movie(name, totalSeats, ticketPrice));

        // Save data to file after adding a movie
        saveMoviesToFile();

        cout << "Movie added successfully!\n";
    }
};

int main() {
    BookingSystem bookingSystem;

    int choice;
    string username, password, email;
    int age;

    do {
        cout << "\n1. Display Movies\n2. Signup\n3. Login\n4. Book Tickets\n5. Cancel Booking\n6. Admin Login\n7. Exit\n";
        do {
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail() || choice < 1 || choice > 7) {
                cout << "Invalid input. Please enter a valid number.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while (cin.fail() || choice < 1 || choice > 7);

        // Removed clearScreen

        
        switch (choice) {
            case 1:
                bookingSystem.displayMovies();
                break;

            case 2:
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter password: ";
                cin >> password;
                cout << "Enter email: ";
                cin >> email;
                cout << "Enter age: ";
                cin >> age;

                bookingSystem.signup(username, password, email, age);
                break;

            case 3:
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter password: ";
                cin >> password;

                if (bookingSystem.login(username, password)) {
                    cout << "Login successful! Welcome, " << username << "!\n";
                } else {
                    cout << "Invalid username or password.\n";
                }

                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
                break;

            case 4:
                bookingSystem.bookTickets();
                break;

            case 5:
                bookingSystem.cancelBooking();
                break;

            case 6:
                bookingSystem.adminLogin();
                break;

            case 7:
                cout << "Exiting the system. Thank you!\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 7);

    return 0;
}


