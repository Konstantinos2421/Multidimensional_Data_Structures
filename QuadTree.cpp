#include "utilities.cpp"

#define MAX_LEAF_CAPACITY 16

using namespace std;
ofstream fout1("results\\tree_nodes.csv");
ofstream fout2("results\\query_result.csv");

class Node{
    public:
        // Bounding box of the node, defined by the minimum and maximum values of the indexing attributes
        int min_runtime, max_runtime;
        double min_vote_average, max_vote_average;
        double min_popularity, max_popularity;
        unsigned long min_original_language, max_original_language;
        unsigned long min_release_date, max_release_date;

        bool is_leaf;
        vector<Node*> children;
        Node *parent_node;
        vector<Movie> movies;

    Node(){
        this->is_leaf = false;
        this->children = vector<Node*>();
        this->parent_node = nullptr;
        this->movies = vector<Movie>();
    }

    Node(bool is_leaf) : Node(){
        this->is_leaf = is_leaf;
    }

    bool contains(Movie movie){
        // Checking if the movie is within the bounding box of the node
        return (movie.runtime >= this->min_runtime && movie.runtime <= this->max_runtime &&
                movie.vote_average >= this->min_vote_average && movie.vote_average <= this->max_vote_average &&
                movie.popularity >= this->min_popularity && movie.popularity <= this->max_popularity &&
                stringToInt(movie.original_language) >= this->min_original_language && stringToInt(movie.original_language) <= this->max_original_language &&
                movie.release_date.toNumber() >= this->min_release_date && movie.release_date.toNumber() <= this->max_release_date);
    }

    bool contains(int min_runtime, int max_runtime, double min_vote_average, double max_vote_average, double min_popularity, double max_popularity, unsigned long min_original_language, unsigned long max_original_language, unsigned long min_release_date, unsigned long max_release_date){
        // Checking if the bounding box defined by the given arguments is within the bounding box of the node
        return (min_runtime <= this->max_runtime && max_runtime >= this->min_runtime &&
                min_vote_average <= this->max_vote_average && max_vote_average >= this->min_vote_average &&
                min_popularity <= this->max_popularity && max_popularity >= this->min_popularity &&
                min_original_language <= this->max_original_language && max_original_language >= this->min_original_language &&
                min_release_date <= this->max_release_date && max_release_date >= this->min_release_date);
    }

    Node* selectSubtree(Movie movie){
        // Selecting the appropriate child node for the given movie based on in which child node's bounding box the movie falls into
        for(int i=0; i<this->children.size(); i++){
            if(this->children[i]->contains(movie)){
                return this->children[i];
            }
        }
        return nullptr;
    }
};


class QuadTree{
    public:
        Node *root;

    QuadTree(){
        this->root = nullptr;
    }

    QuadTree(ifstream &fin){
        this->root = nullptr;
        vector<Movie> movies;
        readCsvMovies(fin, movies);

        // Calculating the minimum and maximum values of the indexing attributes of all movies to define the bounding box of the root node
        int min_runtime = min_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return a.runtime < b.runtime;})->runtime;
        int max_runtime = max_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return a.runtime < b.runtime;})->runtime;
        double min_vote_average = min_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return a.vote_average < b.vote_average;})->vote_average;
        double max_vote_average = max_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return a.vote_average < b.vote_average;})->vote_average;
        double min_popularity = min_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return a.popularity < b.popularity;})->popularity;
        double max_popularity = max_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return a.popularity < b.popularity;})->popularity;
        unsigned long min_original_language = stringToInt(min_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return stringToInt(a.original_language) < stringToInt(b.original_language);})->original_language);
        unsigned long max_original_language = stringToInt(max_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return stringToInt(a.original_language) < stringToInt(b.original_language);})->original_language);
        unsigned long min_release_date = min_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return a.release_date.toNumber() < b.release_date.toNumber();})->release_date.toNumber();
        unsigned long max_release_date = max_element(movies.begin(), movies.end(), [](Movie a, Movie b) {return a.release_date.toNumber() < b.release_date.toNumber();})->release_date.toNumber();

        // Creating the root node of the QuadTree with the calculated bounding box
        this->root = new Node(true);
        this->root->parent_node = nullptr;
        this->root->min_runtime = min_runtime;
        this->root->max_runtime = max_runtime;
        this->root->min_vote_average = min_vote_average;
        this->root->max_vote_average = max_vote_average;
        this->root->min_popularity = min_popularity;
        this->root->max_popularity= max_popularity;
        this->root->min_original_language = min_original_language;
        this->root->max_original_language = max_original_language;
        this->root->min_release_date= min_release_date;
        this->root->max_release_date= max_release_date;

        // Inserting all movies into the QuadTree
        for(int i=0; i<movies.size(); i++){
            Insert(movies[i]);
        }
    }

    void Insert(Movie movie);

    void Delete(Movie movie);

    void Update(Movie old_movie, Movie new_movie);

    Movie* Search(Movie movie);

    void RangeSearch(Node *v, vector<Movie> &result, int min_runtime, int max_runtime, double min_vote_average, double max_vote_average, double min_popularity, double max_popularity, vector<string> &original_languages, Date min_release_date, Date max_release_date);

    void Split(Node *v);

    void printElements(Node *v);
};


void QuadTree::Insert(Movie movie){
    Node *v = this->root;
    
    // Traversing the tree to find the appropriate leaf node for the given movie based on its indexing attributes
    while(!v->is_leaf){
        v = v->selectSubtree(movie);
    }
    v->movies.push_back(movie);
    
    // If the number of movies in the leaf node exceeds the maximum capacity, the node is split into child nodes
    if(v->movies.size() > MAX_LEAF_CAPACITY){
        Split(v);
    }
}


void QuadTree::Delete(Movie movie){
    Node *v = this->root;

    // Searching for the appropriate leaf node that contains the given movie
    while(!v->is_leaf){
        v = v->selectSubtree(movie);
    }

    // Removing the movie from the leaf node's movies vector
    for(int i=0; i<v->movies.size(); i++){
        if(v->movies[i] == movie){
            v->movies.erase(v->movies.begin() + i);
            break;
        }
    }
}


void QuadTree::Update(Movie old_movie, Movie new_movie){
    // The movie is deleted from the tree and the updated movie is re-inserted into the tree
    Delete(old_movie);
    Insert(new_movie);
}


Movie* QuadTree::Search(Movie movie){
    Node *v = this->root;

    // Traversing the tree to find the appropriate leaf node that contains the given movie
    while(!v->is_leaf){
        v = v->selectSubtree(movie);
    }

    // Searching for the movie in the leaf node's movies vector and returning a pointer to it if found
    for(int i=0; i<v->movies.size(); i++){
        if(v->movies[i] == movie){
            return &v->movies[i];
        }
    }

    return nullptr;
}


void QuadTree::RangeSearch(Node *v, vector<Movie> &result, int min_runtime, int max_runtime, double min_vote_average, double max_vote_average, double min_popularity, double max_popularity, vector<string> &original_languages, Date min_release_date, Date max_release_date){
    // Sorting the original_languages vector to find the minimum and maximum values of the original_language indexing attribute
    if(v == this->root){
        sort(original_languages.begin(), original_languages.end());
    }

    // Converting the min and max values of the original_language indexing attribute to integers
    unsigned long min_original_language, max_original_language;
    if(find(original_languages.begin(), original_languages.end(), "*") != original_languages.end()){
        min_original_language = MIN_ULONG;
        max_original_language = MAX_ULONG;
    }else{
        min_original_language = stringToInt(original_languages[0]);
        max_original_language = stringToInt(original_languages[original_languages.size()-1]);
    }
    
    if(v->is_leaf){
        // If the current node is a leaf node, checking each movie in the node's movies vector to see if it falls within the specified range and adding it to the result vector if it does
        for(int i=0; i<v->movies.size(); i++){
            if(v->movies[i].inRange(min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date)){
                result.push_back(v->movies[i]);
            }
        }
    }else{
        // If the current node is not a leaf node, recursively calling the RangeSearch function on each child node that may contain movies within the specified range
        for(int i=0; i<v->children.size(); i++){
            if(v->children[i]->contains(min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, min_original_language, max_original_language, min_release_date.toNumber(), max_release_date.toNumber())){
                RangeSearch(v->children[i], result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);
            }
        }
    }
}


void QuadTree::Split(Node *v){
    vector<bool> movie_assigned(v->movies.size(), false);

    // Calculating the midpoints of the bounding box of the node to create child nodes that partition the space
    int mid_runtime = (v->min_runtime + v->max_runtime) / 2;
    double mid_vote_average = (v->min_vote_average + v->max_vote_average) / 2;
    double mid_popularity = (v->min_popularity + v->max_popularity) / 2;
    unsigned long mid_original_language = (v->min_original_language + v->max_original_language) / 2;
    unsigned long mid_release_date = (v->min_release_date + v->max_release_date) / 2;

    // Creating 32 child nodes that partition the space based on the midpoints of the bounding box, using bitmasking
    v->is_leaf = false;
    for(int i=0; i<32; i++){
        Node *new_node = new Node(true);
        new_node->parent_node = v;

        if(i & 1){
            new_node->min_runtime = mid_runtime;
            new_node->max_runtime = v->max_runtime;
        }else{
            new_node->min_runtime = v->min_runtime;
            new_node->max_runtime = mid_runtime;
        }

        if(i & 2){
            new_node->min_vote_average = mid_vote_average;
            new_node->max_vote_average = v->max_vote_average;
        }else{
            new_node->min_vote_average = v->min_vote_average;
            new_node->max_vote_average = mid_vote_average;
        }

        if(i & 4){
            new_node->min_popularity = mid_popularity;
            new_node->max_popularity = v->max_popularity;
        }else{
            new_node->min_popularity = v->min_popularity;
            new_node->max_popularity = mid_popularity;
        }

        if(i & 8){
            new_node->min_original_language = mid_original_language;
            new_node->max_original_language = v->max_original_language;
        }else{
            new_node->min_original_language = v->min_original_language;
            new_node->max_original_language = mid_original_language;
        }

        if(i & 16){
            new_node->min_release_date = mid_release_date;
            new_node->max_release_date = v->max_release_date;
        }else{
            new_node->min_release_date = v->min_release_date;
            new_node->max_release_date = mid_release_date;
        }

        // Assigning movies from the parent node to the appropriate child nodes based on which child node's bounding box contains the movie
        v->children.push_back(new_node);
        for(int j=0; j<v->movies.size(); j++){
            if(new_node->contains(v->movies[j]) && !movie_assigned[j]){
                new_node->movies.push_back(v->movies[j]);
                movie_assigned[j] = true;
            }
        }
    }

    // Clearing the movies vector of the parent node after the movies have been assigned to the child nodes
    v->movies.clear();
}


void QuadTree::printElements(Node *v){
    // Printing the elements of the QuadTree in a CSV format, starting from the root node and recursively traversing the tree to print the movies in each leaf node
    if(v == this->root){
        fout1 << "id;title;runtime;vote_average;popularity;original_language;release_date;production_companies" << endl;
    }
    
    if(v->is_leaf){
        for(int i=0; i<v->movies.size(); i++){
            fout1 << v->movies[i].id << ";" << v->movies[i].title << ";" << v->movies[i].runtime << ";" << v->movies[i].vote_average << ";"<< v->movies[i].popularity << ";" << v->movies[i].original_language << ";" << v->movies[i].release_date.toString() << ";";
            fout1 << "[";
            for(int j=0; j<v->movies[i].production_companies.size(); j++){
                fout1 << "\'" << v->movies[i].production_companies[j] << "\'";
                if(j != v->movies[i].production_companies.size()-1){
                    fout1 << ",";
                }
            }
            fout1 << "]" << endl;
        }
        return;
    }
    
    for(int i=0; i<v->children.size(); i++){
        printElements(v->children[i]);
    }
}


void queryResultToCsv(vector<Movie> result){
    // Writing a query result to a CSV file
    fout2 << "id;title;runtime;vote_average;popularity;original_language;release_date;production_companies" << endl;
    for(int i=0; i<result.size(); i++){
        fout2 << result[i].id << ";" << result[i].title << ";" << result[i].runtime << ";" << result[i].vote_average << ";" << result[i].popularity << ";" << result[i].original_language << ";" << result[i].release_date.toString() << ";";
        fout2 << "[";
        for(int j=0; j<result[i].production_companies.size(); j++){
            fout2 << "\'" << result[i].production_companies[j] << "\'";
            if(j != result[i].production_companies.size()-1){
                fout2 << ",";
            }
        }
        fout2 << "]" << endl;
    }
}


int main(){
    ifstream fin("movies csv data\\data_movies_clean.csv");

    auto start = std::chrono::high_resolution_clock::now();
    QuadTree tree(fin);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    cout << "QuadTree Construction Time: " << duration.count() << " seconds" << std::endl;

    tree.printElements(tree.root);

    int min_runtime = 0;
    int max_runtime = 100;
    double min_vote_average = 6.0;
    double max_vote_average = 10.0;
    double min_popularity = 2.0;
    double max_popularity = 8000.0;
    vector<string> original_languages = {"en"};
    Date min_release_date = Date("2019-01-01");
    Date max_release_date = Date("2020-12-31");

    vector<Movie> result;
    tree.RangeSearch(tree.root, result, min_runtime, max_runtime, min_vote_average, max_vote_average, min_popularity, max_popularity, original_languages, min_release_date, max_release_date);

    cout << "Query Result Size: " << result.size() << endl;
    queryResultToCsv(result);
}
