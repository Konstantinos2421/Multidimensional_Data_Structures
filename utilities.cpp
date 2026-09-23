#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <random>
#include <unordered_map>
#include <functional>
#include <utility>
#include <chrono>

// Constants for +infinity and -infinity for different data types
#define MIN_INT std::numeric_limits<int>::min()
#define MAX_INT std::numeric_limits<int>::max()
#define MIN_DOUBLE std::numeric_limits<double>::min()
#define MAX_DOUBLE std::numeric_limits<double>::max()
#define MIN_ULONG std::numeric_limits<unsigned long>::min()
#define MAX_ULONG std::numeric_limits<unsigned long>::max()
#define MIN_DATE Date("0000-01-01")
#define MAX_DATE Date("9999-12-31")

using namespace std;


// djb2 hash function for converting strings to unsigned long integers
unsigned long stringToInt(const string &str){
    unsigned long hash = 5381;
    for (unsigned char c : str) {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash;
}


class Date{
    public:
        int day;
        int month;
        int year;

    Date(){}

    Date(string date){
        // Parsing the date string in the format "YYYY-MM-DD" and initializing the Date object
        string month_str, day_str, year_str;
        stringstream data(date);
        getline(data, year_str, '-');
        getline(data, month_str, '-');
        getline(data, day_str);
        this->day = stoi(day_str);
        this->month = stoi(month_str);
        this->year = stoi(year_str);
    }

    string toString(){
        // Convert the Date object to a string in the format "YYYY-MM-DD"
        return to_string(this->year) + "-" + to_string(this->month) + "-" + to_string(this->day);
    }

    unsigned long toNumber(){
        // Convert the Date object to an unsigned long integer in the format YYYYMMDD
        return this->year * 10000 + this->month * 100 + this->day;
    }

    // Overloaded comparison operators for comparing Date objects
    bool operator<(Date date){
        if(this->year < date.year || (this->year == date.year && this->month < date.month) || (this->year == date.year && this->month == date.month && this->day < date.day))
            return true;
        else
            return false;
    }

    bool operator>(Date date){
        if(this->year > date.year || (this->year == date.year && this->month > date.month) || (this->year == date.year && this->month == date.month && this->day > date.day))
            return true;
        else
            return false;
    }

    bool operator==(Date date){
        if(this->year == date.year && this->month == date.month && this->day == date.day)
            return true;
        else
            return false;
    }

    bool operator<=(Date date){
        if(this->year < date.year || (this->year == date.year && this->month < date.month) || (this->year == date.year && this->month == date.month && this->day <= date.day))
            return true;
        else
            return false;
    }

    bool operator>=(Date date){
        if(this->year > date.year || (this->year == date.year && this->month > date.month) || (this->year == date.year && this->month == date.month && this->day >= date.day))
            return true;
        else
            return false;
    }
};


class Movie{
    public:
        //Id Attributes
        int id;
        string title;

        //Indexing Attributes
        int runtime;
        double vote_average;
        double popularity;
        string original_language;
        Date release_date;

        //LSH Text Attribute
        vector<string> production_companies;

    bool operator==(Movie movie){
        // Checking if all indexing attributes of the two Movie objects are equal
        if(this->runtime == movie.runtime && this->vote_average == movie.vote_average && this->popularity == movie.popularity && this->original_language == movie.original_language && this->release_date == movie.release_date)
            return true;
        else
            return false;
    }

    bool inRange(int min_runtime, int max_runtime, double min_vote_average, double max_vote_average, double min_popularity, double max_popularity, vector<string> original_languages, Date min_release_date, Date max_release_date){
        // Checking if the Movie arithmetic fields falls within the specified range of indexing attributes
        bool runtime_flag = (this->runtime >= min_runtime && this->runtime <= max_runtime);
        bool vote_average_flag = (this->vote_average >= min_vote_average && this->vote_average <= max_vote_average);
        bool popularity_flag = (this->popularity >= min_popularity && this->popularity <= max_popularity);

        // Checking if the Movie original_language field falls within the specified range of indexing attributes
        bool language_flag = false;
        if(find(original_languages.begin(), original_languages.end(), "*") != original_languages.end()){
            // If the original_languages vector contains "*", it means any language is acceptable
            language_flag = true;
        }else{
            // If the original_languages vector does not contain "*", iterating through the original_languages vector to check if the Movie original_language is present
            for(int i=0; i<original_languages.size(); i++){
                if(find(original_languages.begin(), original_languages.end(), this->original_language) != original_languages.end()){
                    language_flag = true;
                    break;
                }
            }
        }

        // Checking if the Movie release_date field falls within the specified range of indexing attributes, using the overloaded comparison operators of the Date class
        bool date_flag = (this->release_date >= min_release_date && this->release_date <= max_release_date);

        return (runtime_flag && vote_average_flag && popularity_flag && language_flag && date_flag);
    }

    double getDistance(Movie movie){
        // Calculating the Euclidean distance between the current Movie object and another Movie object based on their indexing attributes
        // original_language and release_date fields are converted to numerical values for distance calculation
        double distance = 0;
        distance += pow(this->runtime - movie.runtime, 2);
        distance += pow(this->vote_average - movie.vote_average, 2);
        distance += pow(this->popularity - movie.popularity, 2);
        distance += pow(stringToInt(this->original_language) - stringToInt(movie.original_language), 2);
        distance += pow(this->release_date.toNumber() - movie.release_date.toNumber(), 2);
        return sqrt(distance);
    }
};


int getCsvLength(ifstream &fin){
    int i = 0;
    // Count the number of lines in the CSV file
    if(fin.is_open()){
        fin.clear();
        fin.seekg(0, std::ios::beg);

        string temp_string;
        while (getline(fin, temp_string)){
            i++;
        }
    }else{
        cout << "Error in reading file";
    }

    fin.clear();
    fin.seekg(0, std::ios::beg);

    return i-1;
}


void readCsvMovies(ifstream &fin, vector<Movie> &movies){
    int csv_length = getCsvLength(fin);
    movies.clear();

    // Reading and discarding the header line of the CSV file
    string temp_string, field;
    getline(fin, temp_string);

    // Reading each line of the CSV file and parsing the fields into Movie objects
    for(int i=0; i<csv_length; i++){
        Movie new_element;

        getline(fin,temp_string);
        stringstream line(temp_string);
        for(int j=1; j<=14; j++){
            bool flag = false;
            if(line.peek() == '"'){
                getline(line, field, '"');
                getline(line, field, '"');
                flag = true;
            }else{
                getline(line, field, ';');
            }
            
            if(j == 1){
                new_element.id = stoi(field);
            }else if(j == 2){
                new_element.title = field;
            }else if(j == 4){
                new_element.original_language = field;
            }else if(j == 6){
                new_element.release_date = Date(field);
            }else if(j == 8){
                stringstream production_companies_stream(field);
                string production_company;
                while(getline(production_companies_stream, production_company, ',')){
                    int start = production_company.find('\'');
                    int end   = production_company.rfind('\'');
                    if(start != string::npos && end != string::npos){
                        production_company = production_company.substr(start + 1, end - start - 1);
                        new_element.production_companies.push_back(production_company);
                    }
                }
            }else if(j == 11){
                new_element.runtime = stoi(field);
            }else if(j == 12){
                replace(field.begin(), field.end(), ',', '.');
                new_element.popularity = stod(field);
            }else if(j == 13){
                replace(field.begin(), field.end(), ',', '.');
                new_element.vote_average = stod(field);
            }

            if(flag == true){
                getline(line, field, ';');
            }
        }

        movies.push_back(new_element);
    }
}


void readCsvResult(ifstream &fin, vector<Movie> &movies){
    int csv_length = getCsvLength(fin);
    movies.clear();
    
    // Reading and discarding the header line of the CSV file
    string temp_string, field;
    getline(fin, temp_string);

    // Reading each line of the CSV file and parsing the fields into Movie objects
    for(int i=0; i<csv_length; i++){
        Movie new_element;
        getline(fin,temp_string);
        stringstream line(temp_string);
        for(int j=1; j<=8; j++){
            bool flag = false;
            if(line.peek() == '"'){
                getline(line, field, '"');
                getline(line, field, '"');
                flag = true;
            }else{
                getline(line, field, ';');
            }

            if(j == 1){
                new_element.id = stoi(field);
            }else if(j == 2){
                new_element.title = field;
            }else if(j == 3){
                new_element.runtime = stoi(field);
            }else if(j == 4){
                new_element.vote_average = stod(field);
            }else if(j == 5){
                new_element.popularity = stod(field);
            }else if(j == 6){
                new_element.original_language = field;
            }else if(j == 7){
                new_element.release_date = Date(field);
            }else if(j == 8){
                stringstream production_companies_stream(field);
                string production_company;
                while(getline(production_companies_stream, production_company, ',')){
                    int start = production_company.find('\'');
                    int end   = production_company.rfind('\'');
                    if(start != string::npos && end != string::npos){
                        production_company = production_company.substr(start + 1, end - start - 1);
                        new_element.production_companies.push_back(production_company);
                    }
                }
            }

            if(flag == true){
                getline(line, field, ';');
            }
        }

        movies.push_back(new_element);
    }
}
